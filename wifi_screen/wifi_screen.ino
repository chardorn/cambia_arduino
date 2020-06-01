/**************************************************************************
  This is a library for several Adafruit displays based on ST77* drivers.

  Works with the Adafruit 1.8" TFT Breakout w/SD card
    ----> http://www.adafruit.com/products/358
  The 1.8" TFT shield
    ----> https://www.adafruit.com/product/802
  The 1.44" TFT breakout
    ----> https://www.adafruit.com/product/2088
  The 1.14" TFT breakout
  ----> https://www.adafruit.com/product/4383
  The 1.3" TFT breakout
  ----> https://www.adafruit.com/product/4313
  The 1.54" TFT breakout
    ----> https://www.adafruit.com/product/3787
  The 2.0" TFT breakout
    ----> https://www.adafruit.com/product/4311
  as well as Adafruit raw 1.8" TFT display
    ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams.
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional).

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <SPI.h>
#include <ESP8266WiFi.h>


// For the breakout, you can use any (4 or) 5 pins
#define TFT_SCLK 2  // SainSmart: SCL
#define TFT_MOSI 0  // SainSmart: SDA
#define TFT_CS   4  // SainSmart: CS
#define TFT_DC   5  // SainSmart: RS/DC
#define TFT_RST  16  // SainSmart: RES

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


void setup(void) {
  Serial.begin(9600);
  
  Serial.print(F("Hello! ST77xx TFT Test"));

  // Use this initializer if using a 1.8" TFT screen:
  tft.initR(INITR_BLACKTAB);      // Init ST7735S chip, black tab

  tft.fillScreen(ST77XX_WHITE);

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  time = millis() - time;

  Serial.println(time, DEC);
  delay(500);

  delay(500);

  Serial.println("done");
  delay(1000);
  }

void loop() {

    int cursor_pos = 20;

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

        String to_display = (String(WiFi.SSID(thisNet)) + " " + "\tSignal: " + String(WiFi.RSSI(thisNet)));
        
        tft.setCursor(20, cursor_pos);
        tft.setTextColor(ST77XX_BLACK);
        tft.setTextSize(1);
        tft.println(to_display);
        
      }
    }
}
