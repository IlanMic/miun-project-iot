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
	
#define PORT	5683
#define MAXLINE 2048
	
using namespace std;

//Get the header of the message
string getHeader(string buffer)
{
    string requestHeader;
    requestHeader = buffer.substr(0, buffer.find(0b11111111));
    return requestHeader;
}

//Get the content of the payload from the message
string getPayload(string buffer){
    string payload;
    payload = buffer.substr(1 + buffer.find(0b11111111));
    return payload;
}

//GET Request 
string get(string path)
{
	int sizePathLength = 4;

	string pathToResource = path;

	//The string message will contain the message of the request
	string message= "";

    //Version 1 "01", Type NON-Confirmable "01" and no token "0000"
    unsigned char messageInformation = 0b01010000;

    //GET method "0000 0001"
    unsigned char code = 0b00000001;

	//Random message IDs
    unsigned char messageID[] = {0b00000000, 0b00000001};

	//Delta -> Uri-Host: 3 (0011) / Length -> coap.me: 7 (0111)
    unsigned char optionHost = 0b00110111;

	//Host: coap.me
	unsigned char host[] = { 0b01100011, 0b01101111, 0b01100001, 0b01110000, 0b00101110, 0b01101101, 0b01100101 };

	unsigned char optionResource;
	int pathLength = path.length();
	if(pathLength == 4)
	{
		optionResource = 0b10000100;
	}
	else if (pathLength == 5)
	{
		optionResource = 0b10000101;
	}
	pathToResource = path;

	//Write the message with the different char
	message.push_back(messageInformation);
	message.push_back(code);
	
	for(char id: messageID)
	{
		message.push_back(id);
	}

	message.push_back(optionHost);

	for(char h: host)
	{
		message.push_back(h);
	}

	message.push_back(optionResource);

	message += pathToResource;

	return message;
}

//POST request
string post(string path, string input_to_post)
{
	string path_to_post_on, element_to_post;

	//The string message will contain the message of the request
	string message = "";

	//Version 1 "01", Type NON-Confirmable "01" and no token "0000"
    unsigned char messageInformation = 0b01010000;

    //POST method "0000 0010"
    unsigned char code = 0b00000010;

	//Random message IDs
    unsigned char messageID[] = {0b00000010, 0b00000011};

	//Delta -> Uri-Host: 3 (0011) / Length -> coap.me: 7 (0111)
    unsigned char optionHost = 0b00110111;

	//Host: coap.me
	unsigned char host[] = { 0b01100011, 0b01101111, 0b01100001, 0b01110000, 0b00101110, 0b01101101, 0b01100101 };


	unsigned char optionResource;
	int pathLength = path.length();
	if(pathLength == 4)
	{
		optionResource = 0b10000100;
	}
	else if (pathLength == 5)
	{
		optionResource = 0b10000101;
	}
	path_to_post_on = path;

	unsigned char splitter = 0b11111111;

	unsigned char options_values = 0b00010000;

	//Write the message with the different char
	message.push_back(messageInformation);
	message.push_back(code);
	
	for(char id: messageID)
	{
		message.push_back(id);
	}

	message.push_back(optionHost);

	for(char h: host)
	{
		message.push_back(h);
	}

	message.push_back(optionResource);


	message += path_to_post_on;

	message.push_back(options_values);

	message.push_back(splitter);

	message += input_to_post;

	return message;
}

//PUT request
string put(string path, string edited_element)
{
	string path_to_put_on, element_to_put;

	//The string message will contain the message of the request
	string message = "";

	//Version 1 "01", Type NON-Confirmable "01" and no token "0000"
    unsigned char messageInformation = 0b01010000;

    //PUT method "0000 0011"
    unsigned char code = 0b00000011;

	//Random message IDs
    unsigned char messageID[] = {0b00000010, 0b00000011};

	//Delta -> Uri-Host: 3 (0011) / Length -> coap.me: 7 (0111)
    unsigned char optionHost = 0b00110111;

	//Host: coap.me
	unsigned char host[] = { 0b01100011, 0b01101111, 0b01100001, 0b01110000, 0b00101110, 0b01101101, 0b01100101 };

	unsigned char optionResource;
	int pathLength = path.length();
	if(pathLength == 4)
	{
		optionResource = 0b10000100;
	}
	else if (pathLength == 5)
	{
		optionResource = 0b10000101;
	}
	path_to_put_on = path;

	unsigned char splitter = 0b11111111;

	unsigned char options_values = 0b00010000;

	//Write the message with the different char
	message.push_back(messageInformation);
	message.push_back(code);
	
	for(char id: messageID)
	{
		message.push_back(id);
	}

	message.push_back(optionHost);

	for(char h: host)
	{
		message.push_back(h);
	}

	message.push_back(optionResource);

	/*for(char res: resource)
	{
		message.push_back(res);
	}*/

	message += path_to_put_on;

	message.push_back(options_values);

	message.push_back(splitter);

	message += edited_element;

	return message;
}

//DELETE request
string del()
{
	//The string message will contain the message of the request
	string message = "";

	//Resource: sink
	//unsigned char resource[] = { 0b01110011, 0b01101001, 0b01101110, 0b01101011 };
	string path_to_delete_from = "sink";

	//Version 1 "01", Type NON-Confirmable "01" and no token "0000"
    unsigned char messageInformation = 0b01010000;

    //DELETE method "0000 0100"
    unsigned char code = 0b00000100;

	//Random message IDs
    unsigned char messageID[] = {0b00000010, 0b00000011};

	//Delta -> Uri-Host: 3 (0011) / Length -> coap.me: 7 (0111)
    unsigned char optionHost = 0b00110111;

	//Host: coap.me
	unsigned char host[] = { 0b01100011, 0b01101111, 0b01100001, 0b01110000, 0b00101110, 0b01101101, 0b01100101 };

	//Delta -> Location-Path (1000) / Length -> sink: 4 (0100)
	unsigned char optionResource = 0b10000100;

	//Write the message with the different char
	message.push_back(messageInformation);
	message.push_back(code);
	
	for(char id: messageID)
	{
		message.push_back(id);
	}

	message.push_back(optionHost);

	for(char h: host)
	{
		message.push_back(h);
	}

	message.push_back(optionResource);
	message += path_to_delete_from;

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
	
	// Creating socket file descriptor
	if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	memset(&servaddr, 0, sizeof(servaddr));
		
	// Filling server information
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(PORT);
	servaddr.sin_addr.s_addr = inet_addr("134.102.218.18");
	
	//printf("Server : %s\n", buffer);

	//CoAP menu
	bool menuLoop = true;
	while(menuLoop == true)
	{
		int choice;
		cout << "What do you want to do ?\n 1. Get a resource. \n 2. Post a resource. \n 3. Update a resource. \n 4. Delete a resource. \n 5. Exit the menu. \n\n" << endl;
		cin >> choice;
		switch(choice)
		{
			case 1:
				//Getting the path to execute the request on
				cout << "Input the path of the resource you want to get" << endl;
				cin >> path_get;
				msg = get(path_get);

				//Sending the request
				sendto(sockfd, msg.c_str(), msg.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
				n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
				buffer[n] = '\0';
				cout << "Header: \n" << getHeader(buffer) << "\n\nPayload: \n" << getPayload(buffer) << "\n\n" << endl;
				break;
			case 2:
				//Getting the path to execute the request on
				cout << "Input the path on which you want to post on :\n" << endl;
				cin >> path_post;
				cout << "Input the element you want to post:\n" << endl;
				cin >> posted_element;
				msg = post(path_post, posted_element);

				//Sending the request
				sendto(sockfd, msg.c_str(), msg.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
				n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
				buffer[n] = '\0';
				cout << "Header: \n" << getHeader(buffer) << "\n\nPayload: \n" << getPayload(buffer) << "\n\n" << endl;
				break;
			case 3:
				//Getting the path to execute the request on
				cout << "Input the path on which you want to put:\n" << endl;
				cin >> path_put;
				cout << "Input the new element:\n" << endl;
				cin >> put_element;
				msg = put(path_put, put_element);

				//Sending the request
				sendto(sockfd, msg.c_str(), msg.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
				n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
				buffer[n] = '\0';
				cout << "Header: \n" << getHeader(buffer) << "\n\nPayload: \n" << getPayload(buffer) << "\n\n" << endl;
				break;
			case 4:
				msg = del();

				//Sending the request
				sendto(sockfd, msg.c_str(), msg.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
				n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
				buffer[n] = '\0';
				cout << "Header: \n" << getHeader(buffer) << "\n\nPayload: \n" << getPayload(buffer) << "\n\n" << endl;
				break;
			case 5:
				cout << "Exiting the menu... Goodbye" << endl;
				menuLoop = false;
				break;
			default:
				cout << "Wrong input. Try again!" << endl;
				break;
		}
	}
	
	close(sockfd);
	return 0;
}
