
#include <SPI.h>
#include <EthernetENC.h>
#include <Adafruit_MAX31865.h>
#include <PubSubClient.h>

Adafruit_MAX31865 thermo = Adafruit_MAX31865(7, 8, 9, 6);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

byte ip[] = {192, 168, 137, 177};
byte mydns[] = {192, 168, 137, 1};
byte gateway[] = {192, 168, 137, 1};
byte subnet[] = {255, 255, 255, 0};

EthernetClient net;

unsigned long lastMillis = 0;

#define MQTT_DeviceName "DEV01"
#define MQTT_topic_Message  "temp/DEV01"
#define MQTT_out_topic  "req/DEV01"
#define MQTT_Broker  "192.168.137.10"
#define MQTT_Port  1883

void callback(char* topic, byte* payload, unsigned int length);

PubSubClient client(MQTT_Broker,MQTT_Port,callback,net);

#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

void sendData(char* topic){
  client.publish(topic,String(thermo.temperature(RNOMINAL, RREF)).c_str());
}
void callback(char* topic, byte* payload, unsigned int length) {
  sendData(MQTT_out_topic);
}
void setup() {
 
  Serial.begin(19200);
  
  thermo.begin(MAX31865_3WIRE);

  //Ethernet.begin(mac, ip,mydns,gateway,subnet);
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // try to congifure using IP address instead of DHCP:
    Ethernet.begin(mac, ip, mydns);
  } else {
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  
  delay(1000);
while (!client.connect(MQTT_DeviceName))
{
    Serial.print(".");
    delay(1000);
}
Serial.println("\nconnected!");
client.subscribe(MQTT_DeviceName);

}

void loop() {
  client.loop();
 
if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    sendData(MQTT_topic_Message);
  }
}