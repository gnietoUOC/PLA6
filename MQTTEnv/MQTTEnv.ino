#include <Adafruit_MQTT_Client.h>

#include <WiFi101.h>

#define SERVER      "192.168.0.173"
#define PORT  1883           
#define USERNAME    "genaro"
#define PWD         "passw0rd"

//char ssid[16]; // SSID
//char pwd[16];  // Password
#define WSSID  "Genaro0712"
#define WPWD   "passw0rd"
int status = WL_IDLE_STATUS;     

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, SERVER, PORT, USERNAME, PWD);
Adafruit_MQTT_Publish pub = Adafruit_MQTT_Publish(&mqtt, "MKRTemp");
Adafruit_MQTT_Subscribe sub = Adafruit_MQTT_Subscribe(&mqtt, "UbuntuTemp");

void setup() {
  // Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial);

  connectWifi();
  dumpWiFi();

  mqtt.subscribe(&sub);
  
}

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

void connectWifi() {
  
  // Verificamos que la Ethernet está disponible.
  // En una MKR1000 nunca debería fallar.
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }

//  readSSID(ssid,pwd);

  // Nos conectamos al AP (Raspberry)
  while ( status != WL_CONNECTED) {
    Serial.print(".");
    status = WiFi.begin(WSSID,WPWD);

    // Espero un poco antess de reintentar la conexión
    delay(5000);
  }
  Serial.println("\nConectado.");
}

void loop() {

  MQTT_connect();

  Adafruit_MQTT_Subscribe *msg;
  if ((msg = mqtt.readSubscription(5000))) {
//    Serial.println((char *) msg->lastread);
    if (msg == &sub) {
      Serial.println((char *) sub.lastread);
    }
  }

  Serial.print(".");

  if (pub.publish("260966")) {
    Serial.println("Pub OK");
  } else {
    Serial.println("Pub Failed");
  }
  
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

// Leo el SSD la contraseña
/*
void readSSID(char *ssid, char *pwd) {
  readValue("SSID?",ssid);
  readValue("Password?",pwd);

  Serial.print("Conecting with SSID: ");
  Serial.println(ssid);
}
*/

// Leo un parámetro de la consola
// TODO: Validar que no llegan más caracteres de los esperados
void readValue(char *msg, char *data) {

  // Mostramos el mensaje
  Serial.println(msg);
  // Esperamos a que lleguen caracteres
  while(!Serial.available()); 
  
  // Leemos los caracteres
  int n = Serial.available();
  for(int i=0; i<n; i++) {
    data[i] = Serial.read();
  }
  
  // Terminamos la cadena. 
  // Sé que tendría que ser data[n] pero así evito el CR final.
  data[n-1] = '\0'; 

}
