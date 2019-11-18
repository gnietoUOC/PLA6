#ifndef HOMIE_H
#define HOMIE_H

#define MAX_DEVICES     1
#define MAX_NODES       2
#define MAX_PROPERTIES  6
#define MAX_ATTRIBUTES  4
#define NA              -1000
#define SERVER          "192.168.0.173"
#define MQPORT          1883           
#define MQRETAIN        true
#define MQUSERNAME      "genaro"
#define MQPWD           "passw0rd"
#define WILLTOPIC       "MKR1000"
#define WILLMESSAGE     "ATPC"

#define MQCLIENT          "mkr1000"
#define MQPERIOD          1000 // 1sg

#define PISSID  "Genaro0712"
#define PIPWD   "passw0rd"

#define ENV_PROPS       3

//#define DEBUG   // Esta línea me permite activar o desactivar las trazas del código
#ifdef DEBUG      
  #define DPRINT(...)    Serial.print(__VA_ARGS__)    //DPRINT es un Serial.print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)  //DPRINTLN es un Serial.prinln
#else
  #define DPRINT(...)                                 //DPRINT es una línea en blanco
  #define DPRINTLN(...)                               //DRINTLN es una línea en blanco
#endif

#include <Arduino.h>
#include <WiFi101.h>
#include <PubSubClient.h>
#include <Arduino_MKRENV.h>

class Homie;
class Device;
class Node;
class Property;
class Attribute;

class Attribute {

  public:
    Attribute(char *name, char *cvalue);
    Attribute(char *name, int ivalue);
    char *getName();
    char *getCValue();
    int getIValue();

  private:
    char *name;
    char *cvalue;
    int ivalue;
};

class Base {
  public:
    Base (PubSubClient *client, Base *parent, char* name);

    char* getName();
    void setName(char *name);
    Base *getParent();
    void setParent(Base *parent);
    void setClient(PubSubClient *client);
    PubSubClient *getClient();
    virtual void update(){};
    virtual void dump(){};

    Attribute **getAttributes();
    int getNumAttributes();
    Attribute *getAttribute(char *name);
    void addAttribute(Attribute *attrib);

    void getPath(char *path);
    void process(char *topic,char *payload);
    void pub(char *tag, char *value);

  private:
    char* name;
    Base *parent;
    PubSubClient *client;
    int n;
    Attribute **attributes;
};

class Node : public Base {

  public:
    Node(PubSubClient *client, Device *parent, char* name);
    Property** getProperties();
    int getNumProperties();
    void addProperty(Property *p);
    Property *getProperty(int i);
    Property *getProperty(char *name);
    virtual void process(char *topic, char* value);
    
    void update();
    void dump();
    void clear();

  private: 
    int n;
    Property** properties;

  
};

class Device : public Node {

  public:
    Device(PubSubClient *client, Device *parent, char *name);
    Node** getChildren();
    int getNumChildren();
    void addChild(Node *n);
    Node *getChild(int i);
    Node *getChild(char *name);
    void process(char *topic, char* value);

    void update();
    void dump();
    void clear();

  private: 
    int n;
    Node** children;
  
};

class Homie : public Device {

  public:
    void update();
    void dump();
    void reconnect();
    Homie(PubSubClient *client);
    void callback(char* topic, uint8_t* payload, unsigned int length);

};

class Property : public Base {

  public:
    Property(PubSubClient *client, Node *parent, char *name, char *units, char *type, bool settable);
    float getValue();
    void setValue(float value);
    int getIValue();
    void setIValue(int ivalue);
    bool getBValue();
    void setBValue(bool bvalue);
    virtual void set(char *value);
    
    void dump();
    void update();
    void clear();

  private:
    float value;
    int ivalue;
    int bvalue;

};

class Temperature : public Property {

  public:
    Temperature(PubSubClient *client, Node *parent);
    void update();  
};

class Humidity : public Property {
  public:
    Humidity(PubSubClient *client, Node *parent);
    void update();
};

class Pressure : public Property {

  public:
    Pressure(PubSubClient *client, Node *parent);
    void update();
};

class Illuminance : public Property {

  public:
    Illuminance(PubSubClient *client, Node *parent);
    void update();
};

class UVA : public Property {

  public:
    UVA(PubSubClient *client, Node *parent);
    void update();
};

class UVB : public Property {

  public:
    UVB(PubSubClient *client, Node *parent);
    void update();
};

class UVIndex : public Property {

  public:
    UVIndex(PubSubClient *client, Node *parent);
    void update();
};

class Memory : public Property {

  public:
    Memory(PubSubClient *client, Node *parent);
    void update();
};

class LED : public Property {

  public:
    LED(PubSubClient *client, Node *parent,int port);
    void update();
    void set(bool status);
    void set(char *value);

   private:
    int port;
   
};

#endif
