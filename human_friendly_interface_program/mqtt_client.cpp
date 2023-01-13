#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <string>
#include <string.h>
#include <MQTTClient.h>
#include <iostream>


using namespace std;

/**
 * MQTT client constants such as its address, id, topic it is subscribes to... 
*/
#define ADDRESS     "tcp://127.0.0.1"
#define CLIENTID    "mqtt_ed_client"
#define TOPIC       "temperature/cpu"
#define QOS         0
#define TIMEOUT     10000L

volatile MQTTClient_deliveryToken deliveredtoken;

/**
 * This function's purpose is to convert the pointer of a char type character into float
 *      This will be used to convert the payload of the received MQTT message from the 
 *      broker into a readable and comparable value corresponding to the CPU's temperature
*/
float convert_char_ptr_to_float(char * char_ptr)
{
    string string_value_of_temp = char_ptr;
    float received_temperature = stof(string_value_of_temp);
    return received_temperature;
}

/**
 * This function's purpose is to print a specific instruction
 *      based on the CPU's temperature received from the application
 *          
 *  If temperature < 40°, the CPU is too cold
 *  else if 40° <= temperature < 60°, the CPU is at the right temperature
 *  else if 60° <= temperature < 80°, the CPU is too hot but can handle it, the fans should rotate faster
 *  else (temperature >= 80°), the fans should rotate way faster than they currently are doing !!
 * 
*/
int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
	char* payload = (char*)message->payload;
    float temperature = convert_char_ptr_to_float(payload);
    system("clear");
    if(temperature < 40.0)
    {
        cout << "The CPU is too cold (" << temperature <<" °C). The fans should rotate slower.\n" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⠛⠉⠛⠿⠿⠿⠛⠋⠉⠉⠁⠀⠀⠉⠉⠙⠻⠿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣨⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠛⠻⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠙⢿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣶⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⣼⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠠⣀⣀⣀⣀⣀⣀⣀⣠⣤⣾⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠘⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
    } 
    else if(temperature >= 40.0 && temperature < 60.0)
    {
        cout << "The CPU is in right conditions (" << temperature <<" °C). The fans should continue to rotate at the same speed.\n" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠁⠀⠈⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡟⠀⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠀⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠁⠀⠀⠀⢰⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠋⠀⠀⠀⠀⠀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠟⠀⠀⠀⠀⠀⠀⠀⠛⠛⠛⠿⠿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⡿⠿⠿⠿⠋⠀⠀⠀⠀⠀⠀⢀⣤⣤⣀⣀⠀⠀⠀⠙⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⠁⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢰⣟⠁⠈⠉⠙⠛⠓⠀⠀⢸⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⡞⠛⠛⠶⢶⣤⣤⣤⣴⢶⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⡿⠷⣦⣄⡀⠀⠀⠀⠀⠀⣀⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣅⡀⠀⠉⠛⠿⠿⠛⠛⢻⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣷⣾⣄⠀⠀⠀⢠⡾⠋⠛⠷⢶⣤⣤⣤⣤⣴⣾⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣦⣄⣉⠛⠷⢶⣤⣤⣤⣤⣼⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
    }
    else if(temperature >= 60.0 && temperature < 80.0)
    {
        cout << "The CPU is being overwhelmed (" << temperature <<" °C). The fans should rotate faster.\n" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠿⠿⠿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⠀⠀⢹⣿⣿⡿⠛⣉⣠⣤⣶⣶⣶⣶⣶⣶⣤⣄⣉⠛⢿⣿⣿⣏⠀⠀⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣾⣿⠟⣡⡴⠟⠉⠀⠀⠀⢻⣿⣿⣿⡄⠀⠀⠉⠻⢦⣌⠻⣿⣷⣾⣿⣿" << endl;
        cout << "   ⣿⣿⣿⡿⢁⣾⡏⠀⠀⠀⠀⠀⠀⠈⣿⣿⣿⡇⠀⠀⠀⠀⠀⣻⣷⡈⢿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⡟⢠⣿⣿⣿⣷⣶⣤⡀⠀⠀⠀⢸⣿⣿⡇⠀⠀⠀⠀⣴⣿⣿⣿⡄⢻⣿⣿" << endl;
        cout << "   ⣿⡿⠀⣾⡿⠿⠿⢿⣿⣿⣿⣷⣄⣠⣤⣿⣿⡀⠀⠀⣠⣾⣿⣿⣿⠿⣷⠀⢿⣿" << endl;
        cout << "   ⣿⡇⢸⡏⠀⠀⠀⠀⠀⠈⠉⣿⠿⣧⡶⢿⣌⢻⣶⣾⣿⣿⠟⠋⠀⠀⢸⡇⢸⣿" << endl;
        cout << "   ⣿⡇⢸⡇⠀⠀⠀⠀⠀⢀⣀⣿⣸⣏⠀⠀⣹⡏⣿⠉⠁⠀⠀⠀⠀⠀⢸⡇⢸⣿" << endl;
        cout << "   ⣿⡇⢸⡇⠀⠀⣠⣴⣿⣿⡿⠿⣧⡙⣷⠾⢻⣶⣿⣀⡀⠀⠀⠀⠀⠀⣸⡇⢸⣿" << endl;
        cout << "   ⣿⣷⠀⢿⣶⣿⣿⣿⡿⠋⠀⠀⠈⣿⣿⠛⠋⠙⢿⣿⣿⣿⣷⣶⣶⣾⡿⠀⣾⣿" << endl;
        cout << "   ⣿⣿⣧⠘⣿⣿⣿⠟⠀⠀⠀⠀⢸⣿⣿⡇⠀⠀⠀⠈⠛⠿⢿⣿⣿⣿⠃⣼⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣷⡈⢿⣯⠀⠀⠀⠀⠀⢸⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⣸⡿⢁⣾⣿⣿⣿" << endl;
        cout << "   ⣿⣿⡿⢿⣿⣦⡙⠳⣦⣀⠀⠀⠸⣿⣿⣿⣧⠀⠀⠀⣀⣴⠞⢋⣴⣿⡿⢿⣿⣿" << endl;
        cout << "   ⣿⣿⠀⠀⣿⣿⣿⣷⣤⣉⠙⠛⠿⠿⠿⠿⠿⠿⠛⠋⣉⣤⣾⣿⣿⣯⠀⠀⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣶⣶⣶⣶⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
    }
    else
    {
        cout << "The CPU is overheating (" << temperature <<" °C). The fans should either rotate way faster or the desktop should be shut down.\n" << endl; 
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⠛⠉⠛⠿⠿⠿⠛⠋⠉⠉⠁⠀⠀⠉⠉⠙⠻⠿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⠇⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣨⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⠛⠻⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣧⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠙⢿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣶⡄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣀⣠⣼⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠛⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣆⠀⠀⠀⠀⠀⠠⣀⣀⣀⣀⣀⣀⣀⣠⣤⣾⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠙⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣄⠀⠀⠀⠘⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡄⠀⠀⠘⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⠀⠀⢻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿\n\n" << endl;

        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠿⠿⠿⠿⠿⠿⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⠀⠀⢹⣿⣿⡿⠛⣉⣠⣤⣶⣶⣶⣶⣶⣶⣤⣄⣉⠛⢿⣿⣿⣏⠀⠀⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣾⣿⠟⣡⡴⠟⠉⠀⠀⠀⢻⣿⣿⣿⡄⠀⠀⠉⠻⢦⣌⠻⣿⣷⣾⣿⣿" << endl;
        cout << "   ⣿⣿⣿⡿⢁⣾⡏⠀⠀⠀⠀⠀⠀⠈⣿⣿⣿⡇⠀⠀⠀⠀⠀⣻⣷⡈⢿⣿⣿⣿" << endl;
        cout << "   ⣿⣿⡟⢠⣿⣿⣿⣷⣶⣤⡀⠀⠀⠀⢸⣿⣿⡇⠀⠀⠀⠀⣴⣿⣿⣿⡄⢻⣿⣿" << endl;
        cout << "   ⣿⡿⠀⣾⡿⠿⠿⢿⣿⣿⣿⣷⣄⣠⣤⣿⣿⡀⠀⠀⣠⣾⣿⣿⣿⠿⣷⠀⢿⣿" << endl;
        cout << "   ⣿⡇⢸⡏⠀⠀⠀⠀⠀⠈⠉⣿⠿⣧⡶⢿⣌⢻⣶⣾⣿⣿⠟⠋⠀⠀⢸⡇⢸⣿" << endl;
        cout << "   ⣿⡇⢸⡇⠀⠀⠀⠀⠀⢀⣀⣿⣸⣏⠀⠀⣹⡏⣿⠉⠁⠀⠀⠀⠀⠀⢸⡇⢸⣿" << endl;
        cout << "   ⣿⡇⢸⡇⠀⠀⣠⣴⣿⣿⡿⠿⣧⡙⣷⠾⢻⣶⣿⣀⡀⠀⠀⠀⠀⠀⣸⡇⢸⣿" << endl;
        cout << "   ⣿⣷⠀⢿⣶⣿⣿⣿⡿⠋⠀⠀⠈⣿⣿⠛⠋⠙⢿⣿⣿⣿⣷⣶⣶⣾⡿⠀⣾⣿" << endl;
        cout << "   ⣿⣿⣧⠘⣿⣿⣿⠟⠀⠀⠀⠀⢸⣿⣿⡇⠀⠀⠀⠈⠛⠿⢿⣿⣿⣿⠃⣼⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣷⡈⢿⣯⠀⠀⠀⠀⠀⢸⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⣸⡿⢁⣾⣿⣿⣿" << endl;
        cout << "   ⣿⣿⡿⢿⣿⣦⡙⠳⣦⣀⠀⠀⠸⣿⣿⣿⣧⠀⠀⠀⣀⣴⠞⢋⣴⣿⡿⢿⣿⣿" << endl;
        cout << "   ⣿⣿⠀⠀⣿⣿⣿⣷⣤⣉⠙⠛⠿⠿⠿⠿⠿⠿⠛⠋⣉⣤⣾⣿⣿⣯⠀⠀⣿⣿" << endl;
        cout << "   ⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⣶⣶⣶⣶⣶⣶⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿" << endl;
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

/**
 * This functions print that the connection with the MQTT Broker has been lost
 * and the reason why
*/
void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

/**
 * This function confirms the delivery of the message
*/
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}


int main(int argc, char* argv[]) {

    MQTTClient mqttClient;
    MQTTClient_connectOptions connectionOptions =     MQTTClient_connectOptions_initializer;
    MQTTClient_message message = MQTTClient_message_initializer;
    MQTTClient_deliveryToken deliveryToken;
    int rc;

    if((rc = MQTTClient_create(&mqttClient, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL)) != MQTTCLIENT_SUCCESS)
    {
        cout << "Failed to create client, return code " << rc << ".\n" << endl;;
        exit(EXIT_FAILURE);
    }

    if ((rc = MQTTClient_setCallbacks(mqttClient, NULL, connlost, msgarrvd, delivered)) != MQTTCLIENT_SUCCESS)
    {
        cout << "Failed to set callbacks, return code " << rc << ".\n" << endl;;
        rc = EXIT_FAILURE;
        MQTTClient_destroy(&mqttClient);
    }

    connectionOptions.keepAliveInterval = 20;
    connectionOptions.cleansession = 1;

    /**
     * the client connects to the broker
     *  if that would fail, an error code is returned
     */
    if ((rc = MQTTClient_connect(mqttClient, &connectionOptions)) != MQTTCLIENT_SUCCESS) {
        cout << "Failed to connect, return code" << rc << ".\n";
        return (-1);
    }

    /**
     * the client subscribes to the topic "temperature/cpu"
     *  if that would fail, an error code is returned
     */
    if ((rc = MQTTClient_subscribe(mqttClient, TOPIC, QOS)) != MQTTCLIENT_SUCCESS)
    {
        cout << "Failed to subscribe, return code" << rc << ".\n";
        rc = EXIT_FAILURE;
    }
    else
    {
        int ch;
        do
        {
            ch = getchar();
        } while (ch!='Q' && ch != 'q');
        if ((rc = MQTTClient_unsubscribe(mqttClient, TOPIC)) != MQTTCLIENT_SUCCESS)
        {
            cout << "Failed to unsubscribe, return code" << rc << ".\n";
            rc = EXIT_FAILURE;
        }
    }

    /**
     * the client disconnects from the broker
     *  if that would fail, an error code is returned
     */
    if ((rc = MQTTClient_disconnect(mqttClient, 10000)) != MQTTCLIENT_SUCCESS)
    {
        cout << "Failed to unsubscribe, return code" << rc << ".\n";
    }

    MQTTClient_destroy(&mqttClient);

    return (0);
}
