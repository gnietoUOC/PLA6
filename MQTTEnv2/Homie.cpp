#include "Homie.h"
#include "freeMemory.h"

//Base::Base(PubSubClient *client, char* name) {
Base::Base(PubSubClient *client) {
  Serial.println("-> Base(PS,char)");
  this->setParent(NULL);
  Serial.println(" -> Parent");
  this->setClient(client);
  Serial.println(" -> Client");
  this->setName((char *)"Homie");
  Serial.println("<- Base");
}

Base::Base(Base *parent, char* name) {
//  Serial.println("-> Base(P,char)");
  this->setParent(parent);
  this->setClient(parent->getClient());
  this->setName(name);
//  Serial.println("<- Base");
}

Base::Base(Base *parent, char* name, char *units, bool settable) {
  this->setParent(parent);
  this->setClient(parent->getClient());
  this->setName(name);
}

char* Base::getName() {
  return name;
}

void Base::setName(char *name) {
  this->name = name;

  pub((char *)"$name",name);
}

void Base::pub(char *tag,char *value) {
  char data[64];
  char path[64];
  
  getPath(path);
  if (tag!=NULL) {
    sprintf(data,"%s/%s",path,tag);
    client->publish(data,value,MQRETAIN);
  } else {
    client->publish(path,value,MQRETAIN);
  }
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
  Serial.println("-> Base.getPath()");
  char data[64];
  
  if (getParent()) {
    getParent()->getPath(data);
    sprintf(path,"%s/%s",data,getName());
  } else {
    sprintf(path,"%s",getName());
  }
  Serial.println(path);
  Serial.println("<- Base.getPath()");
}

void Base::process(char *topic, char *payload) {
  
}

Homie::Homie(PubSubClient *client) : Base(client){
  n=0;
  devices = (Device**)calloc(MAX_DEVICES,sizeof(Device*));
  Serial.println("<- Homie.Homie");
//  client->setCallback(&Homie::callback);  
}

Device **Homie::getDevices() {

  return devices;
}

void Homie::addDevice(Device *device) {

  device->setParent(this);
  Serial.println(" -> Parent");
  Serial.println(n);
  devices[n++] = device;
}

//void Device::callback(char* topic, byte* payload, unsigned int length) {
void Homie::callback(char* topic, byte* payload, unsigned int length) {
  char data[64];
  char token[16];

  strncpy(data,(char *)payload,length);
  data[length]=0;

  char *ptr = strtok(data,"/");
  
  strncpy(token,data,ptr-data);

  
  Serial.print(topic);
  Serial.print(":");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void Homie::update() {
//  Serial.println("-> Homie.update");
  
  Device** dd = getDevices();
  for (int i=0;i<1;i++) {
    Device* d = dd[i];
    if (d!=NULL) {
      d->update();
    }
  }

//  Serial.println("<- Home.update");
}

void Homie::dump() {
  
  Device** dd = getDevices();
  for (int i=0;i<1;i++) {
    Device* d = dd[i];
    if (d!=NULL) {
      d->dump();
    }
  }
}

//Device::Device(PubSubClient *client, char *name) : Node(client,name){
Device::Device(Homie *parent, char *name) : Node(parent, name) {
  n=0;
  nodes = (Node**)calloc(MAX_NODES,sizeof(Node*));
  Serial.println("nodes"); 
  parent->addDevice(this);
  Serial.println("<- Device.Device ");  
};

Node** Device::getNodes() {

  return nodes;
}

void Device::addNode(Node *node) {

  node->setParent(this);
  nodes[n++] = node;
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

void Device::update() {
//  Serial.println("-> Device.update");
  
  Node** nn = getNodes();
  for (int i=0;i<1;i++) {
    Node* n = nn[i];
    if (n!=NULL) {
      n->update();
    }
  }

  Property **pp = getProperties();
  for (int j=0;j<1;j++) {
    Property* p = pp[j];
    if (p!=NULL) {
      p->update();
    }
  }
    
//  Serial.println("<- Device.update");
}

void Device::dump() {

  Serial.println("-> dumpDevice");

  Node** nn = getNodes();
  for (int i=0;i<1;i++) {
    Node* n = nn[i];
    if (n!=NULL) {
      n->dump();
    }
  }
  
  Serial.println("<- dumpDevice");
}

Node::Node(Homie *parent, char* name) : Base(parent, name) {
  
  n=0;
  properties = (Property**)calloc(MAX_PROPERTIES,sizeof(Property*));
 
}

//Node::Node(PubSubClient *client, char* name) : Base(client, name) {
Node::Node(Device *parent, char* name) : Base(parent, name) {
  
  n=0;
  properties = (Property**)calloc(MAX_PROPERTIES,sizeof(Property*));
  parent->addNode(this);
  
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
  pub((char *)"$properties",data);
  
}

Property** Node::getProperties() {

  return properties;
}

void Node::update() {
//  Serial.println("-> Node.update");

  Property **pp = getProperties();
  for (int j=0;j<ENV_PROPS;j++) {
//    Serial.println(j);
    Property* p = pp[j];
    if (p!=NULL) {
//      Serial.println("->p");
      p->update();
//      Serial.println("<-p");
    }
  }

//  Serial.println("<- Node.update");
  
}

void Node::dump() {

  char data[64];
  char path[64];

//  sprintf(data,"-> dumpDevice [%s]",getName());
//  Serial.println(data);

  getPath(path);
  
  sprintf(data,"%s [%s]",getName(),path);
  Serial.println(data);

  Property **pp = getProperties();
  for (int j=0;j<ENV_PROPS;j++) {
    Property* p = pp[j];
    if (p!=NULL) {
      p->dump();
    }
  }
}

Property::Property(Node *parent, char *name, char *units, bool settable) : Base (parent,name, units, settable) {
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

//  Serial.println("-> Property.setValue");

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
//  Serial.println("<- Property.setValue");
}

int Property::getIValue() {
  return ivalue;
}

void Property::setIValue(int ival) {
  char data[40];

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
}

/*
void Property::update() {
  
}
*/

void Property::dump() {

//  Serial.println("-> dumpProperty");

  char data[64];
  char path[64];

//  sprintf(data,"-> dumpProperty [%s]",getName());
//  Serial.println(data);
  getPath(path);
  
  sprintf(data,"%s (%s) [%s]",getName(),getUnits(),path);
  Serial.println(data);
  
}

Temperature::Temperature(Node *parent) : Property(parent,(char *)"Temperature",(char *)"ºC",false) {
}

void Temperature::update() {
//  char data[16];

//  value = ENV.readTemperature();
//  if (this->getValue()<>value) {
//    sprintf(data,"%4.2f",ENV.readTemperature());           // Recuperamos la temperatura 
//  setValue(data);
  setValue(ENV.readTemperature());
//  }
}

Humidity::Humidity(Node *parent) : Property(parent,(char *)"Humidity",(char *)"%",false) {
}

void Humidity::update() {
//  char data[8];

//  sprintf(data,"%4.2f",ENV.readHumidity());           // Recuperamos la humedad 
//  setValue(data);
  setValue(ENV.readHumidity());
}

Pressure::Pressure(Node *parent) : Property(parent,(char *)"Pressure",(char *)"kPa",false) {
}

void Pressure::update() {
//  char data[8];

//  sprintf(data,"%4.2f",ENV.readPressure());           // Recuperamos la humedad 
//  setValue(data);
  setValue(ENV.readPressure());
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

Memory::Memory(Device *parent) : Property(parent,(char *)"FreeMem",(char *)"KB",false) {
}

void Memory::update() {
//  char data[8];

//  sprintf(data,"%d",freeMemory);           // Recuperamos la memoria disponible
  setIValue(freeMemory()); 
  
}
