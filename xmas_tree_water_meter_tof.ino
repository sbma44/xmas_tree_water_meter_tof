#include <Wire.h>
#include "Adafruit_VL6180X.h"

Adafruit_VL6180X vl = Adafruit_VL6180X();

#include <PubSubClient.h>

#include <ESP8266WiFi.h>

#include "credentials.h"
// define WLAN_SSID "MyNetwork"
// define WLAN_PASS "MyPassword"

#define MQTT_SERVER      "192.168.1.2"
#define MQTT_SERVERPORT  1883                   // use 8883 for SSL

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(D4, OUTPUT);
  digitalWrite(D4, HIGH);
  Serial.begin(115200);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  client.setServer(MQTT_SERVER, MQTT_SERVERPORT); //connecting to mqtt server
  client.setCallback(callback);

  connectmqtt();
}

unsigned long last = 0;
void loop() {
  // mqtt
  if (!client.connected())
  {
    reconnect();
  }
  
  if ((millis() < last) || (millis() - last > 10000)) {
    digitalWrite(D4, LOW);
    delay(20);
  
    uint8_t range = vl.readRange();
    uint8_t status = vl.readRangeStatus();
  
    if (status == VL6180X_ERROR_NONE) {
      Serial.print("Range: "); Serial.println(range);
    }
  
    // Some error occurred, print it out!
    
    if  ((status >= VL6180X_ERROR_SYSERR_1) && (status <= VL6180X_ERROR_SYSERR_5)) {
      Serial.println("System error");
    }
    else if (status == VL6180X_ERROR_ECEFAIL) {
      Serial.println("ECE failure");
    }
    else if (status == VL6180X_ERROR_NOCONVERGE) {
      Serial.println("No convergence");
    }
    else if (status == VL6180X_ERROR_RANGEIGNORE) {
      Serial.println("Ignoring range");
    }
    else if (status == VL6180X_ERROR_SNR) {
      Serial.println("Signal/Noise error");
    }
    else if (status == VL6180X_ERROR_RAWUFLOW) {
      Serial.println("Raw reading underflow");
    }
    else if (status == VL6180X_ERROR_RAWOFLOW) {
      Serial.println("Raw reading overflow");
    }
    else if (status == VL6180X_ERROR_RANGEUFLOW) {
      Serial.println("Range reading underflow");
    }
    else if (status == VL6180X_ERROR_RANGEOFLOW) {
      Serial.println("Range reading overflow");
    }
    
    digitalWrite(D4, HIGH);    
    client.publish("xmas/tree/water/raw", String(range).c_str());
    last = millis(); 
  }

  client.loop();
}


void callback(char* topic, byte* payload, unsigned int length) {   //callback includes topic and payload ( from which (topic) the payload is comming)
  //client.publish("outTopic", "LED turned OFF");
}

void reconnect() {
  while (!client.connected()) {
    //Serial.println("Attempting MQTT connection...");
    if (client.connect("ESP8266_xmastree")) {
      //Serial.println("connected");
      // Once connected, publish an announcement...
      //client.publish("outTopic", "Nodemcu connected to MQTT");
      // ... and resubscribe
      //client.subscribe("inTopic");

    } else {
      /*
      
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      */
      delay(5000);
    }
  }
}

void connectmqtt()
{
  client.connect("ESP8266_xmastree");  // ESP will connect to mqtt broker with clientID
  {
    //Serial.println("connected to MQTT");
    // Once connected, publish an announcement...

    // ... and resubscribe
    //client.subscribe("inTopic"); //topic=Demo
    //client.publish("outTopic",  "connected to MQTT");

    if (!client.connected())
    {
      reconnect();
    }
  }
}
