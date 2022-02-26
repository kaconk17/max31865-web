#include <Arduino.h>

#include <SPI.h>
#include <EthernetENC.h>
#include <Adafruit_MAX31865.h>

Adafruit_MAX31865 thermo = Adafruit_MAX31865(7, 8, 9, 6);

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

// Set the static IP address to use if the DHCP fails to assign
IPAddress ip(192, 168, 137, 177);
EthernetServer server(80);
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
#define RREF      430.0
// The 'nominal' 0-degrees-C resistance of the sensor
// 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL  100.0

// Variables to measure the speed
unsigned long beginMicros, endMicros;
unsigned long byteCount = 0;
bool printWebData = true;  // set to false for better speed measurement

void setup() {
 
  Serial.begin(19200);
  Serial.println("Adafruit MAX31865 PT100 Sensor Test!");
  thermo.begin(MAX31865_3WIRE);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  // start the Ethernet connection:
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
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());
  } else {
    server.begin();
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
  // give the Ethernet shield a second to initialize:
  delay(1000);
  // if you get a connection, report back via serial:
 
}

void loop() {
 
  EthernetClient client = server.available();
  if (client) {
  Serial.println("new client");
  boolean currentLineIsBlank = true;
  while (client.connected()) {
  if (client.available()) {
  char c = client.read();
  Serial.write(c);
  if (c == '\n' && currentLineIsBlank) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close"); 
  client.println("Refresh: 5"); 
  client.println();
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<h1>");
  client.println("Temperatur Monitoring..:");
  client.println("</h1>");
  client.println("<br />");
  client.println("<h2>");
  client.println("Hasil Pengukuran : ");
  client.print(thermo.temperature(RNOMINAL, RREF));
  client.print(" C");
  client.println("</h2>");
  
  client.println("</html>");
  break;
  }
  if (c == '\n') {
  currentLineIsBlank = true;
  } else if (c != '\r') {
  currentLineIsBlank = false;
  }
  }
  }
    delay(1);
    client.stop();
    Serial.println("client disconnected");
    }
}