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

bool printTransition(Edge *edge) {
    Serial.printf("transitioning from %i to %i\n", edge->getFromNodeId(), edge->getToNodeId());
    return true;
}

void setup() {
    Serial.begin(115200);
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);

    delay(10000);

    Serial.println("creating root node");
    Node *rootNode = new Node(0, "/", rootNodeEnter);

    stateManager = new StateManager(rootNode);

    stateManager->addNode(new Node(1, "01"));
    stateManager->addNode(new Node(2, "02"));
    stateManager->addNode(new Node(3, "03"));
    stateManager->addNode(new Node(4, "04"));

    stateManager->addEdge(new Edge(0, 1, printTransition));
    stateManager->addEdge(new Edge(0, 2, printTransition));
    stateManager->addEdge(new Edge(0, 3, printTransition));
    stateManager->addEdge(new Edge(0, 4, printTransition));

    stateManager->addEdge(new Edge(1, 0, printTransition));
    stateManager->addEdge(new Edge(2, 0, printTransition));
    stateManager->addEdge(new Edge(3, 0, printTransition));
    stateManager->addEdge(new Edge(4, 0, printTransition));

    auto transitions = stateManager->getPossibleTransitions();

    stateManager->transition((*transitions)[2]);

    transitions = stateManager->getPossibleTransitions();

    stateManager->transition((*transitions)[0]);
}

void loop() {
    Serial.println("Starting loop...");

    while (1);
}
