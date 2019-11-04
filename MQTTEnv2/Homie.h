#ifndef HOMIE_H
#define HOMIE_H

#define MAX_NODES       2
#define MAX_PROPERTIES  6

#define SERVER      "192.168.0.173"
#define MQPORT  1883           
#define USERNAME    "genaro"
#define PWD         "passw0rd"
#define CLIENT      "mkr1000"

//#include <vector>
#include <cstdlib> 
//#include <stdio.h>

#include <Arduino.h>
#include <WiFi101.h>
#include <PubSubClient.h>

class Base {
  public:
//    Base (PubSubClient *client, char* name);
//    Base (PubSubClient *client, char* name, char *units, bool settable);
    Base (PubSubClient *client, char* name);
    Base (Base *parent, char* name);
//    Base (char* name, char *units, bool settable);
//    Base (PubSubClient *client, char* name, char *units, bool settable);
    Base (Base *parent, char* name, char *units, bool settable);

    char* getName();
    void setName(char *name);
    char* getUnits();
    void setUnits(char *units);
    bool getSettable();
    void setSettable(bool settable);
    void setParent(Base *parent);
    Base *getParent();
    void getPath(char *path);
    void setClient(PubSubClient *client);
    PubSubClient *getClient();

  private:
    char* name;
    bool settable;
    char* units;
    char* type;  
    Base *parent;
    PubSubClient *client;

    void pub(char *tag, char *value);
};

class Property : public Base {

  public:
//    Property(char *name, char *units, bool settable);
//    Property(PubSubClient *client, char *name, char *units, bool settable);
    Property(Base *parent, char *name, char *units, bool settable);
    void dump();

};

class Node : public Base {

  public:

//    Node(PubSubClient *client, char *name);
//    Node(char *name);
    Node(Base *parent, char* name);
    Property** getProperties();
    void addProperty(Property *p);
    void dump();

  private: 
    int n;
    Property** properties;

  
};

class Device : public Base {

  public:
    Device(PubSubClient *client, char *name);
//    Device(char *name);
    Node** getNodes();
    void addNode(Node *n);
    void dump();
    void init();

  private: 
    int n;
    Node** nodes;
  
};

class Temperature : public Property {

  public:
//    Temperature(PubSubClient *client);
    Temperature(Base *parent);
//    Temperature();
};

class Humidity : public Property {
  public:
    Humidity(Base *parent);
//    Humidity();
};

class Pressure : public Property {

  public:
    Pressure(Base *parent);
//    Pressure();
};



#endif
