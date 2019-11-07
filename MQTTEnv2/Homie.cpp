#include "Homie.h"
#include "freeMemory.h"

//Base::Base(PubSubClient *client, char* name) {
//Base::Base(PubSubClient *client) {
//  Serial.println("-> Base(PS,char)");
//  this->setParent(NULL);
//  Serial.println(" -> Parent");
//  this->setClient(client);
//  Serial.println(" -> Client");
//  this->setName((char *)"Homie");
//  Serial.println("<- Base");
//}

Base::Base(PubSubClient *client, Base *parent, char* name) {
//  Serial.println("-> Base(P,char)");
  this->setParent(parent);
//  this->setClient(parent->getClient());
  this->setClient(client);
  this->setName(name);
//  Serial.println("<- Base");
}

//Base::Base(PubSubClient *client, Base *parent, char* name, char *units, bool settable) {
//  this->setParent(parent);
////  this->setClient(parent->getClient());
//  this->setClient(client);
//  this->setName(name);
//}

char* Base::getName() {
  return name;
}

void Base::setName(char *name) {
  this->name = name;

  pub((char *)"$name",name);
}

void Base::pub(char *tag,char *value) {
  char data[96];
  char path[96];

  DPRINTLN("-> Base.pub");
  if (getClient()->connected()) {
    getPath(path);
    if (tag!=NULL) {
//      sprintf(data,"%s/%s",path,tag);
      client->publish(data,value,MQRETAIN);
    } else {
//      sprintf(data,"%s/",path);
      client->publish(path,value,MQRETAIN);
    }
//    Serial.println(data);
  }   
  DPRINTLN("<- Base.pub");
}

void Base::setParent(Base *parent) {
  this->parent = parent;
}

Base *Base::getParent() {
  return parent;
}

void Base::setClient(PubSubClient *client) {
  this->client = client;
}

PubSubClient *Base::getClient() {
  return client;
}

void Base::getPath(char *path) {
  DPRINTLN("-> Base.getPath()");
  char data[96];
  
  if (getParent()) {
    getParent()->getPath(data);
    sprintf(path,"%s/%s",data,getName());
  } else {
    sprintf(path,"%s",getName());
  }
  DPRINTLN(path);
  DPRINTLN("<- Base.getPath()");
}

//void Base::update() {
//  Serial.println("-> Base.update()");
//  Serial.println("<- Base.update()");
//}

void Base::process(char *topic, char *payload) {
  
}

//Homie::Homie(PubSubClient *client) : Base(client){
Homie::Homie(PubSubClient *client) : Device(client, NULL,(char *)"Homie") {
//  n=0;
//  devices = (Device**)calloc(MAX_DEVICES,sizeof(Device*));
  Serial.println("<- Homie.Homie");
  client->setCallback(&Homie::callback);  
//  client->setCallback([this](char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
}

//Device **Homie::getDevices() {
//
//  return devices;
//}

//void Homie::addDevice(Device *device) {
//
//  device->setParent(this);
//  Serial.println(" -> Parent");
//  Serial.println(n);
//  devices[n++] = device;
//}

//void Device::callback(char* topic, byte* payload, unsigned int length) {
void Homie::callback(char* topic, byte* payload, unsigned int length) {
  char data[64];
  char token[16];

  DPRINTLN("-> Homie.callback");

  strncpy(data,(char *)payload,length);
  data[length]=0;

//  char *ptr = strtok(data,"/");
  
//  strncpy(token,data,ptr-data);

  
  Serial.print(topic);
  Serial.print(":");
//  for (int i=0;i<length;i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
  Serial.println(data);
  DPRINTLN("-> Homie.callback");

}

void Homie::update() {
  DPRINTLN("-> Homie.update");
  
//  Device** dd = getDevices();
  Device** dd = (Device **)getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Device* d = dd[i];
    if (d!=NULL) {
      d->update();
    }
  }

  DPRINTLN("<- Homie.update");
}

//void Homie::dump() {
//  
////  Device** dd = getDevices();
//  Node** dd = getChildren();
//  for (int i=0;i<1;i++) {
//    Device* d = dd[i];
//    if (d!=NULL) {
//      d->dump();
//    }
//  }
//}

//Device::Device(PubSubClient *client, char *name) : Node(client,name){
//Device::Device(char *name) : Node(NULL, name) {
//}

void Homie::reconnect() {
  // Reintentamos hasta conseguir conexión
  while (!getClient()->connected()) {
    Serial.print("Attempting MQTT connection...");
    // He modificado la conexión para definir un mensaje 'Last Will'
    if (getClient()->connect(CLIENT,USERNAME,PWD,WILLTOPIC,1,true,WILLMESSAGE,true)) {
      Serial.println("Connected");
      getClient()->subscribe("#");
//      getClient()->setCallback(&Homie::callback);
      getClient()->setCallback(this->callback);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(getClient()->state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
//Device::Device(Homie *parent, char *name) : Node(parent, name) {
Device::Device(PubSubClient *client, Device *parent, char *name) : Node(client, parent, name) {
  n=0;
//  nodes = (Node**)calloc(MAX_NODES,sizeof(Node*));
  children = (Node**)calloc(MAX_NODES,sizeof(Node*));
//  Serial.println("nodes"); 
//  parent->addDevice(this);
  parent->addChild(this);
  Serial.println("<- Device.Device ");  
};

//Node** Device::getNodes() {
Node** Device::getChildren() {

//  return nodes;
  return children;
}

int Device::getNumChildren() {
  return n;
}

//void Device::addNode(Node *node) {
void Device::addChild(Node *node) {

  node->setParent(this);
//  nodes[n++] = node;
  children[n++] = node;
}

/*
Adafruit_ZeroTimer timer = Adafruit_ZeroTimer(4);

void Device::init() {
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
  timer.setCallback(true, TC_CALLBACK_CC_CHANNEL0, this->update);
  timer.enable(true);
}
*/
//
void Device::update() {
  DPRINTLN("-> Device.update");
  
//  Node** nn = getNodes();
  Node** nn = getChildren();
//  Serial.println(getNumChildren());
  for (int i=0;i<getNumChildren();i++) {
    Node* n = nn[i];
    if (n!=NULL) {
//  Serial.println(i);
      n->update();
    }
  }

//  Property **pp = getProperties();
//  for (int j=0;j<1;j++) {
//    Property* p = pp[j];
//    if (p!=NULL) {
//      p->update();
//    }
//  }
//    
  DPRINTLN("<- Device.update");
}

void Device::dump() {

  Serial.println("-> Device.dump");

//  Node** nn = getNodes();
  Node** nn = getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Node* n = nn[i];
    if (n!=NULL) {
      n->dump();
    }
  }
  
  Serial.println("<- Device.dump");
}

//Node::Node(Homie *parent, char* name) : Base(parent, name) {
//  
//  n=0;
//  properties = (Property**)calloc(MAX_PROPERTIES,sizeof(Property*));
// 
//}

//Node::Node(PubSubClient *client, char* name) : Base(client, name) {
Node::Node(PubSubClient *client, Device *parent, char* name) : Base(client, (Base *)parent, name) {
  
  n=0;
  properties = (Property**)calloc(MAX_PROPERTIES,sizeof(Property*));
//  parent->addNode(this);
  parent->addChild(this);
  
}

void Node::addProperty(Property *property) {
  char data[64];

  property->setParent(this);
  properties[n++] = property;

  strcpy(data,properties[0]->getName());
  if (n>1) {
    for (int i=1;i<n;i++) {
      strcat(data,",");
      strcat(data,properties[i]->getName());
    }
  }
  Serial.println("..Property,Property");
  pub((char *)"$properties",data);
  
}

Property** Node::getProperties() {

  return properties;
}

int Node::getNumProperties() {
  return n;
}

void Node::update() {
  DPRINTLN("-> Node.update");

  Property **pp = getProperties();
//  for (int j=0;j<ENV_PROPS;j++) {
  DPRINTLN(getNumProperties());
  for (int j=0;j<getNumProperties();j++) {
//    Serial.println(j);
    Property* p = pp[j];
    if (p!=NULL) {
//      Serial.println(j);
#ifdef DEBUG
        p->dump();
#endif        
      p->update();
    }
  }

  DPRINTLN("<- Node.update");
  
}

void Node::dump() {

  char data[96];
  char path[96];

  getPath(path);
  
  sprintf(data,"%s [%s]",getName(),path);
  Serial.println(data);

  Property **pp = getProperties();
//  for (int j=0;j<ENV_PROPS;j++) {
//  Serial.print(n);
  for (int j=0;j<getNumProperties();j++) {
    Property* p = pp[j];
    if (p!=NULL) {
//      Serial.println(j);
      p->dump();
    }
  }
  Serial.println("<- Node.dump");
}

//Property::Property(PubSubClient *client, Node *parent, char *name, char *units, bool settable) : Base (client, parent,name, units, settable) {
Property::Property(PubSubClient *client, Node *parent, char *name, char *units, bool settable) : Base (client, parent,name) {
//  value = NAN;
  parent->addProperty(this);
//  setValue((char *)NA);
//  setValue(NA);
//  setIValue(NA);
  value = -1000;
  ivalue = -1000;
  this->setUnits(units);
  this->setSettable(settable);
}

char *Property::getUnits() {
  return units;
}

void Property::setUnits(char *units) {
  this->units = units;

  pub((char *)"$units",units);

}

bool Property::getSettable() {
  return settable;
}

void Property::setSettable(bool settable) {
  this->settable = settable;

  pub((char *)"$settable",(char *)(settable? "True":"False"));
}

//char *Property::getValue() {
float Property::getValue() {
  char data[16];
    sprintf(data,"%ld",&value);           // Recuperamos la temperatura 
    Serial.println(data);
  return value;
}

//void Property::setValue(char *val) {
void Property::setValue(float val) {
  char data[64];

  DPRINTLN("-> Property.setValue");

//  Serial.print(this->value);
//  Serial.print(" <-> ");
//  Serial.println(value);
  
//  if (strcmp(this->value,val)) {
//  if (abs(this->value-val)>0.01) {
  if (abs(this->value-val)>1) {
    sprintf(data,"%.2f <-> %.2f (%.4f)",this->value,val,this->value-val);            
    Serial.println(data);    
    sprintf(data,"%.2f",val);            
    pub(NULL,data);
    this->value = val;
  } else {
    Serial.print("*");
  }
  DPRINTLN("<- Property.setValue");
}

int Property::getIValue() {
  return ivalue;
}

void Property::setIValue(int ival) {
  char data[64];

  DPRINTLN("-> Property.setIValue");
  if (abs(this->ivalue-ival)>1) {
//    Serial.println("Changed");
    sprintf(data,"%d <-> %d",this->ivalue,ival);           
    Serial.println(data);    
    sprintf(data,"%d",ival);            
    pub(NULL,data);
    this->ivalue = ival;
  } else {
    Serial.println("@");
  }
  DPRINTLN("-> Property.setIValue");
}


void Property::update() {
  DPRINTLN("-> Property.update");  
  DPRINTLN("<- Property.update");  
}

void Property::dump() {

  DPRINTLN("-> Property.dump");

  char data[64];
  char path[64];

//  sprintf(data,"-> dumpProperty [%s]",getName());
//  Serial.println(data);
  getPath(path); 
  
  sprintf(data,"%s (%s) [%s]",getName(),getUnits(),path);
  Serial.println(data);
  
  DPRINTLN("<- Property.dump");
}

Temperature::Temperature(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"Temperature",(char *)"ºC",false) {
}

void Temperature::update() {
//  char data[16];

  DPRINTLN("-> Temperature.update");
//  value = ENV.readTemperature();
//  if (this->getValue()<>value) {
//    sprintf(data,"%4.2f",ENV.readTemperature());           // Recuperamos la temperatura 
//  setValue(data);
  setValue(ENV.readTemperature());
//  }
  DPRINTLN("<- Temperature.update");
}

Humidity::Humidity(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"Humidity",(char *)"%",false) {
}

void Humidity::update() {
//  char data[8];

  DPRINTLN("-> Humidity.update");
//  sprintf(data,"%4.2f",ENV.readHumidity());           // Recuperamos la humedad 
//  setValue(data);
  setValue(ENV.readHumidity());
  DPRINTLN("<- Humidity.update");
}

Pressure::Pressure(PubSubClient *client, Node *parent) : Property(client,parent,(char *)"Pressure",(char *)"kPa",false) {
}

void Pressure::update() {
//  char data[8];

  DPRINTLN("-> Pressure.update");
//  sprintf(data,"%4.2f",ENV.readPressure());           // Recuperamos la humedad 
//  setValue(data);
  setValue(ENV.readPressure());
  DPRINTLN("<- Pressure.update");
}

//void reconnect() {
//  // Intento conectarme
//  while (!client->connected()) {
//    Serial.println("Connecting...");
//    // Attempt to connect
//    if (client->connect(CLIENT,USERNAME,PWD)) {
//      Serial.println("connected");
//      // Once connected, publish an announcement...
//      client->publish("outTopic", "hello world");
//      // ... and resubscribe
//      client->subscribe("UbuntuTemp");
//    } else {
//      Serial.print("failed, rc=");
//      Serial.print(client->state());
//      Serial.println(" try again in 5 seconds");
//      // Wait 5 seconds before retrying
//      delay(5000);
//    }
//  }
//}

Memory::Memory(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"FreeMem",(char *)"KB",false) {
}

void Memory::update() {
//  char data[8];

//  sprintf(data,"%d",freeMemory);           // Recuperamos la memoria disponible
  setIValue(freeMemory()); 
  
}
