#ifndef HOMIE_H
#define HOMIE_H

#define MAX_DEVICES     1
#define MAX_NODES       2
#define MAX_PROPERTIES  6
#define NA              -99.99
#define SERVER          "192.168.0.173"
#define MQPORT          1883           
#define MQRETAIN        false
#define USERNAME        "genaro"
#define PWD             "passw0rd"
#define WILLTOPIC       "MKR1000"
#define WILLMESSAGE     "ATPC"

#define CLIENT          "mkr1000"

//#define MKCLOCK           48000000
//#define MKPERIOD          1 // 1sg
#define MQPERIOD          1000 // 1sg

#define ENV_PROPS       3

#include <cstdlib> 

#include <Arduino.h>
#include <WiFi101.h>
#include <PubSubClient.h>
//#include "Adafruit_ZeroTimer.h"
#include <Arduino_MKRENV.h>

class Device;
class Node;
class Property;

class Base {
  public:
//    Base (PubSubClient *client, char* name);
    Base (PubSubClient *client);
    Base (Base *parent, char* name);
    Base (Base *parent, char* name, char *units, bool settable);

    char* getName();
    void setName(char *name);
//    float *getValue();
//    void setValue(float value);
    Base *getParent();
    void setParent(Base *parent);
    void setClient(PubSubClient *client);
    PubSubClient *getClient();
    virtual void update() {};
    virtual void dump() {};
//    Base **getChildren();
//    Base *getChildren(char *name);
//    void addChildren(Base *);

    void getPath(char *path);
    void process(char *topic,char *payload);
    void pub(char *tag, char *value);

  private:
    char* name;
 //   float* value;
    Base *parent;
    PubSubClient *client;

};

class Homie : public Base {

  public:
    Homie(PubSubClient *client);
    Device **getDevices();
    void addDevice(Device *d);
    void update();
    void dump();

  private:
    int n;
    Device **devices;  
    static void callback(char* topic, byte* payload, unsigned int length);
  
};

class Node : public Base {

  public:
    Node(Device *parent, char* name);
//    Node(PubSubClient *client, char* name);
    Node(Homie *parent, char* name);
    Property** getProperties();
    void addProperty(Property *p);
    
    void update();
    void dump();

  private: 
    int n;
    Property** properties;

  
};

class Device : public Node {

  public:
//    Device(PubSubClient *client, char *name);
    Device(Homie *parent, char *name);
    Node** getNodes();
    void addNode(Node *n);

    void update();
    void dump();
    

  private: 
    int n;
    Node** nodes;
  
};

class Property : public Base {

  public:
    Property(Node *parent, char *name, char *units, bool settable);
    float getValue();
    void setValue(float value);
    int getIValue();
    void setIValue(int value);
    char* getUnits();
    void setUnits(char *units);
    bool getSettable();
    void setSettable(bool settable);
    
    void dump();

  private:
    float value;
    int ivalue;
    bool settable;
    char* units;
    char* type;  

};

class Temperature : public Property {

  public:
    Temperature(Node *parent);
    void update();
};

class Humidity : public Property {
  public:
    Humidity(Node *parent);
    void update();
};

class Pressure : public Property {

  public:
    Pressure(Node *parent);
    void update();
};

class Memory : public Property {

  public:
    Memory(Device *parent);
    void update();
};

class LED : public Property {

  public:
    LED(Node *parent);
//    void update();
};



#endif
