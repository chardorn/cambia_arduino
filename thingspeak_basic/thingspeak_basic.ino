#include <PubSubClient.h>
#include <ESP8266WiFi.h>


//Option to put in a specific wifi name (ssid) and password
const char* ssid = "";        //Example: "NETGEAR98"

//If only password is provided, it will be tried on any available network
const char* pass = "";        //Example: "password123"

const char* server = "mqtt.thingspeak.com";
char mqttUserName[] = "";     //Optional (if private)
char mqttPass[] = "";         //Optional        
long writeChannelID= 0;      //Example = 1041147
char writeAPIKey[]   = "";    //Change to your MQTT API key from channel page > API Keys > (what's listed underneath) Write API Key

WiFiClient client;                                    // Initialize the Wi-Fi client library.
PubSubClient mqttClient( client );                    // Initialize the PuBSubClient library.

int counter = 0;

// Generate a unique client ID and connect to MQTT broker.
void mqttConnect();  

// Connect to a given Wi-Fi SSID
void connectWifi();

//Send data to the provided MQTT channel
void  publishMQTT();

// Build a random client ID for MQTT connection.
void getID(char clientID[], int idLength);

void setup() {

    Serial.begin(9600);
    Serial.println( "Start" );
    int status = WL_IDLE_STATUS; // Set temporary Wi-Fi status.

    Serial.println("Scanning available networks...");
    connectWifi();  // Connect to Wi-Fi network.
    mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.

}



void loop() {
    
    if (WiFi.status() != WL_CONNECTED) {
        connectWifi();
    }
    
    if (!mqttClient.connected())
    {
       mqttConnect(); // Connect if MQTT client is not connected.
    }

    counter += 1;
    
    publishMQTT();
}

void publishMQTT() {

    String dataString = "field1=" + String(counter); 

    //Add a second field by adding    + "&field2= " + String(data)
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

    /*
     * The following code is if you want to scan for any available
     * networks and try connecting to them.
     */
     
    // Scan for nearby networks:
    Serial.println("** Scan Networks **");
    int numSsid = WiFi.scanNetworks();
    if (numSsid == -1)
    { 
      Serial.println("Couldn't get a wifi connection");
      while(true);
    } 

    // Print the list of networks seen:
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

     /*
     * The following code is if you want to connect to the
     * specific network specified in the wifi and pass fields. 
     */

    /*
    
    WiFi.begin(WiFi.SSID(thisNet), pass);
    Serial.print("Connecting to ");
    Serial.println(ssid); 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected"); 
    server.begin();
    Serial.println("Server started");
    Serial.print("Use this URL to connect: ");
    Serial.print("http://");
    Serial.print(WiFi.localIP());
    Serial.println("/");

    */

     

     
}
