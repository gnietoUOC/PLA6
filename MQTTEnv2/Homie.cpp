#include "Homie.h"

vector<Node> Device::getNodes() {

  return nodes;
  
}

Device::addNode(Node n) {

  nodes.add(n);
  
}

vector<Property> Node::getProperties() {

  return properties;
  
}

Node::addProperty(Property p) {

  properties.add(p);
  
}
