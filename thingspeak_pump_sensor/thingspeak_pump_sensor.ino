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
unsigned long beginning, finished, elapsed;

#include <Servo.h>

Servo myservo;  // create servo object to control a servo
// twelve servo objects can be created on most boards

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
int update_counter = 0;

void setup() {


    Serial.begin( 9600 );
    Serial.println( "Start" );
    int status = WL_IDLE_STATUS; // Set temporary Wi-Fi status.

    emon1.current(A0, 111.1);
    //pinMode(2, OUTPUT);
    //digitalWrite(2, LOW);

    myservo.attach(16);  // attaches the servo on GIO2 to the servo object

    // scan for existing networks:
    Serial.println("Scanning available networks...");
       
    connectWifi();  // Connect to Wi-Fi network.
    mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.
    
    Irms = emon1.calcIrms(1480);
    Serial.println(String(Irms));
    Irms = emon1.calcIrms(1480);
    Serial.println(String(Irms));

    beginning == 0;

    Irms = emon1.calcIrms(1480);  // Calculate Irms only
    String dataString = "field1=" + String(Irms);
    Serial.println(dataString);
    mqttClient.loop(); // Call the loop to maintain connection to the server.  
    String topicString ="channels/" + String(writeChannelID) + "/publish/"+String(writeAPIKey);
    mqttClient.publish( topicString.c_str(), dataString.c_str() );

}



void loop() {

    moveServo();

    delay(1000);
    
    update_counter += 1;
     
    //Every x minutes, send an update with the amperage to ensure sensor is still communicating
    //minutes times 60 seconds
    if(update_counter > 10 * 60){

      if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
      }
    
      if (!mqttClient.connected())
      {
       mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.
       mqttConnect(); // Connect if MQTT client is not connected.
      }

      Irms = emon1.calcIrms(1480);  // Calculate Irms only
      String dataString = "field1=" + String(Irms);
      Serial.println(dataString);
      mqttClient.loop(); // Call the loop to maintain connection to the server.  
      String topicString ="channels/" + String(writeChannelID) + "/publish/"+String(writeAPIKey);
      mqttClient.publish( topicString.c_str(), dataString.c_str() );
    
      update_counter = 0; //reset counter
      
    }

    Serial.print(String(update_counter) + ": ");
    
    
    Irms = emon1.calcIrms(1480);      // Calculate Irms only
    Serial.println(Irms);             // Irms
    
    if(Irms > 3){
      //digitalWrite(2, HIGH);
      //Serial.println("HIGH");

      //if timer hasn't start, start
      if(beginning == 0){
        beginning = millis();
        Serial.println("beginning: " + String(beginning));
        
        moveServo();
              
      }

    }

    else if(beginning > 0){ //and Irms < 3
      
      //start timer
      finished = millis();
      Serial.println("finished: " + String(finished));
      elapsed = finished - beginning;
      sendData(elapsed);
      beginning = 0;
      digitalWrite(2, LOW);
    }
    delay(200);
    }

void sendData(int elapsed) {

  
    if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
    }
  
    if (!mqttClient.connected())
    {
     mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.
     mqttConnect(); // Connect if MQTT client is not connected.
    }

    String dataString = "field1=" + String(Irms) + "&field2= " + String(elapsed);
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
static const char aserlphanum[] ="0123456789"
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz";                        // For random generation of the client ID.

    // Generate client ID.
    for (int i = 0; i < idLength ; i++) {
        clientID[ i ] = alphanum[ random( 51 ) ];
    }
    clientID[ idLength ] = '\0';
    
}

void moveServo(){

  Serial.println("MOVING SERVO");

  myservo.attach(1);  // attaches the servo on GIO2 to the servo object

  int pos;

  for (pos = 0; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(2);                       // waits 15ms for the servo to reach the position
  }
  for (pos = 90; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(2);                       // waits 15ms for the servo to reach the position
  }
}

void connectWifi()
{

    while (WiFi.status() != WL_CONNECTED ) {
      
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
            return;
          }
        }
      }
    }
}
