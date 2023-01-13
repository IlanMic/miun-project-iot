// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <MQTTClient.h>
	
#define PORT	5683
#define MAXLINE 2048

/**
 * MQTT client constants such as its address, topic it is subscribes to... 
*/	
const char *ADDRESS = "tcp://127.0.0.1";
const char *TOPIC = "temperature/cpu";
#define QOS         0
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

using namespace std;

//Get the header of the message
string getHeader(string buffer)
{
    string requestHeader;
    requestHeader = buffer.substr(0, buffer.find(0b11111111));
    return requestHeader;
}

//Get the content of the payload from the message
string getPayload(char* buffer, int bufferLength){
    string payload;
	for(int i = 5; i < bufferLength; i++)
	{
		payload += buffer[i];
	} 
    //payload = buffer.substr(1 + buffer.find(0b11111111));
    return payload;
}

//GET Request 
string get_temperature_cpu()
{
	//The string message will contain the message of the request
	string message= "";

    //Version 1 "01", Type NON-Confirmable "01" and no token "0000"
    unsigned char messageInformation = 0b01010000;

    //GET method "0000 0001"
    unsigned char code = 0b00000001;

	//Random message IDs
    unsigned char messageID[] = {0b00000000, 0b00000001};

	//Options for the resource: URI-Path -> 11 / Option length: temp_cpu -> 8
	unsigned char optionForResource = { 0b10111000};

	//path to ressource: temp_cpu
	unsigned char pathToResource[] = { 0b01110100, 0b01100101, 0b01101101, 0b01110000 ,0b01011111, 0b01100011, 0b01110000, 0b01110101 };

	//Write the message with the different char
	message.push_back(messageInformation);
	message.push_back(code);
	
	for(char id: messageID)
	{
		message.push_back(id);
	}

	message.push_back(optionForResource);

	for(char p: pathToResource)
	{
		message.push_back(p);
	}

	return message;
}


// Menu
int main() 
{
	int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in	 servaddr;
	string msg, posted_element, put_element, deleted_element;
	string path_get, path_post, path_put;
	unsigned int n = 0, len;

	int rc;
	MQTTClient mqttClient;
    MQTTClient_connectOptions connectionOptions =     MQTTClient_connectOptions_initializer;
    MQTTClient_message message = MQTTClient_message_initializer;
    MQTTClient_deliveryToken deliveryToken;


    MQTTClient_create(&mqttClient, ADDRESS, "RaspberryPi", 
    MQTTCLIENT_PERSISTENCE_NONE, NULL);

    connectionOptions.keepAliveInterval = 20;
    connectionOptions.cleansession = 1;

    if (MQTTClient_connect(mqttClient, &connectionOptions) != MQTTCLIENT_SUCCESS) {
        printf("Failed to connect\n");
        return (-1);
    }

	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");


	//CoAPClient getting temperature values and MQTTClient transmitting them to MQTTBroker
	bool menuLoop = true;
	while(menuLoop == true)
	{
		cout << "Entering the loop" << endl;
        //Getting the temperature of the CPU from the CoAP server
		msg = get_temperature_cpu();
		cout << "Message formatted, ready to send" << endl;
        //Sending the request
        sendto(sockfd, msg.c_str(), msg.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
        cout << "Message sent to CoAP server" << endl;
		n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
        buffer[n] = '\0';
		sleep(1);
		//cout << "Received measure of the temperature: " << getPayload(buffer, n) << endl;
		const char* payload_in_mqtt = getPayload(buffer, n).c_str();
		message.payload =(void *) payload_in_mqtt;
		message.payloadlen = sizeof(payload_in_mqtt);
		cout << "Waiting to publish the message: " << getPayload(buffer, n).c_str() << endl;
		MQTTClient_publishMessage(mqttClient, TOPIC, &message, &deliveryToken);
		cout << "Message just published" << endl;
		cout << "Restarting the loop\n" << endl;
	}

	MQTTClient_disconnect(mqttClient, 5000);
    MQTTClient_destroy(&mqttClient);
	
	close(sockfd);
	return 0;
}
