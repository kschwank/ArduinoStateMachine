//
// Created by ksh on 21.01.20.
//

#ifndef STATEMACHINE_SYSTEM_MENU_H
#define STATEMACHINE_SYSTEM_MENU_H

#include "system_state_machine.h"

class MenuStateManager : public StateManager {
    std::string _inputBuffer;
public:
    MenuStateManager(Node *rootNode = nullptr): StateManager(rootNode) { }// (rootMenuItem ? rootMenuItem : new MenuItem(DEFAULT_ROOT_NODE_ID, "/")) { }

    bool handleCommand(std::string cmd) {
        Log.trace("Handling command '%s'\n", cmd.c_str());
        Edge *edge = findEdgeByName(cmd, getPossibleTransitions());

        if (edge) {
            Log.trace("found matching edge %s [%i->%i]\n", edge->getName().c_str(), edge->getFromNodeId(), edge->getToNodeId());
        } else {
            Log.trace("found no matching edge for command '%s'\n", cmd.c_str());
        }
        return edge? transition(edge) : false;
    }

    std::string getMenuString() {
        std::string menuString;
        auto transitions = getPossibleTransitions();
        for (auto edge : *transitions) {
            menuString += edge->getName() + ": " + edge->getDescription() + "\n";
        }

        return menuString;
    }

    void handleInput(char input_char) {
        if (input_char == 10) return; // skip CR
        Log.verbose("c: %c [%i]\n", input_char, input_char);


        if (input_char == 13) { // evaluate on LF
            Log.trace("evaluating input '%s'\n", _inputBuffer.c_str());
            if (_inputBuffer == "/") {
                restart(true);
            } else if (!(_inputBuffer.empty()) && !(this->handleCommand(_inputBuffer))) {
                Log.warning("Invalid command or command failed: %s\n", _inputBuffer.c_str());
            }
            _inputBuffer.clear();
            Serial.println(this->getMenuString().c_str());
            Serial.print((this->getActiveNode()->getName() + "/ >").c_str());
            return;
        } else {
            _inputBuffer += input_char;
            Log.verbose("serial_menu_cmd: '%s'\n", _inputBuffer.c_str());
        }
    }
};

#endif //STATEMACHINE_SYSTEM_MENU_H
