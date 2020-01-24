#include <Arduino.h>
#include <ArduinoLog.h>

#include "system_menu.h"

MenuStateManager *stateManager;

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
    stateManager = new MenuStateManager();

    stateManager->addNode(new Node(1, "01"));
    stateManager->addNode(new Node(2, "02"));
    stateManager->addNode(new Node(3, "03"));
    stateManager->addNode(new Node(4, "04"));

    stateManager->addEdge(new Edge("1", "", DEFAULT_ROOT_NODE_ID, 1, printTransition));
    stateManager->addEdge(new Edge("2", "", DEFAULT_ROOT_NODE_ID, 2, printTransition));
    stateManager->addEdge(new Edge("3", "", DEFAULT_ROOT_NODE_ID, 3, printTransition));
    stateManager->addEdge(new Edge("4", "" ,DEFAULT_ROOT_NODE_ID, 4, printTransition));

    stateManager->addEdge(new Edge("0", "", 1, DEFAULT_ROOT_NODE_ID, printTransition));
    stateManager->addEdge(new Edge("0", "", 2, DEFAULT_ROOT_NODE_ID, printTransition));
    stateManager->addEdge(new Edge("0", "", 3, DEFAULT_ROOT_NODE_ID, printTransition));
    stateManager->addEdge(new Edge("0", "", 4, DEFAULT_ROOT_NODE_ID, printTransition));

    stateManager->dump();

    auto transitions = stateManager->getPossibleTransitions();

    stateManager->transition((*transitions)[2]);

    transitions = stateManager->getPossibleTransitions();

    stateManager->transition((*transitions)[0]);
}

std::string serial_menu_cmd = std::string();

void loop() {

    while (Serial.available()) {
        char c = (char)Serial.read();
        if (c == 10) continue; // skip CR
        Log.trace("c: %c [%i]\n", c, c);

        if (c == 13) {
            Log.trace("evaluating input '%s'\n", serial_menu_cmd.c_str());
            if (!(stateManager->handleCommand(serial_menu_cmd))) {
                Log.warning("Invalid command or command failed: %s\n", serial_menu_cmd.c_str());
            }
            serial_menu_cmd.clear();
            Serial.println(stateManager->getMenuString().c_str());
            break;
        } else {
            serial_menu_cmd += c;
            Log.trace("serial_menu_cmd: '%s'\n", serial_menu_cmd.c_str());
        }
    }
}
