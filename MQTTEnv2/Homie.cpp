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

  n = 0;
  attributes = (Attribute**)calloc(MAX_ATTRIBUTES,sizeof(Attribute*));
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
//    Serial.println(tag==NULL? "*NULL*":"");
//    Serial.println(value);
    if (tag!=NULL) {
      sprintf(data,"%s/%s",path,tag);
      client->publish(data,value,MQRETAIN);
      DPRINTLN(data);
    } else {
      client->publish(path,value,MQRETAIN);
      DPRINTLN(path);
    } 
  }   
  DPRINTLN("<- Base.pub");
}

void Base::setParent(Base *parent) {
  this->parent = parent;
}

Base *Base::getParent() {
  return parent;
}

Attribute **Base::getAttributes() {
  return attributes;
}

int Base::getNumAttributes() {
  return n;
}

//void addAttribute(char *name, char *value) {
//  attributes[n++] = new Attribute(name,value);
void Base::addAttribute(Attribute *attrib) {
  char data[16];

  DPRINTLN("-> Base.addAttribute");
  attributes[n++] = attrib;

  sprintf(data,"$%s",attrib->getName());
  pub(data,attrib->getCValue());
  DPRINTLN("<- Base.addAttribute");
}

Attribute *Base::getAttribute(char *name) {
  DPRINTLN("-> Base.getAttribute");
  Attribute *a = NULL;
  
  int n = getNumAttributes();
  DPRINTLN(n);
  Attribute **aa = getAttributes(); 
  for (int i=0;i<n && a==NULL;i++) {
    Attribute *ai = aa[i];
    DPRINTLN(i);
    if (!strcmp(ai->getName(),name)) {
      DPRINTLN("Found!");
      a = ai;
    }
  }
  DPRINTLN("<- Base.getAttribute");
  return a;
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
  DPRINTLN("-> Homie.Homie");
  Serial.println("-> Homie.Homie");
  reconnect();
//  client->setCallback(&Homie::callback);  
//  client->setCallback([this](char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
  Serial.println("<- Homie.Homie");
  DPRINTLN("<- Homie.Homie");
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
  Serial.println("-> Homie.update");
  Serial.println(getNumChildren());
//  Device** dd = getDevices();
  Device** dd = (Device **)getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Device* d = dd[i];
    if (d!=NULL) {
      d->update();
    }
  }

  Serial.println("<- Homie.update");
  DPRINTLN("<- Homie.update");
}

void Homie::dump() {
  
//  Device** dd = getDevices();
  Device** dd = (Device **)getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Device* d = dd[i];
    if (d!=NULL) {
      d->dump();
    }
  }
}

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
      getClient()->setCallback(&Homie::callback);
//      getClient()->setCallback(this->callback);
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
  Serial.println("-> Device.Device ");  
  n=0;
//  nodes = (Node**)calloc(MAX_NODES,sizeof(Node*));
  children = (Node**)calloc(MAX_NODES,sizeof(Node*));
//  Serial.println("nodes"); 
//  parent->addDevice(this);
//  if (parent!=NULL) {
//    parent->addChild(this);
//  } else {
//    Serial.println("NULL Parent");
//  }
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
  Serial.println("-> Device.addChild");

  node->setParent(this);
//  nodes[n++] = node;
  children[n++] = node;

  Serial.println("<- Device.addChild");
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
  Serial.println("-> Device.update");
  Serial.println(getNumChildren());
  
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
  Serial.println("<- Device.update");
  DPRINTLN("<- Device.update");
}

void Device::dump() {

  DPRINTLN("-> Device.dump");

//  Node** nn = getNodes();
  Node** nn = getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Node* n = nn[i];
    if (n!=NULL) {
      n->dump();
    }
  }
  
  DPRINTLN("<- Device.dump");
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
  if (parent!=NULL) {
    parent->addChild(this);
  } else {
    Serial.println("NULL Parent");
  }
  
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

int Node::getNumProperties() {
  return n;
}

void Node::update() {
  DPRINTLN("-> Node.update");
  Serial.println("-> Node.update");
  Serial.println(getNumProperties());

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

  Serial.println("<- Node.update");
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
  DPRINTLN("<- Node.dump");
}

//Property::Property(PubSubClient *client, Node *parent, char *name, char *units, bool settable) : Base (client, parent,name, units, settable) {
Property::Property(PubSubClient *client, Node *parent, char *name, char *units, char *type, bool settable) : Base (client, parent,name) {
//  value = NAN;
  parent->addProperty(this);
//  setValue((char *)NA);
//  setValue(NA);
//  setIValue(NA);
  value = NA;
  ivalue = NA;
  bvalue = false;
//  this->setUnits(units);
//  this->setSettable(settable);
  addAttribute(new Attribute((char *)"units",units));
  if (type!=NULL) {
    addAttribute(new Attribute((char *)"type",type));
  }
  addAttribute(new Attribute((char *)"settable",(char *)(settable? "True":"False")));
//  Attribute *a = new Attribute((char *)"units",units);
//  addAttribute(a);
//  a = new Attribute((char *)"settable",settable);
//  addAttribute(a);
  
}

//char *Property::getUnits() {
//  return units;
//}
//
//void Property::setUnits(char *units) {
//  this->units = units;
//
//  pub((char *)"$units",units);
//
//}
//
//bool Property::getSettable() {
//  return settable;
//}
//
//void Property::setSettable(bool settable) {
//  this->settable = settable;
//
//  pub((char *)"$settable",(char *)(settable? "True":"False"));
//}

//char *Property::getValue() {
float Property::getValue() {
  char data[16];
    sprintf(data,"%ld",&value);           // Recuperamos la temperatura 
    Serial.println(data);
  return value;
}

//void Property::setValue(char *val) {
void Property::setValue(float value) {
  char data[64];

  DPRINTLN("-> Property.setValue");

//  Serial.print(this->value);
//  Serial.print(" <-> ");
//  Serial.println(value);
  
//  if (strcmp(this->value,val)) {
//  if (abs(this->value-val)>0.01) {
  if (abs(this->value-value)>=1) {
    sprintf(data,"%.2f <-> %.2f (%.4f)",this->value,value,this->value-value);            
    Serial.println(data);    
    sprintf(data,"%.2f",value);            
    pub(NULL,data);
    this->value = value;
  } else {
    DPRINTLN("*");
  }
  DPRINTLN("<- Property.setValue");
}

int Property::getIValue() {
  return ivalue;
}

void Property::setIValue(int ivalue) {
  char data[64];

  DPRINTLN("-> Property.setIValue");
  if (abs(this->ivalue-ivalue)>=1) {
//    Serial.println("Changed");
    sprintf(data,"%d <-> %d",this->ivalue,ivalue);           
    Serial.println(data);    
    sprintf(data,"%d",ivalue);            
    pub(NULL,data);
    this->ivalue = ivalue;
  } else {
    DPRINTLN("@");
  }
  DPRINTLN("-> Property.setIValue");
}

bool Property::getBValue() {
  return bvalue;
}

void Property::setBValue(bool bvalue) {
  char data[64];

  DPRINTLN("-> Property.setBValue");
  if (this->bvalue!=bvalue) {
//    Serial.println("Changed");
    sprintf(data,"%d <-> %d",this->bvalue,bvalue);           
    Serial.println(data);    
    sprintf(data,"%d",bvalue);            
    pub(NULL,data);
    Serial.println("pub>SetBvalue");
    this->bvalue = bvalue;
  } else {
    DPRINTLN("@");
  }
  DPRINTLN("<- Property.setBValue");
}

void Property::update() {
  DPRINTLN("-> Property.update");  
  Serial.println("<-> Property.update");
  DPRINTLN("<- Property.update");  
}

void Property::dump() {

  DPRINTLN("-> Property.dump");

  char data[64];
  char path[64];

//  Attribute *a = getAttribute((char *)"type");
//  if (a!=NULL) {
//    sprintf(data,"type: %s",a->getCValue());
//    Serial.println(data);
//  } else {
//    Serial.println("Not found");
//  }

//  sprintf(data,"-> dumpProperty [%s]",getName());
//  Serial.println(data);
  getPath(path); 
  
 // sprintf(data,"%s (%s) [%s]",getName(),getUnits(),path);
  sprintf(data,"%s [%s]",getName(),path);
  Serial.println(data);
  
  DPRINTLN("<- Property.dump");
}

Temperature::Temperature(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"Temperature",(char *)"ºC",(char *)"Float",false) {
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

Humidity::Humidity(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"Humidity",(char *)"%",(char *)"Float",false) {
}

void Humidity::update() {
//  char data[8];

  DPRINTLN("-> Humidity.update");
//  sprintf(data,"%4.2f",ENV.readHumidity());           // Recuperamos la humedad 
//  setValue(data);
  setValue(ENV.readHumidity());
  DPRINTLN("<- Humidity.update");
}

Pressure::Pressure(PubSubClient *client, Node *parent) : Property(client,parent,(char *)"Pressure",(char *)"kPa",(char *)"Float",false) {
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

Memory::Memory(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"FreeMem",(char *)"KB",(char *)"Integer",false) {
}

void Memory::update() {
//  char data[8];

//  sprintf(data,"%d",freeMemory);           // Recuperamos la memoria disponible
  Serial.println("-> Memory.update");

  setIValue(freeMemory()); 
  
  Serial.println("<- Memory.update");
}

LED::LED(PubSubClient *client, Node *parent, int port) : Property(client, parent,(char *)"LED",NULL,(char *)"Boolean",true) {
//  addAttribute(new Attribute((char *)"port",port));
  this->port = port;
  setIValue(0);
  pinMode(port, OUTPUT);
}

void LED::set(bool status) {
  setBValue(status);
//  int i = status? 1:0;
//  setIValue(i);
  digitalWrite(port,status);
//  update();
}

void LED::update() {
//  Attribute *a = getAttribute((char *)"port");
//  setBValue(digitalRead(a->getIValue())); 
//  setBValue(digitalRead(port)); 
  Serial.println("<-> LED.update");
  
}

Attribute::Attribute(char *name, char *cvalue) {
   
  this->name = name;
  this->cvalue = cvalue;
}

Attribute::Attribute(char *name, int ivalue) {
   
  this->name = name;
  this->ivalue = ivalue;
}

char *Attribute::getName() {
  return name;
}

char *Attribute::getCValue() {
  return cvalue;
}

int Attribute::getIValue() {
  return ivalue;
}
