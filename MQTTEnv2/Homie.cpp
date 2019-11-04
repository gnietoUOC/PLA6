#include "Homie.h"

Base::Base(PubSubClient *client, char* name) {
//  Serial.println("-> Base(PS,char)");
  this->setParent(NULL);
  this->setClient(client);
  this->setName(name);
//  Serial.println("<- Base");
}

//Base::Base(char* name, char *units, bool settable) {
//  this->setName(name);
//  this->setUnits(units);
//  this->setSettable(settable);
////  this->parent = NULL;
//  this->setParent(NULL);
//}

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
  this->setUnits(units);
  this->setSettable(settable);
//  this->parent = NULL;
}

char* Base::getName() {
  return name;
}

void Base::setName(char *name) {
  this->name = name;

/*  
  char data[64];
  char path[64];
  getPath(path);
  sprintf(data,"%s/$name",path);
  Serial.print("PUB: ");
  Serial.print(data);
  Serial.print(":");
  Serial.println(name);
  client->publish(data,name);
*/

  pub((char *)"$name",name);
}

void Base::pub(char *tag,char *value) {
  char data[64];
  char path[64];
  getPath(path);
  sprintf(data,"%s/%s",path,tag);
  client->publish(data,value);
}

char *Base::getUnits() {
  return units;
}

void Base::setUnits(char *units) {
  this->units = units;

  pub((char *)"$units",units);

}

bool Base::getSettable() {
  return settable;
}

void Base::setSettable(bool settable) {
  this->settable = settable;
}

void Base::setParent(Base *parent) {
  this->parent = parent;
//  Serial.print(name);
//  Serial.print("->");
//  if (parent!=NULL) {
//    Serial.println(parent->name);
//  } else {
//    Serial.println("[]");
//  }
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
//  Serial.println("-> Base.getPath()");
  char data[64];
  
  if (getParent()) {
    getParent()->getPath(data);
    sprintf(path,"%s/%s",data,getName());
  } else {
    sprintf(path,"%s",getName());
  }
//  Serial.println(path);
//  Serial.println("<- Base.getPath()");
}

//Device::Device(char *name) : Base(name){
Device::Device(PubSubClient *client, char *name) : Base(client,name){
  n=0;
//  nodes = (Node**)calloc(MAX_NODES*sizeof(Node*));
  nodes = (Node**)calloc(MAX_NODES,sizeof(Node*));
};

Node** Device::getNodes() {

  return nodes;
}

void Device::addNode(Node *node) {

  node->setParent(this);
  nodes[n++] = node;
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


//Property::Property(char *name, char *units, bool settable) : Base (name, units, settable) {
//}

//Property::Property(PubSubClient *client, char *name, char *units, bool settable) : Base (client,name, units, settable) {
Property::Property(Base *parent, char *name, char *units, bool settable) : Base (parent,name, units, settable) {
  ((Node *)parent)->addProperty(this);
}

Property** Node::getProperties() {

  return properties;
  
}

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

//Node::Node(char* name) : Base(name) {
Node::Node(Base *parent, char* name) : Base(parent, name) {
  
  n=0;
//  properties = (Property**)malloc(MAX_PROPERTIES*sizeof(Property*));
  properties = (Property**)calloc(MAX_PROPERTIES,sizeof(Property*));
  ((Device *)parent)->addNode(this);
}

void Node::addProperty(Property *property) {

  property->setParent(this);
  properties[n++] = property;
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
  for (int j=0;j<3;j++) {
    Property* p = pp[j];
    if (p!=NULL) {
      p->dump();
    }
  }
}

//Temperature::Temperature(PubSubClient *client) : Property(client,(char *)"Temperature",(char *)"ºC",false) {
//Temperature::Temperature() : Property((char *)"Temperature",(char *)"ºC",false) {
Temperature::Temperature(Base *parent) : Property(parent,(char *)"Temperature",(char *)"ºC",false) {

//  client->publish("GG","NN");
  
}

//Humidity::Humidity() : Property((char *)"Humidity",(char *)"%",false) {
Humidity::Humidity(Base *parent) : Property(parent,(char *)"Humidity",(char *)"%",false) {
}

//Pressure::Pressure() : Property((char *)"Pressure",(char *)"mmHg",false) {
Pressure::Pressure(Base *parent) : Property(parent,(char *)"Pressure",(char *)"mmHg",false) {
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
