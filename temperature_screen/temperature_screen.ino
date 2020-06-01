#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#define DATA_FIELD 1                                  // Data field to post the signal strength to.
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>

// For the breakout, you can use any (4 or) 5 pins
#define TFT_SCLK 14  // SainSmart: SCL
#define TFT_MOSI 12  // SainSmart: SDA
#define TFT_CS   3  // SainSmart: CS
#define TFT_DC   13  // SainSmart: RS/DC
#define TFT_RST  15  // SainSmart: RES

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);

OneWire  ds(2);  // on pin 2 (a 4.7K pullup is necessary)
//NOTE: On the NODEMCU, this will be Pin D4

//The number of entries will be the first address

const char* pass = "breezyshrub";
const char* server = "mqtt.thingspeak.com";
char mqttUserName[] = "";                // Use any name.
char mqttPass[] = "";                 // Change to your MQTT API key from Account > MyProfile.
long writeChannelID = 1044816;
char writeAPIKey[]   = "GPO15HJ33PT7PH2D";

int msg_interval = 1 * 60; //minutes converted to seconds 

int counter = 0;

const long utcOffsetInSeconds = 3600;

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

WiFiClient client;                                    // Initialize the Wi-Fi client library.
PubSubClient mqttClient(client);                    // Initialize the PuBSubClient library.

// Generate a unique client ID and connect to MQTT broker.
void mqttConnect();  

// Connect to a given Wi-Fi SSID
void connectWifi();

// Build a random client ID for MQTT connection.
void getID(char clientID[], int idLength);

void setup() {
    
    Serial.begin( 9600 );
    Serial.println( "Start" );
    int status = WL_IDLE_STATUS; // Set temporary Wi-Fi status.
    
    EEPROM.begin(512);
    
    // scan for existing networks:
    Serial.println("Scanning available networks...");
       
    connectWifi();  // Connect to Wi-Fi network.
    mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.

      // Use this initializer if using a 1.8" TFT screen:
    tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

    tft.fillScreen(ST77XX_WHITE);

    collectData();
    
}


void loop() {

  counter +=1;

    if(counter > msg_interval){
      collectData();
      counter = 0;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
      connectWifi();
    }
  
    if (!mqttClient.connected())
    {
     mqttClient.setServer( server, 1883 ); // Set the MQTT broker details.
     mqttConnect(); // Connect if MQTT client is not connected.
    }


    //Check EPROM
    //Publish data to EPROM

    delay(1000);
}

void collectData(){

    //get position of EPROM saving 
    
    sensors.requestTemperatures();
    
    //Collect temperature sensor information and RSSI connection value
    float tempC = sensors.getTempCByIndex(0);
    float tempF = sensors.getTempFByIndex(0);
    float rssi = float(WiFi.RSSI());

    tft.setRotation(0);
    tft.setTextWrap(false);

    tft.fillScreen(ST77XX_WHITE);
    tft.setCursor(20, 40);
    tft.setTextColor(ST77XX_BLACK);
    tft.setTextSize(2.5);
    tft.println("C: " + String(tempC));
    tft.setCursor(20, 100);
    tft.setTextColor(ST77XX_BLACK);
    tft.setTextSize(2.5);
    tft.println("F: " + String(tempF));
  
    String dataString = "field1=" + String(tempC) + "&field2=" + String(tempF) + "&field3=" + String(rssi);
    Serial.println(dataString);
 
    Serial.println(dataString);
    mqttClient.loop(); // Call the loop to maintain connection to the server.
 
    String topicString ="channels/" + String(writeChannelID) + "/publish/" + String(writeAPIKey);
    mqttClient.publish( topicString.c_str(), dataString.c_str());
    Serial.println( "to channel " + String(writeChannelID ) );
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
        if((String(WiFi.SSID(thisNet)).indexOf("ESP")) >= 0){
          Serial.print("ESP!");
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
