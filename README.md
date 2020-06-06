These programs were developed by myself, Charlotte Dorn, while working on IoT sensor technology and data transmission at Cambia Community in Louisa, VA.
For more information on the community, see www.cambiacommunity.weebly.com

These programs were used either on an Arduino Uno, a [NodeMCU](https://www.ebay.com/itm/NEW-NodeMcu-Lua-ESP8266-CH340G-ESP-12E-Wireless-WIFI-Internet-Development-Board/332853824344?ssPageName=STRK:MEBIDX:IT&_trksid=p2057872.m2749.l2649), or a [Heltec LoRa Wifi ESP32](https://www.amazon.com/gp/product/B07HJ49VN8/ref=ppx_yo_dt_b_asin_title_o00_s00?ie=UTF8&psc=1).

# Useful File Descriptions
## thingspeak_basic
*For use on NodeMCU or other ESP8266 Board*
This is just an example program for publishing data to a Thingspeak channel.
Libraries: PubSubClient, ESP8266Wifi
## thingspeak_pump_sensor
*For use on NodeMCU or other ESP8266 Board*
This code publishes data recieved from an AC current sensor to Thingspeak.com.
Here is the sensor I purchased from Amazon:
[https://www.amazon.com/dp/B07D8SXQ13?psc=1&ref=ppx_pop_dt_b_product_details](https://www.amazon.com/dp/B07D8SXQ13?psc=1&ref=ppx_pop_dt_b_product_details)

This and the rest of the MQTT files are configured to use Cambia's wifi network and server. Make sure you change pass, server, writeChannelID, and writeAPIKey.

## thingspeak_temperature
*For use on NodeMCU or other ESP8266 Board*
This publishes the temperature read from a [Waterproof DS18B20 Digital temperature sensor](https://www.ebay.com/itm/Waterproof-DS18B20-Digital-temperature-sensor/264349902251?ssPageName=STRK%3AMEBIDX%3AIT&_trksid=p2057872.m2749.l2649).
Libraries: OneWire, DallasTemperature, PubSubClient, ESP8266WiFi

## OLED_Lora_Sender and OLED_Lora_Reciever
These two programs come from the library Heltec Wifi LoRa 32 (V2).


**NEITHER OF THE FOLLOWING HAVE BEEN TESTED**

## Button_Lora_Sender
This file is intended to send the message "ding" when a button hooked up to GPIO Pin 3 is pushed.

## Digital_Lora_Reciever
This file is intended to control a digital ouput whenever a message "ding" is recieved.
