////////////////////////////////////
// duinoNodesDemo.ino
// @Version: 1.001
// 12-01-2019
////////////////////////////////////
// Author: Robin Simonds, Beagle Bay Inc
// License: GPL v.3
///////////////////////////////////
// OVERRIDES FOR THIS SKETCH; REMOVE IF ONLY ONE NODE
////////////////////////////////////
#define NUM_NODES 2
#include "duinoNodes.h"
///////////////////////////////////
// A nodeGroup structure encapsulates all data relevant to a single chain of duinoNodes
// By default a single nodeGroup is created, but you can have as many nodeGroups as you
// have pins to support them; each nodeGroup requires its own microcontroller pins
// Accordingly, an array of one or more nodeGroups is used to handle all duinoNodes
///////////////////////////////////

// A nodeState array must be defined for each nodeGroup
// nodeState ndata0[NUM_NODES] = {{0, 8}};  // single node
nodeState ndata0[2] = {{0, 8}, {0, 8}}; // group of two nodes
// Now define one or more nodeGroups
nodeGroup nodeGroups[NUM_NODE_GROUPS] = {{CLOCKPIN, LATCHPIN, DATAPIN, NUM_NODES, ndata0}};

// Address Lists
nodeAddress nds1[8] = {
  {word(0, 0), 0},
  {word(0, 0), 1},
  {word(0, 0), 2},
  {word(0, 0), 3},
  {word(0, 0), 4},
  {word(0, 0), 5},
  {word(0, 0), 6},
  {word(0, 0), 7}
};
nodeList board1 = {
  8, nds1
};
// for a second board, if it exists
nodeAddress nds2[8] = {
  {word(0, 1), 0},
  {word(0, 1), 1},
  {word(0, 1), 2},
  {word(0, 1), 3},
  {word(0, 1), 4},
  {word(0, 1), 5},
  {word(0, 1), 6},
  {word(0, 1), 7}
};
nodeList board2 = {
  8, nds2
};
void setup() {
  nodesInit();
  Serial.begin(115200);
  Serial.println("duinoNode Demonstration Sketch");
  Serial.println("Testing " + String(NUM_NODES) + " nodes." );
  nodesTest();
  Serial.println("\nRunning...");
}
void loop() {
  static byte counter = 0;
  static byte racepos = 0;
  static long lastCounter, lastRace;
  static bool dirup = true;
  unsigned long curMillis;
  curMillis = millis();
  // Create two independent millis() cycles to demonstrate different manipulation methods
  if(curMillis - lastCounter >= 250){
     // This example sets the pin byte instead of individual port bits
     // causing the ports to show a binary counter
    lastCounter = curMillis;
    counter++;
    nodeSet(nds1[0], counter);
    // call nodesRefresh() if you want the outputs to refresh now
    // otherwise you can wait for the next regular refresh
    // This way you can manipulate bits asynchronously 
    // relative to output refresh
    nodesRefresh();     
  }
  if(NUM_NODES == 2){
    // This example writes to individual port bits
    // Output refresh is automatic
    if(curMillis - lastRace >= 100){
      lastRace = curMillis;
      nodeWrite(nds2[racepos], 0);
      if(dirup){
        racepos++;
        if(racepos == 7){
          dirup = false;
        }
      } else {
        racepos--;
        if(racepos == 0){
          dirup = true;
        }
      }
      nodeWrite(nds2[racepos], 1);
    }
  }
}
void nodesTest(){
  extern nodeGroup nodeGroups[];
  nodeAddress nadr;
  Serial.println("\nTurn ports on one at a time, then off.");
  nodesOff(board1);
  if(NUM_NODES == 2){
    nodesOff(board2);
  }
  for(int i = 0; i < (8 * NUM_NODES); i++){
    nadr = {word(0, i / 8), i % 8};
    nodeWrite(nadr, 1);
    delay(1000);
  }
  for(int i = (8 * NUM_NODES) - 1; i >= 0; i--){
    nadr = {word(0, i / 8), i % 8};
    nodeWrite(nadr, 0);
    delay(1000);
  }
  Serial.println("\nTurn all ports on, then off, simultaneously.");
  Serial.println("If there are two boards, blink board #2");
  Serial.println("12 times before turning both");
  Serial.println("boards fully on then fully off.");
  nodesOn(board1);
  if(NUM_NODES == 2){
    for(int i = 0; i < 12; i++){
      nodesOn(board2);
      delay(500);
      nodesOff(board2);
      delay(500);
    }
    nodesOn(board2);
    delay(1000);
  } else {
    delay(1000);
  }
  nodesOff(board1);
  if(NUM_NODES == 2){
    nodesOff(board2);
  }
  delay(1000);
  Serial.println("\nAlternate Odd and Even ports.");
  for(int i = 0; i < 25; i++){
    // Odd ports on
    for(int g = 0; g < NUM_NODE_GROUPS; g++){
      for(int j = 0; j < nodeGroups[g].numNodes; j++){
        for(int h = 0; h < nodeGroups[g].nodes[j].used; h+=2){
          nodeWrite({word(g,j), h}, 1);
        }
      }
    }
    // Even ports off
    for(int g = 0; g < NUM_NODE_GROUPS; g++){
      for(int j = 0; j < nodeGroups[g].numNodes; j++){
        for(int h = 1; h < nodeGroups[g].nodes[j].used; h+=2){
          nodeWrite({word(g,j), h}, 0);
        }
      }
    }
    delay(100);
    // Even ports on
    for(int g = 0; g < NUM_NODE_GROUPS; g++){
      for(int j = 0; j < nodeGroups[g].numNodes; j++){
        for(int h = 1; h < nodeGroups[g].nodes[j].used; h+=2){
          nodeWrite({word(g,j), h}, 1);
        }
      }
    }
    // Odd ports off
    for(int g = 0; g < NUM_NODE_GROUPS; g++){
      for(int j = 0; j < nodeGroups[g].numNodes; j++){
        for(int h = 0; h < nodeGroups[g].nodes[j].used; h+=2){
          nodeWrite({word(g,j), h}, 0);
        }
      }
    }
    delay(100);
  }
  Serial.println("\nTest Complete.");
  nodesReset();
  nodesRefresh();
  delay(1000);
}
