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

//#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
#ifdef DEBUG    //Macros are usually in all capital letters.
  #define DPRINT(...)    Serial.print(__VA_ARGS__)     //DPRINT is a macro, debug print
  #define DPRINTLN(...)  Serial.println(__VA_ARGS__)   //DPRINTLN is a macro, debug print with new line
#else
  #define DPRINT(...)     //now defines a blank line
  #define DPRINTLN(...)   //now defines a blank line
#endif

#include <Arduino.h>
#include <WiFi101.h>
#include <PubSubClient.h>
//#include "Adafruit_ZeroTimer.h"
#include <Arduino_MKRENV.h>

class Homie;
class Device;
class Node;
class Property;
class Attribute;

class Base {
  public:
//    Base (PubSubClient *client, char* name);
//    Base (PubSubClient *client);
    Base (PubSubClient *client, Base *parent, char* name);
//    Base (PubSubClient *client, Base *parent, char* name, char *units, bool settable);

    char* getName();
    void setName(char *name);
//    float *getValue();
//    void setValue(float value);
    Base *getParent();
    void setParent(Base *parent);
    void setClient(PubSubClient *client);
    PubSubClient *getClient();
//    virtual void update();
//    virtual void dump();
    virtual void update(){};
    virtual void dump(){};
//    Base **getChildren();
//    Base *getChildren(char *name);
//    void addChildren(Base *);
    Attribute *getAttributes();
    Attribute *getAttribute(char *name);
    void addAttribute(Attribute *a);

    void getPath(char *path);
    void process(char *topic,char *payload);
    void pub(char *tag, char *value);

  private:
    char* name;
 //   float* value;
    Base *parent;
    PubSubClient *client;
    Attribute **attributes;
};

class Node : public Base {

  public:
    Node(PubSubClient *client, Device *parent, char* name);
//    Node(PubSubClient *client, char* name);
//    Node(Homie *parent, char* name);
    Property** getProperties();
    int getNumProperties();
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
//    Device(char *name);
//    Device(Homie *parent, char *name);
    Device(PubSubClient *client, Device *parent, char *name);
//    Node** getNodes();
//    void addNode(Node *n);
    Node** getChildren();
    int getNumChildren();
    void addChild(Node *n);

    void update();
    void dump();
    

  private: 
    int n;
//    Node** nodes;
    Node** children;
  
};

class Homie : public Device {

  public:
    Homie(PubSubClient *client);
//    Device **getDevices();
//    void addDevice(Device *d);
    void update();
//    void dump();
    void reconnect();

  private:
//    int n;
//    Device **devices;  
    static void callback(char* topic, byte* payload, unsigned int length);
  
};

class Property : public Base {

  public:
    Property(PubSubClient *client, Node *parent, char *name, char *units, bool settable);
    float getValue();
    void setValue(float value);
    int getIValue();
    void setIValue(int value);
    char* getUnits();
    void setUnits(char *units);
    bool getSettable();
    void setSettable(bool settable);
    
    void dump();
      void update();

  private:
    float value;
    int ivalue;
    bool settable;
    char* units;
    char* type;  

};

class Attribute {

  private:
    char *name;
    char *value;
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

class Memory : public Property {

  public:
    Memory(PubSubClient *client, Node *parent);
    void update();
};

class LED : public Property {

  public:
    LED(Node *parent);
//    void update();
};



#endif
