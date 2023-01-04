// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
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

//Get the temperature from the raspberry
float get_temperature()
{
    string file_name = "/sys/class/thermal/thermal_zone0/temp";
    ifstream temp_raspb_file;
    float temp_raspb = 0.0;
    stringstream buff;

    //we read the temperature of the raspberr's CPU and put it in the buffer
    temp_raspb_file.open(file_name);
    buff << temp_raspb_file.rdbuf();
    temp_raspb_file.close();

    //convert the value of the temperature from string to float
    temp_raspb = stof(buff.str());

    cout << stof(buff.str()) << endl;


    //convert the value of the temperature in celsius
    temp_raspb = temp_raspb / 1000; 
    return temp_raspb;
}

int main()
{
    int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in	 servaddr;
	string msg, posted_element, put_element, deleted_element;
	string path_get, path_post, path_put;
	unsigned int n = 0, len;
    float temperature;
	
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
	while(true)
    {
        temperature == get_temperature();
        if(temperature < 51.0)
        {
            cout << "Sous-regime:" << temperature << endl;
        }
        else {
            cout << "Sur-regime: " << temperature << endl;
        }
        sleep(2);
    }

    close(sockfd);
	return 0;
}