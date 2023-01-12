#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ostream>
#include <string>
#include <string.h>
#include <MQTTClient.h>

using namespace std;

const char *ADDRESS = "tcp://127.0.0.1";
const char *TOPIC = "temperature/cpu";

int main(int argc, char* argv[]) {
    const char *MESSAGE_STRING = "Hello, this is a message from the RevPi";

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
    MQTTClient_disconnect(mqttClient, 5000);
    MQTTClient_destroy(&mqttClient);

    return (0);
}

/*
// With the library header files included, continue by defining a main function.
int main()
{
    // In order to connect the mqtt client to a broker, 
    // Define an Ip address pointing to a broker. In this case, the localhost on port 1883.
    string ip = "localhost:1883";
    // Then, define an ID to be used by the client when communicating with the broker.
    std::string id = "consumer";

    // Construct a client using the Ip and Id, specifying usage of MQTT V5.
    MQTT client(ip, id, create_options(MQTTVERSION_5));
    // Use the connect method of the client to establish a connection to the broker.
    client.connect();
    // In order to receive messages from the broker, specify a topic to subscribe to.
    client.subscribe("in");
    // Begin the client's message processing loop, filling a queue with messages.
    client.start_consuming();

    bool running = true;
    while (running)
    {
        // Construct a message pointer to hold an incoming message.
        const_message_ptr messagePointer;

        // Try to consume a message, passing messagePointer by reference.
        // If a message is consumed, the function will return `true`, 
        // allowing control to enter the if-statement body.
        if (client.try_consume_message(&messagePointer))
        {
            // Construct a string from the message payload.
            string messageString = messagePointer -> get_payload_str();
            // Print payload string to console (debugging).
            cout << messageString << std::endl;

            // Perform processing on the string.
            // This is where message processing can be passed onto different
            // functions for parsing. 
            // Here, we break the loop and exit the program if a `quit` is received.
            if (messageString == "quit")
            {
                running = false;
            } 
        }
    }
    return 0;
}
*/