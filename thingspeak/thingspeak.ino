#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Servo.h> 
#define ANGLE_FIELD 0   
#define DATA_FIELD 1                                  // Data field to post the signal strength to.

const char* ssid = "pink pants cricket";
const char* pass = "breezyshrub";
const char* server = "mqtt.thingspeak.com";
char mqttUserName[] = "";                // Use any name.
char mqttPass[] = "";                 // Change to your MQTT API key from Account > MyProfile.
long readChannelID=0000;
char readAPIKey[]="XXXXXXXXXXXXXXXX";
long writeChannelID=1030376;
char writeAPIKey[]   = "F4JYB3C1540KKLZA";

int sensorPin = A0;    // select the input pin for the potentiometer
int sensorValue = 0;  // variable to store the value coming from the sensor

WiFiClient client;                                    // Initialize the Wi-Fi client library.
PubSubClient mqttClient( client );                    // Initialize the PuBSubClient library.
Servo myservo;  // Create servo object to control a servo .

//int fieldsToPublish[8]={1,1,0,0,0,0,0,0};             // Change to allow multiple fields.
//float dataToPublish[8];                               // Holds your field data.
int sensorData;
int changeFlag=0;                                     // Let the main loop know ther is new data to set.

//  
// Prototypes
//

// Handle messages from MQTT subscription.
int mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length);  

// Generate a unique client ID and connect to MQTT broker.
void mqttConnect();  

// Subscribe to a field or feed from a ThingSpeak channel.
//int mqttSubscribe( long subChannelID,int field,char* readKey, int unSub);

// Publish messages to a channel feed.
// void mqttPublish(long pubChannelID, char* pubWriteAPIKey, float dataArray[], int fieldArray[]);

// Connect to a given Wi-Fi SSID
int connectWifi();

// Measure the Wi-Fi signal strength.
void updateRSSIValue();

// Build a random client ID for MQTT connection.
void getID(char clientID[], int idLength);

void setup() {
Serial.begin( 9600 );
Serial.println( "Start" );
int status = WL_IDLE_STATUS; // Set temporary Wi-Fi status.
       
    connectWifi();  // Connect to Wi-Fi network.
    mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.
    //mqttClient.setCallback( mqttSubscriptionCallback );   // Set the MQTT message handler function.


}

void loop() {
    
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    
    if (!mqttClient.connected())
    {
       
       mqttConnect(); // Connect if MQTT client is not connected.
        
         //if(mqttSubscribe( readChannelID,1,readAPIKey,0 )==1 ){
         //       Serial.println( " Subscribed " );
         //   }
    }

        String dataString="";
 
    Serial.println( dataString );
    
    mqttClient.loop(); // Call the loop to maintain connection to the server.  
    
    sensorValue = analogRead(sensorPin);
    dataString = String("field1=") + String(sensorValue, DEC);

    String topicString ="channels/" + String(writeChannelID) + "/publish/"+String(writeAPIKey);
    mqttClient.publish( topicString.c_str(), dataString.c_str() );
    Serial.println( "Published " + String(sensorValue));
    Serial.println( "to channel " + String(writeChannelID ) );

    //mqttPublish( writeChannelID, writeAPIKey, dataToPublish, fieldsToPublish );

    
    delay(300000);
}

/**
 * Process messages received from subscribed channel via MQTT broker.
 *   topic - Subscription topic for message.
 *   payload - Field to subscribe to. Value 0 means subscribe to all fields.
 *   mesLength - Message length.
 */

void mqttConnect()
{
    char clientID[ 9 ];
    
    // Loop until connected.
    while ( !mqttClient.connected() )
    {

        getID(clientID,8);
       
        // Connect to the MQTT broker.
        Serial.print( "Attempting MQTT connection..." );
        //if ( mqttClient.connect( clientID, mqttUserName, mqttPass ) )
        if ( mqttClient.connect( clientID) )

        {
            Serial.println( "Connected with Client ID:  " + String( clientID ) + " User "+ String( mqttUserName ) + " Pwd "+String( mqttPass ) );
           
        } else
        {
            Serial.print( "failed, rc = " );
            // See https://pubsubclient.knolleary.net/api.html#state for the failure code explanation.
            Serial.print( mqttClient.state() );
            Serial.println( " Will try again in 5 seconds" );
            delay( 5000 );
        }
    }
}

/**
 * Build a random client ID.
 *   clientID - Character array for output
 *   idLength - Length of clientID (actual length is one character longer for NULL)
 */

void getID(char clientID[], int idLength){
static const char alphanum[] ="0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";                        // For random generation of the client ID.

    // Generate client ID.
    for (int i = 0; i < idLength ; i++) {
        clientID[ i ] = alphanum[ random( 51 ) ];
    }
    clientID[ idLength ] = '\0';
    
}

/**
 * Subscribe to fields of a channel.
 *   subChannelID - Channel to subscribe to.
 *   field - Field to subscribe to. Value 0 means subscribe to all fields.
 *   readKey - Read API key for the subscribe channel.
 *   unSub - Set to 1 for unsubscribe.
 */
 

void mqttPublish(long pubChannelID, char* pubWriteAPIKey, float dataArray[], int fieldArray[]) {
    int index=0;
    String dataString="";

    
        
    while (index<8){
        
        // Look at the field array to build the posting string to send to ThingSpeak.
        if (fieldArray[ index ]>0){
            dataString+="&field" + String( index+1 ) + "="+String( dataArray [ index ] );
        }
        index++;
    }
    
    Serial.println( dataString );
    
    // Create a topic string and publish data to ThingSpeak channel feed.
    
    String topicString ="channels/" + String( pubChannelID ) + "/publish/"+String( pubWriteAPIKey );
    mqttClient.publish( topicString.c_str(), dataString.c_str() );
    
    Serial.println( "Published to channel " + String( pubChannelID ) );
}

int connectWifi()
{
    WiFi.begin( ssid, pass );
    while ( WiFi.status() != WL_CONNECTED ) {
        
        delay( 2500 );
        Serial.println( "Connecting to Wi-Fi" ); 
    }
    Serial.println( "Connected" );

}
