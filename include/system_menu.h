//
// Created by ksh on 21.01.20.
//

#ifndef STATEMACHINE_SYSTEM_MENU_H
#define STATEMACHINE_SYSTEM_MENU_H

#include "system_state_machine.h"

class MenuStateManager : public StateManager {
    std::string _inputBuffer;
private:
    static std::vector<std::string> *splitInput(std::string input, char delim) {
        Log.trace("splitting input: '%s' with delimiter '%c'\n", input.c_str(), delim);
        auto *result = new std::vector<std::string>(0);
        Log.verbose("created initial result vector %i\n", result);

        std::string substr;
        for (auto it = input.begin(); it != input.end(); it++) {
            if (*it == delim) {
                Log.trace("extracted substr: '%s'\n", substr.c_str());
                result->push_back(substr);
                substr.clear();
            } else {
                substr += *it;
            }
        }
        Log.trace("extracted substr: '%s'\n", substr.c_str());
        result->push_back(substr);

        return result;
    }
    static std::string fillString(const std::string& s, int length, char c) {
        std::string chars(length, c);
        return s + chars.substr(0, length - s.size());
    }
public:
    MenuStateManager(Node *rootNode = nullptr): StateManager(rootNode) { }// (rootMenuItem ? rootMenuItem : new MenuItem(DEFAULT_ROOT_NODE_ID, "/")) { }

    bool handleCommand(std::string cmd) {
        Log.trace("Handling command '%s'\n", cmd.c_str());
        std::vector<std::string> *cmdline = splitInput(_inputBuffer, ' ');
        Log.verbose("Command line consists of %i elements\n", cmdline->size());

        cmd = cmdline->front();
        Edge *edge = findEdgeByName(cmd, getPossibleTransitions());

        if (edge) {
            Log.trace("found matching edge %s [%i->%i]\n", edge->getName().c_str(), edge->getFromNodeId(), edge->getToNodeId());
        } else {
            Log.trace("found no matching edge for command '%s'\n", cmd.c_str());
        }
        bool result = transition(edge, cmdline);
        delete cmdline;
        return result;
    }

    std::string getMenuString() {
        std::string menuString = "** " + _active->getName() + " **\n";
        auto transitions = getPossibleTransitions();
        for (auto edge : *transitions) {
            menuString += fillString("*" + edge->getName() + ": ", 15, ' ') + edge->getDescription() + "\n";
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
            } else if (!(this->handleCommand(_inputBuffer))) {
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
