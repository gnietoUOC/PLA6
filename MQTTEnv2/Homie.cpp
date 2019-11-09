#include "Homie.h"
#include "freeMemory.h"

Base::Base(PubSubClient *client, Base *parent, char* name) {
//  Serial.println("-> Base(P,char)");
  this->setParent(parent);
  this->setClient(client);
  this->setName(name);
//  Serial.println("<- Base");

  n = 0;
  attributes = (Attribute**)calloc(MAX_ATTRIBUTES,sizeof(Attribute*));
}

char* Base::getName() {
  return name;
}

void Base::setName(char *name) {
  this->name = name;

  if (getClient()!=NULL) {
    pub((char *)"$name",name);
  }
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
  Serial.println("-> Base.setClient");
  this->client = client;
  Serial.println("<- Base.setClient");
}

PubSubClient *Base::getClient() {
  return client;
}

void Base::getPath(char *path) {
  DPRINTLN("-> Base.getPath");
  char data[96];
  
  if (getParent()) {
    getParent()->getPath(data);
    sprintf(path,"%s/%s",data,getName());
  } else {
    sprintf(path,"%s",getName());
  }
  DPRINTLN(path);
  DPRINTLN("<- Base.getPath");
}

void Base::process(char *topic, char *payload) {
  
}

//Homie::Homie(PubSubClient *client) : Device(client, NULL,(char *)"Homie") {
//  DPRINTLN("-> Homie.Homie");
//  reconnect();
//  client->setCallback(&Homie::callback);  
////  client->setCallback([this](char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
////  client->setCallback([this](char* topic, uint8_t* payload, unsigned int length) { this->callback(topic, payload, length); });
////  client->setCallback([this] (char* topic, byte* payload, unsigned int length) { this->callback(topic, payload, length); });
////  client->setCallback(std::bind(&Homie:callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
//
//  DPRINTLN("<- Homie.Homie");
//}

Homie::Homie() : Device(NULL, NULL,(char *)"Homie") {
  
}

void Homie::setClient(PubSubClient *client) {
  Base:setClient(client);
  reconnect();
  client->setCallback(&Homie::callback);
}

Homie* Homie::homie= 0;
//Homie *Homie::getInstance(PubSubClient *client) {
Homie *Homie::getInstance() {
  //static Homie homie;
  if (homie == 0) {
    homie = new Homie();
  }
  Serial.println("<-> Homie.getInstance");
  return homie;
}


//void Homie::callback(char* topic, byte* payload, unsigned int length) {
void Homie::callback(char* topic, uint8_t* payload, unsigned int length) {
  char data[64];
  char token[16];

  DPRINTLN("-> Homie.callback");

  strncpy(data,(char *)payload,length);
  data[length]=0;

  Serial.print(topic);
  Serial.print(":");
//  for (int i=0;i<length;i++) {
//    Serial.print((char)payload[i]);
//  }
//  Serial.println();
  Serial.println(data);

//  process(topic,data);
  
  DPRINTLN("-> Homie.callback");

}

void Homie::update() {

  DPRINTLN("-> Homie.update");
//  Serial.println(getNumChildren());
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
  
  // Reintentamos hasta conseguir conexión
  while (!getClient()->connected()) {
    Serial.print("Attempting MQTT connection...");
    // He modificado la conexión para definir un mensaje 'Last Will'
    if (getClient()->connect(CLIENT,USERNAME,PWD,WILLTOPIC,1,true,WILLMESSAGE,true)) {
      Serial.println("Connected");
//      getClient()->subscribe("#");
      // Sólo me subscribo al 'set' de las propiedades
      sprintf(data,"Homie/%s/+/+/set",getName());
      getClient()->subscribe(data);
//      getClient()->setCallback(&Homie::callback);
    } else {
      Serial.print("Failed, rc=");
      Serial.print(getClient()->state());
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

Device::Device(PubSubClient *client, Device *parent, char *name) : Node(client, parent, name) {
  Serial.println("-> Device.Device ");  
  n=0;
  children = (Node**)calloc(MAX_NODES,sizeof(Node*));

  Serial.println("<- Device.Device ");  
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
  Serial.println(topic);
  if (!strncmp(topic,"Homie/",6)) {
    topic += 6;
  }
  char *token = strchr(topic,'/');
  Serial.println(token);
  if (token!=NULL) {
    int n = token-topic;
    strncpy(name,topic,n);
    name[n]=0;
    Serial.println(name);
    node = getChild(name);
    if (node!=NULL) {
      Serial.println("Found!");
//      ((Device *)node)->process(token+1,value);
      node->process(token+1,value);
    } else {
      Serial.println("Not found");
    }
    
//  } else {
//     Serial.print("Property:");
//     Serial.print("Property:");
//    node = getChild(name);
//    if (node!=NULL) {
//      node2->process(token,value);
//    }
  }
  DPRINTLN("<- Device.process");
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

  DPRINTLN("-> Device.update");
  Serial.println(getNumChildren());
  
  Node** nn = getChildren();
  for (int i=0;i<getNumChildren();i++) {
    Node* n = nn[i];
    if (n!=NULL) {
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
  Serial.println("-> Node.update");
  Serial.println(getNumProperties());

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
  Serial.print(topic);
  Serial.print(":");
  Serial.println(value);

  char *token = strchr(topic,'/');
  Serial.println(token);
  if (token!=NULL) {
    int n = token-topic;
    strncpy(name,topic,n);
    name[n]=0;
    Serial.println(name);
    prop = getProperty(name);
    if (prop!=NULL) {
      Serial.println(name);
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

//char *Property::getValue() {
float Property::getValue() {
  char data[16];
    sprintf(data,"%ld",&value);           // Recuperamos la temperatura 
    Serial.println(data);
  return value;
}

void Property::setValue(float value) {
  char data[64];

  DPRINTLN("-> Property.setValue");

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
    Serial.println("pub>SetBvalue");
    this->bvalue = bvalue;
  } else {
    DPRINTLN("@");
  }
  DPRINTLN("<- Property.setBValue");
}

void Property::update() {
  DPRINTLN("-> Property.update");  
  DPRINTLN("<-> Property.update");
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

  DPRINTLN("-> Temperature.update");
  setValue(ENV.readTemperature());
  DPRINTLN("<- Temperature.update");
}

Humidity::Humidity(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"Humidity",(char *)"%",(char *)"Float",false) {
}

void Humidity::update() {

  DPRINTLN("-> Humidity.update");
  setValue(ENV.readHumidity());
  DPRINTLN("<- Humidity.update");
}

Pressure::Pressure(PubSubClient *client, Node *parent) : Property(client,parent,(char *)"Pressure",(char *)"kPa",(char *)"Float",false) {
}

void Pressure::update() {

  DPRINTLN("-> Pressure.update");
  setValue(ENV.readPressure());
  DPRINTLN("<- Pressure.update");
}

Memory::Memory(PubSubClient *client, Node *parent) : Property(client, parent,(char *)"FreeMem",(char *)"KB",(char *)"Integer",false) {
}

void Memory::update() {

  DPRINTLN("-> Memory.update");
//  setIValue(freeMemory()); 
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
  Serial.println("-> LED.set");
  if (!strcmp(value,"ON")) {
    set(true);
  } else {
    if (!strcmp(value,"OFF")) {
      set(false);
    }
  }
  Serial.println("<- LED.set");
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

//HPubSubClient::HPubSubClient(Client& client) : PubSubClient(client) {
//}
//
//void HPubSubClient::setHomie(Homie *homie) {
//
//  this->homie = homie;
//}
//
//Homie *HPubSubClient::getHomie() {
//   return homie;
//}
//
//void HPubSubClient::callback(char* topic, uint8_t* payload, unsigned int length) {
//  getHomie()->callback(topic,payload,length);
//}
