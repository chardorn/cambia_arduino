#include <ESP8266MQTTClient.h>
#include <ESP8266WiFi.h>
MQTTClient mqtt;

void setup() {
  Serial.begin(9600);
  WiFi.begin("pink pants cricket", "breezyshrub");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //topic, data, data is continuing
  mqtt.onData([](String topic, String data, bool cont) {
    Serial.printf("Data received, topic: %s, data: %s\r\n", topic.c_str(), data.c_str());
    mqtt.unSubscribe("/qos0");
  });

  mqtt.onSubscribe([](int sub_id) {
    Serial.printf("Subscribe topic id: %d ok\r\n", sub_id);
    mqtt.publish("test", "here we are!", 0, 0);
  });
  
  mqtt.onConnect([]() {
    Serial.printf("MQTT: Connected\r\n");
    Serial.printf("Subscribe id: %d\r\n", mqtt.subscribe("/qos0", 0));
    mqtt.subscribe("test", 1);
    //mqtt.subscribe("/qos2", 2);
  });

  mqtt.begin("192.168.1.191");
//  mqtt.begin("mqtt://test.mosquitto.org:1883", {.lwtTopic = "hello", .lwtMsg = "offline", .lwtQos = 0, .lwtRetain = 0});
//  mqtt.begin("mqtt://user:pass@mosquito.org:1883");
//  mqtt.begin("mqtt://user:pass@mosquito.org:1883#clientId");

}

void loop() {
  mqtt.handle();
}