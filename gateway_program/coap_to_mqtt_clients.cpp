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
string getPayload(char* buffer, int bufferLength){
    string payload;
	for(int i = 13; i < bufferLength; i++)
	{
		payload += buffer[i];
	}
    //payload = buffer.substr(1 + buffer.find(0b11111111));
    return payload;
}

/*
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
}*/

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

	//Delta -> Uri-Host: 3 (0011) / Length -> coap.me: 7 (0111)
    //unsigned char optionHost = 0b00110111;

	//Host: coap.me
	//unsigned char host[] = { 0b01100011, 0b01101111, 0b01100001, 0b01110000, 0b00101110, 0b01101101, 0b01100101 };

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

	/*message.push_back(optionHost);

	for(char h: host)
	{
		message.push_back(h);
	}*/

	//message.push_back(optionResource);

	//message += pathToResource;

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

	//CoAP menu
	bool menuLoop = true;
	while(menuLoop == true)
	{
		cout << "Entering the loop" << endl;
        //Getting the temperature of the CPU from the CoAP server
		cout << "Before formatting the message" << endl;
		msg = get_temperature_cpu();
		//cout << "Received value n" << i << ": " << getHeader(buffer) << endl;
        //msg = get_temperature_cpu();
		cout << "After formatting the message + Before sending the message" << endl;
        //Sending the request
        sendto(sockfd, msg.c_str(), msg.length(), MSG_CONFIRM, (const struct sockaddr *) &servaddr, sizeof(servaddr));
		cout << "After sending the message + Receiving the response" << endl;
        n = recvfrom(sockfd, (char *)buffer, MAXLINE, MSG_WAITALL, (struct sockaddr *) &servaddr, &len);
        buffer[n] = '\0';
		cout << "Response received" << endl;
        cout << "Header: \n" << getHeader(buffer) << "\n\nPayload: \n" << getPayload(buffer, n) << "\n\n" << endl;
		cout << "Before the sleep" << endl;
		sleep(1);
		cout << "After the sleep + end of loop" << endl;
	}
	
	close(sockfd);
	return 0;
}
