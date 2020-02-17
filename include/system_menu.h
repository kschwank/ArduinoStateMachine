//
// Created by ksh on 21.01.20.
//

#ifndef STATEMACHINE_SYSTEM_STATE_MACHINE_H
#define STATEMACHINE_SYSTEM_STATE_MACHINE_H

#include <ArduinoLog.h>

#include "system_state_machine.h"

class MenuStateManager : public StateManager {
    std::string _inputBuffer;
    std::vector<Node*> _path;
private:
    std::vector<std::string> *splitInput(const std::string& input, char delim);
    std::string fillString(const std::string& s, int length, char c);
public:
    explicit MenuStateManager(Node *rootNode = nullptr, Logging *logger = &Log);

    bool handleCommand(std::string cmd);

    std::string getMenuString();

    static std::string getPathString(const std::vector<Node*>& path);

    void handleInput(char input_char);
};

#endif //STATEMACHINE_SYSTEM_STATE_MACHINE_H
