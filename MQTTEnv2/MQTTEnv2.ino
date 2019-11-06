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

//long lastMsg = 0;
//char msg[50]; 
//int value = 0;
unsigned long next=0;

WiFiClient wifiClient;
PubSubClient *client;
// Objeto que gestionar el temporizador. Utilizamos el temporizador 4
// porque tiene una resolución de 32 bits. Sin esa resolución y con esa 
// velocidad de reloj no podríamos medir más de 1sg. 
//Adafruit_ZeroTimer timer = Adafruit_ZeroTimer(4);

//Device* mkrenv;
Homie *homie;

//void callback(char* topic, byte* payload, unsigned int length) {
//  Serial.print("Message arrived [");
//  Serial.print(topic);
//  Serial.print("] ");
//
//  mkrenv->process(topic,(char *)payload);
//
//}

void reconnect() {
  // Reintentamos hasta conseguir conexión
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
//    if (client->connect(CLIENT,USERNAME,PWD)) {
//    if (client->connect(CLIENT,USERNAME,PWD,WILLTOPIC,WILLQOS,WILLRETAIN,WILLMESSAGE,WILLCLEAN)) {
    
    // He modificado la conexión para definir un mensaje 'Last Will'
    if (client->connect(CLIENT,USERNAME,PWD,WILLTOPIC,1,true,WILLMESSAGE,true)) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client->publish("outTopic", "hello world");
      // ... and resubscribe
      client->subscribe("#");
//      client->setCallback(Device::callback);
//        client->setCallback(std::function(&Device:callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));   
//        client->setCallback([this] (char* topic, byte* payload, unsigned int length) { mkrenv->callback(topic, payload, length); });
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
  while (!Serial);
  
  Serial.println("*** MKR1000 ***");
  if (!ENV.begin()) {
    Serial.println("Failed to initialize MKR ENV shield!");
//    sensors = false;
  }
  connectWiFi();
  dumpWiFi();

  client = new PubSubClient(wifiClient);
  
  client->setServer(SERVER, MQPORT);
//  client->setCallback(callback);
//  client->setCallback(Device::callback);

  reconnect();

  defineDevice();
  homie->dump();
  
}

void loop() {
//  char data[16];

  if (millis()>next) {
    next += MQPERIOD;
    homie->update();

//    sprintf(data,"Memory: %d",freeMemory());
//    Serial.println(data);
    
  }

  if (!client->connected()) {
    reconnect();
  }

  client->loop();
 
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

  homie = new Homie(client);
  Serial.println(" -> Homie");
  Device *d = new Device(homie,(char *)"MKR1000");
  Serial.println(" -> Device");
  Memory *m = new Memory(d);
  Node *node = new Node(d,(char *)"MKRENV");
  Serial.println(" -> Node");
  Temperature *t = new Temperature(node);
  Humidity *h = new Humidity(node);
  Pressure *p = new Pressure(node);

  Serial.println("<- defineDevice");
}

/*
void start() {
  // Ajustamos el valor del contador.
  uint32_t compare = MKPERIOD*MKCLOCK/1024;

  timer.enable(false);
  // Ajustamos el valor del prescaler a 1024. Perdemos resolución (no
  // podremos subir de unos 48KHz). En realidad, al poder utilizar 
  // un contador de 32 bits no nos hace falta un prescaler. Lo podríamos 
  // dejas en 1 (ajustando eso sí el valor del contador)
  timer.configure(TC_CLOCK_PRESCALER_DIV1024,   // prescaler
          TC_COUNTER_SIZE_32BIT,                // 32bis
          TC_WAVE_GENERATION_MATCH_PWM          // PWM mode (sea lo que sea)
          );
  timer.setCompare(0, compare);
  // Indicamos la funcion que se debe llamar cuando se produzca la interrupción
  timer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, update);
  timer.enable(true);

  Serial.println("Started");
}
*/
void update() {
  Serial.println("-> update");
  homie->update();
  Serial.println("<- update");
}