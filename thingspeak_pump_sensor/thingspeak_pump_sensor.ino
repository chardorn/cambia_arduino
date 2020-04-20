#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define DATA_FIELD 1                                  // Data field to post the signal strength to.
#include "DHT.h"
#include "EmonLib.h"

EnergyMonitor emon1;



const char* pass = "breezyshrub";
const char* server = "mqtt.thingspeak.com";
char mqttUserName[] = "";                // Use any name.
char mqttPass[] = "";                 // Change to your MQTT API key from Account > MyProfile.
long writeChannelID=1041147;
char writeAPIKey[]   = "S537TW8F21VHNXC3";


WiFiClient client;                                    // Initialize the Wi-Fi client library.
PubSubClient mqttClient( client );                    // Initialize the PuBSubClient library.
int changeFlag=0;                                     // Let the main loop know ther is new data to set.

// Handle messages from MQTT subscription.
int mqttSubscriptionCallback(char* topic, byte* payload, unsigned int length);  

// Generate a unique client ID and connect to MQTT broker.
void mqttConnect();  

// Connect to a given Wi-Fi SSID
void connectWifi();

// Measure the Wi-Fi signal strength.
void updateRSSIValue();

// Build a random client ID for MQTT connection.
void getID(char clientID[], int idLength);

double Irms = 0;
unsigned long start, finished, elapsed;

void setup() {

    Serial.begin( 9600 );
    Serial.println( "Start" );
    int status = WL_IDLE_STATUS; // Set temporary Wi-Fi status.

    emon1.current(A0, 111.1);
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);

    // scan for existing networks:
    Serial.println("Scanning available networks...");
       
    connectWifi();  // Connect to Wi-Fi network.
    mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.

}



void loop() {
  
    elapsed = 0;
    
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    
    if (!mqttClient.connected())
    {
       mqttConnect(); // Connect if MQTT client is not connected.
    }

    Irms = emon1.calcIrms(1480);  // Calculate Irms only
    Serial.print(Irms*230.0);           // Apparent power
    Serial.print(" ");
    Serial.println(Irms);             // Irms
    if(Irms > 3){
      digitalWrite(2, HIGH);
      Serial.println("HIGH");
      start = millis();

      while(1){
        if(Irms < 3){
          finished = millis();
          sendData();
        }
      }
    }
    
    else{
     digitalWrite(2, LOW);
    }

}

void sendData() {

    elapsed = finished - start;
    String dataString = "field1=" + String(Irms) + "&field2= " + String(elapsed);
    Serial.println(dataString);
 
    Serial.println(dataString);
    mqttClient.loop(); // Call the loop to maintain connection to the server.  
 
    String topicString ="channels/" + String(writeChannelID) + "/publish/"+String(writeAPIKey);
    mqttClient.publish( topicString.c_str(), dataString.c_str() );
    Serial.println( "to channel " + String(writeChannelID ) );
}

void mqttConnect()
{
    char clientID[ 9 ];
    
    // Loop until connected.
    while ( !mqttClient.connected() )
    {

        getID(clientID,8);
       
        // Connect to the MQTT broker.
        Serial.print( "Attempting MQTT connection..." );
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

void connectWifi()
{

      // scan for nearby networks:
    Serial.println("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    { 
      Serial.println("Couldn't get a wifi connection");
      while(true);
    } 

      // print the list of networks seen:
    Serial.print("number of available networks:");
    Serial.println(numSsid);
    
    while (WiFi.status() != WL_CONNECTED ) {
      // print the network number and name for each network found:
      for (int thisNet = 0; thisNet<numSsid; thisNet++) {
        if(WiFi.SSID(thisNet) == "NETGEAR98"){
          continue;
        }
        WiFi.begin(WiFi.SSID(thisNet), pass);
        Serial.print(thisNet);
        Serial.print(") ");
        Serial.print(WiFi.SSID(thisNet));
        Serial.print("\tSignal: ");
        Serial.println(WiFi.RSSI(thisNet));
        for(int j = 0; j < 5; j ++){
          Serial.print("Status: ");
          Serial.println(WiFi.status());
          delay( 5000 );
          if (WiFi.status() == 3){
            Serial.println( "Connected" );
            //connectedToLocal = False;
            return;
          }
        }
        
      }
    }
}
