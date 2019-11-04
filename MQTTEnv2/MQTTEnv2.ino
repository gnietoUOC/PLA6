//#include <WiFi101.h>
//#include <PubSubClient.h>

#include "Homie.h"

// Update these with values suitable for your network.

#define SERVER      "192.168.0.173"
#define MQPORT  1883           
#define USERNAME    "genaro"
#define PWD         "passw0rd"
#define CLIENT      "mkr1000"

#define WSSID  "Genaro0712"
#define WPWD   "passw0rd"
int status = WL_IDLE_STATUS;     

long lastMsg = 0;
char msg[50];
int value = 0;

WiFiClient wifiClient;
PubSubClient *client;

Device* mkrenv;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
//    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
//    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }

}

void reconnect() {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client->connect(CLIENT,USERNAME,PWD)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client->publish("outTopic", "hello world");
      // ... and resubscribe
      client->subscribe("UbuntuTemp");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again  in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
//  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output
  Serial.begin(9600);
  connectWiFi();
  dumpWiFi();

  client = new PubSubClient(wifiClient);
  
  client->setServer(SERVER, MQPORT);
  client->setCallback(callback);

  reconnect();

  defineDevice();
  mkrenv->dump();
  
}

void loop() {

/*  

  if (!client->connected()) {
    reconnect();
  }
  client->loop();

  long now = millis();
  if (now - lastMsg > 20000) {
    lastMsg = now;
    ++value;
    snprintf (msg, 50, "hello world #%ld", value);
    Serial.print("Publish message: ");
    Serial.println(msg);
    client->publish("UbuntuTemp", msg);
  }
*/
}

// Vuelco información básica de la conexión
void dumpWiFi() {

  // Imprimo la SSID de la conexión
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // Imprimo la configuración de red
  IPAddress ip = WiFi.localIP();
  Serial.print("IP address:  ");
  Serial.println(ip);

  Serial.print("Subnet mask: ");
  Serial.println((IPAddress)WiFi.subnetMask());

  Serial.print("Gateway IP:  ");
  Serial.println((IPAddress)WiFi.gatewayIP());

}

void connectWiFi() {
  
  // Verificamos que la Ethernet está disponible.
  // En una MKR1000 nunca debería fallar.
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

  // Nos conectamos al AP (Raspberry)
  while ( status != WL_CONNECTED) {
    Serial.print(".");
    status = WiFi.begin(WSSID,WPWD);

    // Espero un poco antess de reintentar la conexión
    delay(5000);
  }
  Serial.println("\nConnected.");
}

void defineDevice() {

  Serial.println("-> defineDevice");

  mkrenv = new Device(client,(char *)"MKR1000");
  Node* node = new Node(mkrenv,(char *)"MKRENV");
  Temperature* t = new Temperature(node);
  Humidity* h = new Humidity(node);
  Pressure* p = new Pressure(node);

  Serial.println("<- defineDevice");
}
