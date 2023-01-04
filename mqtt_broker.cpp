#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <pthread.h>
#include <map>
#include <list>
#include <algorithm>

#define PORT 1883
#define BUFFER_SIZE 1024

using namespace std;

typedef struct
{
	int sock;
	struct sockaddr address;
	socklen_t addr_len;
} connection_t;

/* Creating the map with subscriptions by sockets */
map<string,list<int>> map_of_subscriptions_per_socket;

/* Creating the map with retained publications per topics*/
map<string,string> map_of_retained_publications;

/*
* This function's purpose is to return the int value of the request type 
* For example:
* 1 would be Connect,
* 2 would be Connect Acknowledgement,
* 3 would be Publish,
* etc...
*/
uint8_t get_request_type(char buffer[])
{
    uint8_t request_type, request_type_unshifted;
    request_type_unshifted = (int)buffer[0] & 0xf0;
    request_type = request_type_unshifted >> 4;
    return request_type;
}

/*
* This function's purpose is to return the int value of the duplicate message flag
* 0 means that the message isn't a duplicate
* 1 means that the message is a duplicate
*/
uint8_t get_duplicate_message_flag(char buffer[])
{
    uint8_t dup, dup_unshifted;
    dup_unshifted = (int)buffer[0] & 0x08;
    dup = dup_unshifted >> 3;
    return dup;
}

/*
* This function's purpose is to return the int value of the QoS level
* 0 means that it's a at-most-once delivery, no guarantee of delivery, "Fire and Forget"
* 1 means that it's a at-least-once delivery, with acknowledged delivery
* 2 means exactly-once delivery
*/
uint8_t get_qos_level(char buffer[])
{
    uint8_t qos_level, qos_level_unshifted;
    qos_level_unshifted = (int)buffer[0] & 0x06;
    qos_level = qos_level_unshifted >> 1;
    return qos_level;
}

/*
* This function's purpose is to return the int value for the retain parameter
* 1 means that the server has to retain the last received PUBLISH message and deliver it as a 
* first message to new subscriptions
*/
uint8_t get_retain(char buffer[])
{
    uint8_t retain;
    retain = (int)buffer[0] & 0x01;
    return retain; 
}

/*
* This function's purpose is to return the int value of the remaining length of the message
*
*/
uint8_t get_remaining_length(char buffer[])
{
	uint8_t remaining_length;
	remaining_length = (int)buffer[1];
	return remaining_length;
}

/*
* This function's purpose is to retrieve the packet identifier of the subscription request
* It also works for the unsubscription request.
*/
unsigned short get_packet_identifier(char buffer[])
{
	unsigned char packet_identifier_msb, packet_identifier_lsb;
	packet_identifier_lsb = buffer[3];
	packet_identifier_msb = buffer[2];
	unsigned short packet_identifier = short((uint16_t) packet_identifier_msb << 8 | (uint16_t) packet_identifier_lsb);
	return packet_identifier;
}

/*
* This function's purpose is to retrieve the packet identifier MSB of the subscription request
* It also works for the unsubscription request.
*/
int get_packet_identifier_msb(char buffer[])
{
	unsigned char packet_identifier_msb, packet_identifier_lsb;
	packet_identifier_msb = buffer[2];
	int packet_identifier = int(packet_identifier_msb);
	return packet_identifier;
}

/*
* This function's purpose is to retrieve the packet identifier LSB of the subscription request
* It also works for the unsubscription request.
*/
int get_packet_identifier_lsb(char buffer[])
{
	unsigned char packet_identifier_msb, packet_identifier_lsb;
	packet_identifier_lsb = buffer[3];
	int packet_identifier = int(packet_identifier_lsb);
	return packet_identifier;
}

/*
* This function writes the acknowledgment message for a connection request
*/
string response_connect_ack()
{
	string response_message = "";

	//Type 2, no DUP, QoS 0, no RETAIN
	unsigned char byte_1 = 0x20;

	//2 remaining bytes
	unsigned char remaining_length = 0x02;

	//No session
	unsigned char session = 0x00;

	//code 0: success
	unsigned char result_status = 0x00;

	response_message.push_back(byte_1);
	response_message.push_back(remaining_length);
	response_message.push_back(session);
	response_message.push_back(result_status);

	return response_message;
}

/*
* This function writes the acknowledgement for a subscription request
*/
string response_subscribe_ack(char buffer[])
{
	string response_message = "";

	//Type 9, no DUP, QoS 0, no RETAIN
	unsigned char byte_1 = 0x90;

	//remaining length
	unsigned char remaining_length = 0x03;

	//packet identifier:
	unsigned char packet_identifier_msb = get_packet_identifier_msb(buffer);
	unsigned char packet_identifier_lsb = get_packet_identifier_lsb(buffer);

	//Return code: describes the return code and status of subscription
	unsigned char return_code = 0x00;

	response_message.push_back(byte_1);
	response_message.push_back(remaining_length);
	response_message.push_back(packet_identifier_msb);
	response_message.push_back(packet_identifier_lsb);
	response_message.push_back(return_code);

	return response_message;
}

/*
* This function writes the acknowledgement message for an unsubscription request
*/
string response_unsubscribe_ack(char buffer[])
{
	string response_message = "";

	//Type 10, no DUP, QoS 1, no RETAIN
	unsigned char byte_1 = 0xb2;

	//remaining length
	unsigned char remaining_length = 0x02;

	//packet identifier
	unsigned char packet_identifier_msb = get_packet_identifier_msb(buffer);
	unsigned char packet_identifier_lsb = get_packet_identifier_lsb(buffer);

	response_message.push_back(byte_1);
	response_message.push_back(remaining_length);
	response_message.push_back(packet_identifier_msb);
	response_message.push_back(packet_identifier_lsb);

	return response_message;
}

/*
* This function gets the topic name from the publish message request
*/
string get_topic_name_from_publish(char buffer[])
{
	string topic_name = "";

	//remaining bytes
	unsigned char remaining_length = (int)get_remaining_length(buffer);

	//getting the length of the topic name
	unsigned char topic_name_msb = buffer[2];
	unsigned char topic_name_lsb = buffer[3];
	int topic_name_length = (int)topic_name_msb + (int)topic_name_lsb;

	//getting the name of the topic
	for(int i = 4; i < topic_name_length + 4; i++)
	{
		topic_name += buffer[i];
	}

	return topic_name;
}

/**
 * This function gets the payload from the publish message request
*/
string get_payload_from_publish(char buffer[])
{
	string payload = "";
	//remaining bytes
	unsigned char remaining_length = (int)get_remaining_length(buffer);

	//getting the length of the topic name
	unsigned char topic_name_msb = buffer[2];
	unsigned char topic_name_lsb = buffer[3];
	int topic_name_length = (int)topic_name_msb + (int)topic_name_lsb;

	//getting the payload
	for(int i = topic_name_length + 4; i < remaining_length + 4; i++)
	{
		payload += buffer[i];
	}

	return payload;
}

/**
 * This function gets the topic name for the subscription message request
*/
string get_topic_name_from_subscribe(char buffer[])
{
	string topic_name = "";

	//remaining bytes
	unsigned char remaining_length = (int)get_remaining_length(buffer);

	//getting the length of the topic name
	unsigned char topic_name_msb = buffer[4];
	unsigned char topic_name_lsb = buffer[5];
	int topic_name_length = (int)topic_name_msb + (int)topic_name_lsb;

	//getting the name of the topic
	for(int i = 6; i < topic_name_length + 6; i++)
	{
		topic_name += buffer[i];
	}

	return topic_name;
}

/**
 * This function gets the topic name from the unsubscription message request
*/
string get_topic_name_from_unsubscribe(char buffer[])
{
	string topic_name = "";

	//remaining bytes
	unsigned char remaining_length = (int)get_remaining_length(buffer);

	//getting the length of the topic name
	unsigned char topic_name_msb = buffer[4];
	unsigned char topic_name_lsb = buffer[5];
	int topic_name_length = (int)topic_name_msb + (int)topic_name_lsb;

	//getting the name of the topic
	for(int i = 6; i < topic_name_length + 6; i++)
	{
		topic_name += buffer[i];
	}

	return topic_name;
}

/**
 * This function get the MSB of the packet identifier from the publish message request
*/
int get_packet_identifier_msb_from_publish(char buffer[])
{
	unsigned char packet_identifier_msb, packet_identifier_lsb;

	int topic_name_length = 4 + buffer[2] + buffer [3];
	packet_identifier_msb = buffer[topic_name_length + 1];
	int packet_identifier = int(packet_identifier_msb);
	return packet_identifier;
}

/**
 * This function get the LSB of the packet identifier from the publish message request
*/
int get_packet_identifier_lsb_from_publish(char buffer[])
{
	unsigned char packet_identifier_msb, packet_identifier_lsb;
	int topic_name_length = 4 + buffer[2] + buffer [3];
	packet_identifier_lsb = buffer[topic_name_length + 2];
	int packet_identifier = int(packet_identifier_lsb);
	return packet_identifier;
}

/**
 * This function writes the acknowledgement message for the publish request
*/
string response_publish_ack(char buffer[])
{
	string response_message = "";

	//Type 4, no DUP, QoS 0, no RETAIN
	unsigned char byte_1 = 0x40;

	//remaining bytes
	unsigned char remaining_length = 0x02;

	//getting the packet identifier
	unsigned char packet_identifier_msb = get_packet_identifier_msb_from_publish(buffer);
	unsigned char packet_identifier_lsb =get_packet_identifier_lsb_from_publish(buffer);

	//No session
	unsigned char session = 0x0;

	//code 0: success
	unsigned char result_status = 0x00;

	response_message.push_back(byte_1);
	response_message.push_back(remaining_length);
	response_message.push_back(packet_identifier_msb);
	response_message.push_back(packet_identifier_lsb);
	
	return response_message;
}

/**
 * This function writes the ping response for a ping request
*/
string ping_response()
{
	string response_message = "";

	//Type 13, no DUP, QoS 0, no RETAIN
	unsigned char byte_1 = 0xd0;

	//There is no payload or variable header so 0 remaining byte
	unsigned char remaining_length = 0x00;

	response_message.push_back(byte_1);
	response_message.push_back(remaining_length);

	return response_message;
}

/**
 * This function sends a disconnect request (NOT USED)
*/
string disconnect()
{
	string response_message = "";

	//Type 14, no DUP, QoS 0 & no RETAIN
	unsigned char byte_1 = 0xe0;

	//There is no payload or variable header so 0 remaining byte
	unsigned char remaining_length = 0x00;

	response_message.push_back(byte_1);
	response_message.push_back(remaining_length);

	return response_message;
}

/**
 * This function sends a publish on topic request
 * its purpose is to send a message to every subscriber once this message has been published by one of them
*/
string publish_on_topic(char buffer[])
{
	//writing the publish message send to the other sockets
	string publish_message = "";
	unsigned char byte_1;
	string topic_name = "";
	string content_publication = "";

	//Differenciation of the case with retain or without
	if(get_retain(buffer) == 0)
	{
		byte_1 = 0x32;
	} else if(get_retain(buffer) == 1)
	{
		byte_1 = 0x33;
	}

	//remaining bytes
	unsigned char remaining_length = (int)get_remaining_length(buffer);

	//getting the length of the topic name
	unsigned char topic_name_msb = buffer[2];
	unsigned char topic_name_lsb = buffer[3];
	int topic_name_length = (int)topic_name_msb + (int)topic_name_lsb;

	//getting the packet identifier
	unsigned char packet_identifier_msb = buffer[topic_name_length+5];
	unsigned char packet_identifier_lsb = buffer[topic_name_length+6];

	//writing the message
	publish_message.push_back(byte_1);
	publish_message.push_back(remaining_length);
	publish_message.push_back(topic_name_msb);
	publish_message.push_back(topic_name_lsb);
	
	//name of the topic
	for(int i = 4; i < topic_name_length + 4; i++)
	{
		publish_message.push_back(buffer[i]);
	}

	publish_message.push_back(packet_identifier_msb);
	publish_message.push_back(packet_identifier_lsb);

	//content of the publication
	for(int i = topic_name_length + 6; i < remaining_length + 2; i++)
	{
		publish_message.push_back(buffer[i]);
		content_publication.push_back(buffer[i]);
	}	
	return publish_message;
}


/**
 * writes the retained message sent to those subscribed to a certain topic
*/
string get_retained_message(char topic[])
{
	//Publish, no DUP, QoS 0, no RETAIN
	unsigned char byte_1  = 0x30;
	unsigned char remaining_length;
	unsigned char msb_topic;
	unsigned char lsb_topic;
	unsigned char msb_packet_id;
	unsigned char lsb_packet_id;

	cout << "\nRetained message for: " << topic << endl;
	string message = "";
	string topic_name, retained_msg = "";
	unsigned short topic_name_length, retained_msg_length;

	topic_name = topic;
	topic_name_length = topic_name.length();
	msb_topic = (unsigned)topic_name_length >> 8;
	lsb_topic =	(unsigned)topic_name_length & 0x00ff;

	//Assigning random numbers to the packet identifers byte
	msb_packet_id = (unsigned)(rand() % 255);
	lsb_packet_id = (unsigned)(rand() % 255);

	retained_msg = "";

	for(map<string, string>::iterator it_publication = map_of_retained_publications.begin(); it_publication != map_of_retained_publications.end(); it_publication++)
	{
		if(it_publication->first == topic_name)
		{
			retained_msg += it_publication->second;
			retained_msg.erase(retained_msg.end() - 2, retained_msg.end());
		}
	}

	retained_msg_length = retained_msg.length();
	remaining_length = (unsigned)4 + (unsigned)retained_msg_length + (unsigned)topic_name_length;

	message.push_back(byte_1);
	message.push_back(remaining_length);
	message.push_back(msb_topic);
	message.push_back(lsb_topic);
	message += topic_name;
	message.push_back(msb_packet_id);
	message.push_back(lsb_packet_id);
	message += retained_msg;

	return message;
}

/**
 * This function details the threaded operation (while loop listening on the requests)
*/
void * threaded_operation(void * ptr)
{
	char buffer[1024] = { 0 };
	connection_t * conn;
	long addr = 0;
	string ack_from_broker = "";
	uint8_t request;
	string publish_message = "";
	string publish_retained_after_publication = "";
	string new_sub = "";
	int new_sub_length;
	char* topic_to_subscribe_to;

	if(!ptr) pthread_exit(0);
	conn = (connection_t *)ptr;

	while(buffer[0] != 0xffffffe0)
	{
		addr = (long)((struct sockaddr_in *)&conn->address)->sin_addr.s_addr;

		read(conn->sock, buffer, 1024);

		request = get_request_type(buffer);
		switch(unsigned(request))
		{
			//Case 1: "Connect"
			case 1:
				cout << "\nConnecting..." << endl;
				ack_from_broker = response_connect_ack();
				send(conn->sock, ack_from_broker.c_str(), ack_from_broker.length(), 0);
				cout << "Connection acknowledgment sent" << endl;
				break;

			//Case 3: "Publish"
			case 3:
				cout << "\nPublishing new content..." << endl;
				ack_from_broker = response_publish_ack(buffer);
				send(conn->sock, ack_from_broker.c_str(), ack_from_broker.length(), 0);
				publish_message = publish_on_topic(buffer);
				//cout << "Currently used socket" << conn->sock << endl;
				for(map<string, list<int>>::iterator iterator = map_of_subscriptions_per_socket.begin(); iterator != map_of_subscriptions_per_socket.end(); iterator++)
				{
					if(iterator->first == get_topic_name_from_publish(buffer))
					{
						for(int socket : iterator->second)
						{
							if(socket != conn->sock)
							{
								send(socket, publish_message.c_str(), publish_message.length(), 0);
								cout << "New message sent..." << endl;
							}
						}
					}
				}

				if(get_retain(buffer) == 1)
				{
					if(map_of_retained_publications.find(get_topic_name_from_publish(buffer)) != map_of_retained_publications.end())
					{
						map_of_retained_publications.erase(get_topic_name_from_publish(buffer));
						map_of_retained_publications.insert({get_topic_name_from_publish(buffer), get_payload_from_publish(buffer)});
					}
					else 
					{
						map_of_retained_publications.insert({get_topic_name_from_publish(buffer), get_payload_from_publish(buffer)});
					}
				}
				cout << "Publish acknowledgement sent" << endl;
				break;

			//Case 8: "Suscribe"
			case 8:
				cout << "\nSubscribing to a topic..." << endl;
				ack_from_broker = response_subscribe_ack(buffer);

				send(conn->sock, ack_from_broker.c_str(), ack_from_broker.length(), 0);
				if(map_of_subscriptions_per_socket.find(get_topic_name_from_subscribe(buffer)) != map_of_subscriptions_per_socket.end())
				{
					
					list<int> list_of_sockets_subcribed = map_of_subscriptions_per_socket.find(get_topic_name_from_subscribe(buffer))->second;
					list<int>::iterator itr = find(begin(list_of_sockets_subcribed), end(list_of_sockets_subcribed), conn->sock); 
					if(itr == end(list_of_sockets_subcribed))
					{
						list_of_sockets_subcribed.push_back(conn->sock);
						map_of_subscriptions_per_socket.erase(get_topic_name_from_subscribe(buffer));
						map_of_subscriptions_per_socket.insert({get_topic_name_from_subscribe(buffer), list_of_sockets_subcribed});
					}
				}
				else 
				{
					list<int> new_subscriptions;
					new_subscriptions.push_back(conn->sock);
					map_of_subscriptions_per_socket.insert({get_topic_name_from_subscribe(buffer), new_subscriptions});
				}

				if(!map_of_retained_publications.empty())
				{
					map<string, string>::iterator it_sub = map_of_retained_publications.find(get_topic_name_from_subscribe(buffer)); 
					if(it_sub != map_of_retained_publications.end())
					{
						if(!it_sub->second.empty())
						{
							new_sub = get_topic_name_from_subscribe(buffer);
							new_sub_length = new_sub.length();
							topic_to_subscribe_to = new char[new_sub_length + 1];
							topic_to_subscribe_to[new_sub_length] = '\0';
							for(int i = 0; i < new_sub.length(); i++)
							{
								topic_to_subscribe_to[i] = new_sub[i];
							}
							publish_retained_after_publication = get_retained_message(topic_to_subscribe_to);
							send(conn->sock , publish_retained_after_publication.c_str(), publish_retained_after_publication.length(), 0);
							cout << "New message sent..." << endl;
						}
					}
				}
				cout << "Subscription acknowledgement sent" << endl;
				break;

			//Case 10: "Unsuscribe"
			case 10:
				cout << "\nUnsuscribing from a topic..." << endl;
				ack_from_broker = response_unsubscribe_ack(buffer);
				send(conn->sock, ack_from_broker.c_str(), ack_from_broker.length(), 0);

				if(map_of_subscriptions_per_socket.find(get_topic_name_from_unsubscribe(buffer)) != map_of_subscriptions_per_socket.end())
				{
					//removing the socket from the list
					list<int> list_of_sockets_subcribed = map_of_subscriptions_per_socket.find(get_topic_name_from_unsubscribe(buffer))->second;
					list_of_sockets_subcribed.remove(conn->sock);
					map_of_subscriptions_per_socket.erase(get_topic_name_from_unsubscribe(buffer));
					map_of_subscriptions_per_socket.insert({get_topic_name_from_unsubscribe(buffer), list_of_sockets_subcribed});
				}

				cout << "Removing subscription from a topic" << endl;
				break;

			//Case 12: "Ping Request"
			case 12:
				cout << "\nSending ping response..." << endl;
				ack_from_broker = ping_response();
				send(conn->sock, ack_from_broker.c_str(), ack_from_broker.length(), 0);
				cout << "Ping response sent" << endl;
				break;

			//Case 14: "Disconnect"
			case 14:
				cout << "\nDisconnecting..." << endl;
				cout << "Succesful disconnection" << endl;
				break;

			//Default case
			default:
				break;
		}
	}

	close(conn->sock);
	free(conn);
	pthread_exit(0);
}

int main(int argc, char const* argv[])
{
	int sock = -1;
	struct sockaddr_in address;
	connection_t * connection;
	pthread_t thread;

	/* Creating the socket */
	sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sock <= 0)
	{
		fprintf(stderr, "%s: error: cannot create the socket \n", argv[0]);
		return -3;
	}

	/* bind the socket to the port 1883 */
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(PORT);
	if(bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
	{
		fprintf(stderr, "%s: error: cannot bind the socket to the port %d\n", argv[0], PORT);
		return -4;
	}

	/* listening on the port */
	if(listen(sock, 5) < 0)
	{
		fprintf(stderr, "%s: error: cannot listen on port \n", argv[0]);
		return -5;
	}

	printf("Server starting to listen...");

	while(1)
	{
		/* accept incoming connections */
		connection = (connection_t *)malloc(sizeof(connection_t));
		connection->sock = accept(sock, &connection->address, &connection->addr_len);
		if(connection->sock <= 0)
		{
			free(connection);
		}
		else{
			/* Start a new thread without waiting for it */
			pthread_create(&thread, 0, threaded_operation, (void *)connection);
			pthread_detach(thread);
		}
	}
    return 0;
}