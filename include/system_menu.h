//
// Created by ksh on 21.01.20.
//

#ifndef STATEMACHINE_SYSTEM_MENU_H
#define STATEMACHINE_SYSTEM_MENU_H

#include "system_state_machine.h"

typedef void (*onEnterMenuItem)();

class MenuItem: public Node {
public:
    MenuItem(node_id_t id, std::string name, node_enter_event onEnter = nullptr, node_before_enter_event beforeEnter = nullptr, node_before_exit_event beforeExit = nullptr)
    : Node(id, name, onEnter, beforeEnter, beforeExit) {
        // ctor
    }
};

class MenuItemLink: public Edge {
    std::string _entryName;
    std::string _entryDescription;

public:
    MenuItemLink(Edge *edge) : Edge(edge->getFromNodeId(), edge->getToNodeId()) {
        _entryName = "Item: " + getToNodeId();
        _entryDescription = ""; // std::string("Link from ", getFromNodeId(), " to " + getToNodeId());
    }
    MenuItemLink(std::string entryName, std::string entryDescription, node_id_t startId, node_id_t endId): Edge(startId, endId), _entryName(entryName), _entryDescription(entryDescription) { }

    std::string getEntryName() {
        return _entryName;
    }

    std::string getEntryDescription() {
        return _entryDescription;
    }
};

class MenuStateManager : public StateManager {

private:
    MenuItemLink *findItemLinkByKey(std::string entryName, std::vector<Edge*> *transitions) {
        transitions->begin();
        for (auto *edge : *transitions) {
            MenuItemLink *itemLink = (MenuItemLink*)edge;
            if (entryName.compare(itemLink->getEntryName()) == 0) {
                return itemLink;
            }
        }
        return nullptr;
    }
public:
    MenuStateManager(MenuItem *rootMenuItem = nullptr): StateManager(rootMenuItem) { }

    bool handleCommand(std::string cmd) {
        MenuItemLink *menuItemLink = findItemLinkByKey(cmd, getPossibleTransitions());
        if (menuItemLink) {
            return transition(menuItemLink);
        }
        return false;
    }

    virtual std::string getMenuString() {
        std::string menuString = "Menu: '" + getActiveNode()->getName() + "':\n";

        auto transitions = getPossibleTransitions();
        for (Edge *edge : *transitions) {
            MenuItemLink *itemLink = (MenuItemLink*)edge;
            menuString += itemLink->getEntryName() + ": " + itemLink->getEntryDescription() + "\n";
        }

        return menuString;
    }
};

#endif //STATEMACHINE_SYSTEM_MENU_H
