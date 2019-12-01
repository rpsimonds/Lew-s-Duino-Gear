////////////////////////////////////
// duinoNodes.h
// duinoNode types & functions
// @Ver: 1.021
// 12-01-2019
////////////////////////////////////
// Author: Robin Simonds, Beagle Bay Inc
// License: GPL v.3
///////////////////////////////////

// Basic Defaults
// override by defining in main sketch
// before including this file
///////////////////////////////////

#ifndef NUM_NODE_GROUPS
#define NUM_NODE_GROUPS 1
#endif
#ifndef NUM_NODES
#define NUM_NODES 1
#endif
#ifndef CLOCKPIN
#define CLOCKPIN 2
#endif
#ifndef LATCHPIN
#define LATCHPIN 3
#endif
#ifndef DATAPIN
#define DATAPIN 4
#endif
typedef struct nodeState {
  byte pins; // each bit represents the state of a port on a board
  byte used; // number of ports in use; use ports sequentially
};
typedef struct nodeGroup {
  byte clockPin;
  byte latchPin;
  byte dataPin;
  byte numNodes;
  nodeState *nodes;
};
typedef struct nodeAddress {
  word node; // high byte is group id 0 - 255; low byte is node id 0 - 255
  byte pin;
};
typedef struct nodeList {
  int elements; // number of nodes in the list
  nodeAddress *nodes;
};
void nodesRefresh(){
  extern nodeGroup nodeGroups[];
  // Shift out current node data, one node group at a time
  for(int i = (NUM_NODE_GROUPS - 1); i>=0; i--) {
    // shift all bits out in MSB order (last node first):
    // Prepare to shift by turning off the output
    digitalWrite(nodeGroups[i].latchPin, LOW);
    // shift out the byte for each node in the group
    for(int j = (nodeGroups[i].numNodes - 1); j>=0; j--) {
      shiftOut(nodeGroups[i].dataPin, nodeGroups[i].clockPin, MSBFIRST, nodeGroups[i].nodes[j].pins);
    }
    // turn on the output to activate
    digitalWrite(nodeGroups[i].latchPin, HIGH);
  }
}
void nodeWrite(struct nodeAddress addr, byte state) {
  extern nodeGroup nodeGroups[];
  // write the state to the pin bit defined by addr
  bitWrite(nodeGroups[highByte(addr.node)].nodes[lowByte(addr.node)].pins, addr.pin, state);
  nodesRefresh();
}
void nodeSet(struct nodeAddress addr, byte state) {
  extern nodeGroup nodeGroups[];
  // set the pins element with a byte value
  nodeGroups[highByte(addr.node)].nodes[lowByte(addr.node)].pins = state;
}
byte nodeGet(struct nodeAddress addr){
  extern nodeGroup nodeGroups[];
  // get the pins element for a node
  return nodeGroups[highByte(addr.node)].nodes[lowByte(addr.node)].pins;
}
void nodesInit(){
  extern nodeGroup nodeGroups[];
  for(int g = 0; g < NUM_NODE_GROUPS; g++){
    pinMode(nodeGroups[g].latchPin, OUTPUT);
    pinMode(nodeGroups[g].clockPin, OUTPUT);
    pinMode(nodeGroups[g].dataPin, OUTPUT);
    for(int j = 0; j < nodeGroups[g].numNodes; j++){
      nodeSet({word(g,j), 0}, 0);
    }
  }
  nodesRefresh();
}
void nodesOn(nodeList z){
  for(int i = 0; i < z.elements; i++){
    nodeWrite(z.nodes[i], HIGH);
  }
}
void nodesOff(nodeList z){
  for(int i = 0; i < z.elements; i++){
    nodeWrite(z.nodes[i], LOW);
  }
}
void nodesReset(){
  extern nodeGroup nodeGroups[];
  for(int g = 0; g < NUM_NODE_GROUPS; g++){
    for(int j = 0; j < nodeGroups[g].numNodes; j++){
      nodeSet({word(g,j), 0}, 0);
    }
  }
}
