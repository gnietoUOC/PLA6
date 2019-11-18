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
      client->publish(data,value,MQRETAIN);
      DPRINTLN(data);
      Serial.println(data);
    } else {
      client->publish(path,value,MQRETAIN);
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

void Homie::dump() {
  
  Device** dd = (Device **)getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Device* d = dd[i];
    if (d!=NULL) {
      d->dump();
    }
  }
}

void Homie::reconnect() {
  char data[32];
  
  DPRINTLN("-> Homie.reconnect");

  // Reintentamos hasta conseguir conexión
  while (!getClient()->connected()) {
    Serial.print("Connecting MQTT...");
    // He modificado la conexión para definir un mensaje 'Last Will'
    if (getClient()->connect(MQCLIENT,MQUSERNAME,MQPWD,WILLTOPIC,1,false,WILLMESSAGE,true)) {
      Serial.println("Connected");

      // Sólo me subscribo al 'set' de las propiedades
      sprintf(data,"Homie/%s/+/+/Set",getName());
      getClient()->subscribe(data);
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

Node** Device::getChildren() {

  return children;
}

int Device::getNumChildren() {

  return n;
}

void Device::addChild(Node *node) {
  DPRINTLN("-> Device.addChild");

  node->setParent(this);
  children[n++] = node;

  DPRINTLN("<- Device.addChild");
}

Node *Device::getChild(int i) {
  return (i>n? NULL:children[i]);
}

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

Property** Node::getProperties() {

  return properties;
}

int Node::getNumProperties() {
  return n;
}

Property *Node::getProperty(int i) {
  return (i>n? NULL:properties[i]);
}

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
