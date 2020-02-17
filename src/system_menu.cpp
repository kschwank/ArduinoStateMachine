#include "system_menu.h"

std::vector<std::string> *MenuStateManager::splitInput(const std::string &input, char delim) {
    _logger->trace("splitting input: '%s' with delimiter '%c'\n", input.c_str(), delim);
    auto *result = new std::vector<std::string>(0);
    _logger->trace("created initial result vector %i\n", result);

    std::string substr;
    for (char c : input) {
        if (c == delim) {
            _logger->debug("extracted substr: '%s'\n", substr.c_str());
            result->push_back(substr);
            substr.clear();
        } else {
            substr += c;
        }
    }
    _logger->debug("extracted substr: '%s'\n", substr.c_str());
    result->push_back(substr);

    return result;
}

std::string MenuStateManager::fillString(const std::string &s, int length, char c) {
    std::string chars(length, c);
    return s + chars.substr(0, length - s.size());
}

MenuStateManager::MenuStateManager(Node *rootNode, Logging *logger) : StateManager(rootNode, logger) {
    _path.push_back(_active);
}

bool MenuStateManager::handleCommand(std::string cmd) {
    _logger->debug("Handling command '%s'\n", cmd.c_str());
    std::vector<std::string> *cmdline = splitInput(_inputBuffer, ' ');
    _logger->trace("Command line consists of %i elements\n", cmdline->size());

    cmd = cmdline->front();
    Edge *edge = findEdgeByName(cmd, getPossibleTransitions());

    if (edge) {
        _logger->debug("found matching edge %s [%i->%i]\n", edge->getName().c_str(), edge->getFromNodeId(), edge->getToNodeId());
    } else {
        _logger->debug("found no matching edge for command '%s'\n", cmd.c_str());
    }
    Node *previous = _active;
    bool result = transition(edge, cmdline);
    if (result && (_active != previous)) {
        _path.push_back(_active);
    }
    delete cmdline;
    return result;
}

std::string MenuStateManager::getMenuString() {
    std::string menuString = "\n** " + _active->getName() + " **\n";
    auto transitions = getPossibleTransitions();
    menuString += "* .\n";
    if (_active != _start) menuString += "* ..\n";
    for (auto edge : *transitions) {
        menuString += fillString("* " + edge->getName() + ": ", 15, ' ') + edge->getDescription() + "\n";
    }

    return menuString;
}

std::string MenuStateManager::getPathString(const std::vector<Node *> &path) {
    std::string result;
    for (auto node : path) {
        result += node->getName() + "/";
    }
    return result;
}

void MenuStateManager::handleInput(char input_char) {
    if (input_char == 10) return; // skip CR
    _logger->trace("c: %c [%i]\n", input_char, input_char);

    if (input_char == 13) { // evaluate on LF
        _logger->debug("evaluating input '%s'\n", _inputBuffer.c_str());
        if (_inputBuffer == "/") {
            restart(true);
            _path.clear();
            _path.push_back(_active);
        } else if ((_inputBuffer == "..") && (_path.size() > 1)) {
            _path.pop_back();
            _active = _path.back();
        } else if (!(this->handleCommand(_inputBuffer))) {
            _logger->warn("Invalid command or command failed: %s\n", _inputBuffer.c_str());
        }
        Serial.println(this->getMenuString().c_str());
        Serial.print((getPathString(_path) + "> ").c_str());
        _inputBuffer.clear();
        return;
    } else {
        _inputBuffer += input_char;
        _logger->trace("serial_menu_cmd: '%s'\n", _inputBuffer.c_str());
    }
}
