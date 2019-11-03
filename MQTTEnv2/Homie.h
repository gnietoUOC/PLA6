#ifndef HOMIE
#define HOMIE

#include <vector>

class Property {

  public:
    char name[16];

  private:
  
};

class Node {

  public:
    char name[16];

    vector<Property> getProperties();
    void addProperty(Property p);

  private: 
    vector<Property> properties;

  
};

class Device {

  public:
    char name[16];

    vector<Node> getNodes();
    void addNode(Node n);

  private: 
    vector<Node> nodes;
  
};



#endif
