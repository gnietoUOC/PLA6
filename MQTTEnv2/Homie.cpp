#include "Homie.h"
#include "freeMemory.h"

Base::Base(PubSubClient *client, Base *parent, char* name) {
  DPRINTLN("-> Base.Base");

  this->setParent(parent);
  this->setClient(client);
  this->setName(name);

  n = 0;
  attributes = (Attribute**)calloc(MAX_ATTRIBUTES,sizeof(Attribute*));
  
  DPRINTLN("<- Base.Base");
}

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
      sprintf(data,"%s/%s",path,tag);
      if (value!=NULL) {
        client->publish(data,value,true);
      } else {
        client->publish(data,NULL,0,true);
      }
      DPRINTLN(data);
      Serial.println(data);
    } else {
      client->publish(path,value,false);
      DPRINTLN(path);
      Serial.println(path);
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

void Base::addAttribute(Attribute *attrib) {
  char data[16];

  DPRINTLN("-> Base.addAttribute");

  attributes[n++] = attrib;
  sprintf(data,"$%s",attrib->getName());
  pub(data,attrib->getCValue());

  DPRINTLN("<- Base.addAttribute");
}

Attribute *Base::getAttribute(char *name) {
  Attribute *a = NULL;

  DPRINTLN("-> Base.getAttribute");

  int n = getNumAttributes();
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
  char data[96];

  DPRINTLN("-> Base.getPath");
  
  if (getParent()) {
    getParent()->getPath(data);
    sprintf(path,"%s/%s",data,getName());
  } else {
    sprintf(path,"%s",getName());
  }

  DPRINTLN("<- Base.getPath");
}

void Base::process(char *topic, char *payload) {
}

Homie::Homie(PubSubClient *client) : Device(client, NULL,(char *)"Homie") {
  DPRINTLN("-> Homie.Homie");

  reconnect();
  
  DPRINTLN("<- Homie.Homie");
}

// Gestiona la recepción de los mensajes que llegan a uno de los 
// topics relacionados con este dispositivo
void Homie::callback(char* topic, uint8_t* payload, unsigned int length) {
  char data[64];
  char token[16];

  DPRINTLN("-> Homie.callback");

  strncpy(data,(char *)payload,length);
  data[length]=0;

  Serial.print(topic);
  Serial.print(":");
  process(topic,data);
  
  DPRINTLN("-> Homie.callback");
}

// Actualiza cada uno de los dispositivos definidos.
void Homie::update() {

  DPRINTLN("-> Homie.update");

  Device** dd = (Device **)getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Device* d = dd[i];
    if (d!=NULL) {
      d->update();
    }
  }

  DPRINTLN("<- Homie.update");
}

// Vuelca cada uno de los dispositivos definidos.
void Homie::dump() {
  
  Device** dd = (Device **)getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Device* d = dd[i];
    if (d!=NULL) {
      d->dump();
    }
  }
}

// Gestiona la reconexión al MQTT
void Homie::reconnect() {
  char data[32];
  
  DPRINTLN("-> Homie.reconnect");

  // Reintentamos hasta conseguir conexión
  while (!getClient()->connected()) {
    Serial.print("Connecting MQTT...");
    // He modificado la conexión para definir un mensaje 'Last Will'
    if (getClient()->connect(MQCLIENT,MQUSERNAME,MQPWD,WILLTOPIC,1,false,WILLMESSAGE,true)) {
      Serial.println("Connected");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(getClient()->state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  DPRINTLN("<- Homie.reconnect");
}

Device::Device(PubSubClient *client, Device *parent, char *name) : Node(client, parent, name) {
  DPRINTLN("-> Device.Device ");  
  
  n=0;
  children = (Node**)calloc(MAX_NODES,sizeof(Node*));

  DPRINTLN("<- Device.Device ");  
};

// Devuelve todos los Nodos/Dispositivos definidos.
Node** Device::getChildren() {

  return children;
}

// Devuelve el número de Nodos/Dispositivos definidos.
int Device::getNumChildren() {

  return n;
}

void Device::addChild(Node *node) {
  DPRINTLN("-> Device.addChild");

  node->setParent(this);
  children[n++] = node;

  DPRINTLN("<- Device.addChild");
}

// Recupera un Nodo/Dispositivo por su índice
Node *Device::getChild(int i) {
  return (i>n? NULL:children[i]);
}

// Recupera un Nodo/Dispositivo por su nombre
Node *Device::getChild(char *name) {
  Node *node = NULL;

  DPRINTLN("-> Device.getChild");
  for (int i=0; i<getNumChildren() && node==NULL; i++) {
    char *namei = getChild(i)->getName();
    if (!strcmp(namei,name)) {
        node = getChild(i);
    }
  }
  DPRINTLN("<- Device.getChild");
  return node;
}

// Método que procesa el mensaje recibido en un topic
// y lo redirije a la entidad correspondiente
// Es recursivo. Primero se extrae el dispositivo y 
// luego el nodo.
void Device::process(char *topic, char* value) {
  char name[16];
  Node *node;
  
  DPRINTLN("-> Device.process");

  if (!strncmp(topic,"Homie/",6)) {
    topic += 6;
  }
  char *token = strchr(topic,'/');
  if (token!=NULL) {
    int n = token-topic;
    strncpy(name,topic,n);
    name[n]=0;
    node = getChild(name);
    if (node!=NULL) {
      node->process(token+1,value);
    } else {
      Serial.println("Not found");
    }
  }

  DPRINTLN("<- Device.process");
}

// Gestiona la actualización de todos los nodos 
// de un dispositivo.
void Device::update() {

  DPRINTLN("-> Device.update");
  
  Node** nn = getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Node* n = nn[i];
    if (n!=NULL) {
      n->update();
    }
  }

  DPRINTLN("<- Device.update");
}

// Vuelca cada uno de los nodos de un dispositivo.
void Device::dump() {

  DPRINTLN("-> Device.dump");
  Node** nn = getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Node* n = nn[i];
    if (n!=NULL) {
      n->dump();
    }
  }
  
  DPRINTLN("<- Device.dump");
}

// Limpia los mensajes retenidos de cada uno de los dispositivos.
void Device::clear() {
  
  Serial.println("-> Device.clear");
  Serial.print("Nodes: ");
  Serial.println(getNumChildren());
  Node** nn = getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Node* n = nn[i];
    if (n!=NULL) {
      n->clear();
    }
  }
  Serial.println("<- Device.clear");
}

Node::Node(PubSubClient *client, Device *parent, char* name) : Base(client, (Base *)parent, name) {
  
  n=0;
  properties = (Property**)calloc(MAX_PROPERTIES,sizeof(Property*));
  if (parent!=NULL) {
    parent->addChild(this);
  } else {
    DPRINTLN("NULL Parent");
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

// Devuelve todas las propiedades definidas.
Property** Node::getProperties() {

  return properties;
}

// Devuelve el número de propiedades definidas.
int Node::getNumProperties() {
  return n;
}

// Recupera una propiedad de un nodo por su índice
Property *Node::getProperty(int i) {
  return (i>n? NULL:properties[i]);
}

// Recupera una propiedad de un nodo por su nombre
Property *Node::getProperty(char *name) {
  Property *prop = NULL;
  
  DPRINTLN("-> Node.getProperty");
  for (int i=0; i<getNumProperties() && prop==NULL; i++) {
    char *namei = getProperty(i)->getName();
    if (!strcmp(namei,name)) {
        prop = getProperty(i);
    }
  }
  DPRINTLN("<- Node.getProperty");
  return prop;
}

// Gestiona la actualización de todas las propiedades 
// de un nodo.
void Node::update() {
  
  DPRINTLN("-> Node.update");

  Property **pp = getProperties();
  DPRINTLN(getNumProperties());
  for (int j=0;j<getNumProperties();j++) {
    Property* p = pp[j];
    if (p!=NULL) {
      p->update();
    }
  }

  DPRINTLN("<- Node.update");
}

// Método que procesa el mensaje recibido en un topic
// y lo redirije a la propiedad correspondiente
void Node::process(char *topic, char *value) {
  char name[16];
  Property *prop;

  DPRINTLN("-> Node.process");

  char *token = strchr(topic,'/');
  if (token!=NULL) {
    int n = token-topic;
    strncpy(name,topic,n);
    name[n]=0;
    prop = getProperty(name);
    if (prop!=NULL) {
      prop->set(value);
    }
  }

  DPRINTLN("<- Node.process");
}

// Vuelca cada uno de los nodos de un dispositivo.
void Node::dump() {

  char data[96];
  char path[96];

  DPRINTLN("-> Node.dump");
  getPath(path);
  
  sprintf(data,"%s [%s]",getName(),path);
  Serial.println(data);

  Property **pp = getProperties();
  for (int j=0;j<getNumProperties();j++) {
    Property* p = pp[j];
    if (p!=NULL) {
      p->dump();
    }
  }
  DPRINTLN("<- Node.dump");
}

// Limpia los mensajes retenidos de cada uno de las propiedades.
void Node::clear() {
  
  Serial.println("-> Node.clear");
  Property **pp = getProperties();
  Serial.print("Properties: ");
  Serial.println(getNumProperties());
  for (int j=0;j<getNumProperties();j++) {
    Property* p = pp[j];
    if (p!=NULL) {
      p->clear();
    }
  }
  Serial.println("<- Node.clear");
}

Property::Property(PubSubClient *client, Node *parent, char *name, char *units, char *type, bool settable) : Base (client, parent,name) {
  parent->addProperty(this);
  value = NA;
  ivalue = NA;
  bvalue = false;
  addAttribute(new Attribute((char *)"units",units));
  if (type!=NULL) {
    addAttribute(new Attribute((char *)"type",type));
  }
  addAttribute(new Attribute((char *)"settable",(char *)(settable? "True":"False")));
  
}

void Property::set(char *value) {
  Serial.println("Not implemented");
}

float Property::getValue() {
  char data[16];
    sprintf(data,"%ld",&value);           // Recuperamos la temperatura 
    Serial.println(data);
  return value;
}

void Property::setValue(float value) {
  char data[64];

  DPRINTLN("-> Property.setValue");

//  if (abs(this->value-value)>=1) {
    sprintf(data,"%.2f <-> %.2f (%.4f)",this->value,value,this->value-value);            
    Serial.println(data);    
    sprintf(data,"%.2f",value);            
    pub(NULL,data);
    this->value = value;
//  }

  DPRINTLN("<- Property.setValue");
}

int Property::getIValue() {
  return ivalue;
}

void Property::setIValue(int ivalue) {
  char data[64];

  DPRINTLN("-> Property.setIValue");
  if (abs(this->ivalue-ivalue)>=1) {
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
    sprintf(data,"%d <-> %d",this->bvalue,bvalue);           
    Serial.println(data);    
    sprintf(data,"%d",bvalue);            
    pub(NULL,data);
    this->bvalue = bvalue;
  }
  DPRINTLN("<- Property.setBValue");
}

void Property::update() {
  DPRINTLN("-> Property.update");  
  DPRINTLN("<-> Property.update");
  DPRINTLN("<- Property.update");  
}

void Property::dump() {

  char data[64];
  char path[64];

  DPRINTLN("-> Property.dump");

  getPath(path); 
  sprintf(data,"%s [%s]",getName(),path);
  Serial.println(data);
  
  DPRINTLN("<- Property.dump");
}

// Limpia los mensajes retenidos de cada uno de las propiedades.
void Property::clear() {
  char data[16];

  Serial.println("-> Property.clear");
  
  Attribute **aa = getAttributes();
  for (int j=0;j<getNumAttributes();j++) {
    Attribute* a = aa[j];
    if (a!=NULL) {
      sprintf(data,"$%s",a->getName());
      pub(data,NULL);
    }
  }
  Serial.println("<- Property.clear");
}

Temperature::Temperature(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"Temperature",(char *)"ºC",(char *)"Float",false) {
}

void Temperature::update() {

  DPRINTLN("-> Temperature.update");

  float value = ENV.readTemperature();
  Serial.print("Temperature: ");
  Serial.println(value);
  setValue(value);

  DPRINTLN("<- Temperature.update");
}

Humidity::Humidity(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"Humidity",(char *)"%",(char *)"Float",false) {
}

void Humidity::update() {

  DPRINTLN("-> Humidity.update");
  
  float value = ENV.readHumidity();
  Serial.print("Humidity: ");
  Serial.println(value);
  setValue(value);
  
  DPRINTLN("<- Humidity.update");
}

Pressure::Pressure(PubSubClient *client, Node *parent) : Property(client,parent,(char *)"Pressure",(char *)"kPa",(char *)"Float",false) {
}

void Pressure::update() {

  DPRINTLN("-> Pressure.update");
  
  float value = ENV.readPressure();
  Serial.print("Pressure: ");
  Serial.println(value);
  setValue(value);
  
  DPRINTLN("<- Pressure.update");
}

Illuminance::Illuminance(PubSubClient *client, Node *parent) : Property(client,parent,(char *)"Illuminance",(char *)"lx",(char *)"Float",false) {
}

void Illuminance::update() {

  DPRINTLN("-> Illuminance.update");
  
  float value = ENV.readIlluminance();
  Serial.print("Illuminance: ");
  Serial.println(value);
  setValue(value);
  
  DPRINTLN("<- Illuminance.update");
}

UVA::UVA(PubSubClient *client, Node *parent) : Property(client,parent,(char *)"UVA",(char *)"",(char *)"Float",false) {
}

void UVA::update() {

  DPRINTLN("-> UVA.update");
  
  float value = ENV.readUVA();
  Serial.print("UVA: ");
  Serial.println(value);
  setValue(value);
  
  DPRINTLN("<- UVA.update");
}

UVB::UVB(PubSubClient *client, Node *parent) : Property(client,parent,(char *)"UVB",(char *)"",(char *)"Float",false) {
}

void UVB::update() {

  DPRINTLN("-> UVB.update");
  
  float value = ENV.readUVB();
  Serial.print("UVB: ");
  Serial.println(value);
  setValue(value);
  
  DPRINTLN("<- UVB.update");
}

UVIndex::UVIndex(PubSubClient *client, Node *parent) : Property(client,parent,(char *)"UV Index",(char *)"",(char *)"Float",false) {
}

void UVIndex::update() {

  DPRINTLN("-> UVIndex.update");

  float value = ENV.readUVIndex();
  Serial.print("UV Index: ");
  Serial.println(value);
  setValue(value);
  
  DPRINTLN("<- UVIndex.update");
}

Memory::Memory(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"FreeMem",(char *)"KB",(char *)"Integer",false) {
}

void Memory::update() {

  DPRINTLN("-> Memory.update");
  
  int value = freeMemory();
  Serial.print("Memory: ");
  Serial.println(value);
  setValue(value);
  
  DPRINTLN("<- Memory.update");
}

LED::LED(PubSubClient *client, Node *parent, int port) : Property(client, parent,(char *)"LED",NULL,(char *)"Boolean",true) {
  this->port = port;
  setIValue(0);
  pinMode(port, OUTPUT);
}

void LED::set(bool status) {
  setBValue(status);
  digitalWrite(port,status);
}

void LED::set(char *value) {
  DPRINTLN("-> LED.set");
  
  if (!strcmp(value,"ON")) {
    set(true);
  } else {
    if (!strcmp(value,"OFF")) {
      set(false);
    }
  }
  
  DPRINTLN("<- LED.set");
}

void LED::update() {
  DPRINTLN("<-> LED.update");
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
