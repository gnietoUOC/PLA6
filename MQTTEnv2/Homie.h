#ifndef HOMIE_H
#define HOMIE_H

#define MAX_DEVICES     1
#define MAX_NODES       2
#define MAX_PROPERTIES  6
#define MAX_ATTRIBUTES  4
#define NA              -1000
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

#define DEBUG   //If you comment this line, the DPRINT & DPRINTLN lines are defined as blank.
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
//    Base (PubSubClient *client, char* name);
//    Base (PubSubClient *client);
    Base (PubSubClient *client, Base *parent, char* name);
//    Base (PubSubClient *client, Base *parent, char* name, char *units, bool settable);

    char* getName();
    void setName(char *name);
    Base *getParent();
    void setParent(Base *parent);
    virtual void setClient(PubSubClient *client);
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
 //   float* value;
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

  private: 
    int n;
    Property** properties;

  
};

class Device : public Node {

  public:
    Device(PubSubClient *client, Device *parent, char *name);
    Device(Device *parent, char *name);
    Node** getChildren();
    int getNumChildren();
    void addChild(Node *n);
    Node *getChild(int i);
    Node *getChild(char *name);
    void process(char *topic, char* value);

    void update();
    void dump();
    

  private: 
    int n;
//    Node** nodes;
    Node** children;
  
};

class Homie : public Device {

  public:
    void update();
    void dump();
    void reconnect();
//    static Homie *getInstance(PubSubClient *client);
    static Homie *getInstance();
//    Homie(PubSubClient *client);
    void setClient(PubSubClient *client);

  private:
//    static void callback(char* topic, byte* payload, unsigned int length);
    static void callback(char* topic, uint8_t* payload, unsigned int length);
//    void callback(char* topic, uint8_t* payload, unsigned int length);
    Homie();
//    Homie(PubSubClient *client);
    static Homie *homie;
};

//Homie *Homie::homie = 0;

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

/*
class HPubSubClient : public PubSubClient {

  public: 
    HPubSubClient(Client& client);
    void setHomie(Homie *homie);
    void callback(char* topic, uint8_t* payload, unsigned int length);
    Homie *getHomie();

  private:
    Homie *homie;
  
};
*/
#endif
