#include <Arduino.h>
#include <ArduinoLog.h>

#include "system_state_machine.h"

StateManager *stateManager;

void rootNodeEnter(Node *node) {
    Serial.printf("Entered node: %s\n", node->getName().c_str());
}

void restartEsp() {
    ESP.restart();
}

bool printTransition(edge_event_data eventData) {
    Serial.printf("transitioning from %i to %i\n", eventData.edge->getFromNodeId(), eventData.edge->getToNodeId());
    return true;
}

void setup() {
    Serial.begin(115200);
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);

    delay(10000);

    Serial.println("creating root node");
    stateManager = new StateManager();

    stateManager->addNode(new Node(1, "01"));
    stateManager->addNode(new Node(2, "02"));
    stateManager->addNode(new Node(3, "03"));
    stateManager->addNode(new Node(4, "04"));

    stateManager->addEdge(new Edge(DEFAULT_ROOT_NODE_ID, 1, printTransition));
    stateManager->addEdge(new Edge(DEFAULT_ROOT_NODE_ID, 2, printTransition));
    stateManager->addEdge(new Edge(DEFAULT_ROOT_NODE_ID, 3, printTransition));
    stateManager->addEdge(new Edge(DEFAULT_ROOT_NODE_ID, 4, printTransition));

    stateManager->addEdge(new Edge(1, DEFAULT_ROOT_NODE_ID, printTransition));
    stateManager->addEdge(new Edge(2, DEFAULT_ROOT_NODE_ID, printTransition));
    stateManager->addEdge(new Edge(3, DEFAULT_ROOT_NODE_ID, printTransition));
    stateManager->addEdge(new Edge(4, DEFAULT_ROOT_NODE_ID, printTransition));

    auto transitions = stateManager->getPossibleTransitions();

    stateManager->transition((*transitions)[2]);

    transitions = stateManager->getPossibleTransitions();

    stateManager->transition((*transitions)[0]);
}

void loop() {
    Serial.println("Starting loop...");

    while (1);
}
