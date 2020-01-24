#include <Arduino.h>
#include <ArduinoLog.h>

#include <unity.h>

#include "system_menu.h"

bool set_log_level(edge_event_data eventData) {
    Log.setLevel(eventData.data);
    Log.notice("Log level set to %i\n", eventData.data);
    return true;
}

void setupMenu(MenuStateManager *menuStateManager) {
    menuStateManager->addNode(new Node(1, "Status"));
    menuStateManager->addNode(new Node(2, "Sensor Configuration"));
    menuStateManager->addNode(new Node(3, "System Configuration"));

    menuStateManager->addNode(new Node(31, "Log Level"));

    // main menu
    menuStateManager->addEdge(new Edge("status", "Show status information", DEFAULT_ROOT_NODE_ID, 1));
    menuStateManager->addEdge(new Edge("sensors", "Sensor Configuration", DEFAULT_ROOT_NODE_ID, 2));
    menuStateManager->addEdge(new Edge("system", "System Configuration", DEFAULT_ROOT_NODE_ID, 3));

    // status menu
    menuStateManager->addEdge(new Edge("info", "Show System Info", 1, 1));
    menuStateManager->addEdge(new Edge("..", "Back to Main Menu", 1, DEFAULT_ROOT_NODE_ID));

    // sensors menu
    menuStateManager->addEdge(new Edge("..", "Back to Main Menu", 2, DEFAULT_ROOT_NODE_ID));

    // system menu
    menuStateManager->addEdge(new Edge("log", "Configure Logging", 3, 31));
    menuStateManager->addEdge(new Edge("reboot", "Reboot System", 3, 3));
    menuStateManager->addEdge(new Edge("..", "Back to Main Menu", 3, DEFAULT_ROOT_NODE_ID));

    // log menu
    menuStateManager->addEdge(new Edge("0", "SILENT", 31, 31, set_log_level, LOG_LEVEL_SILENT));
    menuStateManager->addEdge(new Edge("1", "FATAL", 31, 31, set_log_level, LOG_LEVEL_FATAL));
    menuStateManager->addEdge(new Edge("2", "ERROR", 31, 31, set_log_level, LOG_LEVEL_ERROR));
    menuStateManager->addEdge(new Edge("3", "WARNING", 31, 31, set_log_level, LOG_LEVEL_WARNING));
    menuStateManager->addEdge(new Edge("4", "NOTICE", 31, 31, set_log_level, LOG_LEVEL_NOTICE));
    menuStateManager->addEdge(new Edge("5", "TRACE", 31, 31, set_log_level, LOG_LEVEL_TRACE));
    menuStateManager->addEdge(new Edge("6", "VERBOSE", 31, 31, set_log_level, LOG_LEVEL_VERBOSE));
    menuStateManager->addEdge(new Edge("..", "Back to System Configuration", 31, 3));

}

void setupMenu1(MenuStateManager *menuStateManager) {
    menuStateManager = new MenuStateManager();

    menuStateManager->addNode(new Node(1, "01"));
    menuStateManager->addNode(new Node(2, "02"));
    menuStateManager->addNode(new Node(11, "011"));
    menuStateManager->addNode(new Node(12, "012"));
    menuStateManager->addNode(new Node(21, "021"));
    menuStateManager->addNode(new Node(22, "022"));

    menuStateManager->addEdge(new Edge("1", "0->1", 0, 1));
    menuStateManager->addEdge(new Edge("0", "1->0", 1, 0));
    menuStateManager->addEdge(new Edge("2", "0->2", 0, 2));
    menuStateManager->addEdge(new Edge("0", "2->0", 2, 0));
    menuStateManager->addEdge(new Edge("11","1->11", 1, 11));
    menuStateManager->addEdge(new Edge("12","1->12", 1, 12));
    menuStateManager->addEdge(new Edge("1", "11->1", 11, 1));
    menuStateManager->addEdge(new Edge("1", "12->1", 12, 1));
    menuStateManager->addEdge(new Edge("21","2->21", 2, 21));
    menuStateManager->addEdge(new Edge("22","2->22", 2, 22));
    menuStateManager->addEdge(new Edge("2", "21->2", 21, 2));
    menuStateManager->addEdge(new Edge("2", "22->2", 22, 2));
}

bool printTransition(edge_event_data eventData) {
    Serial.printf("transitioning from %i to %i\n", eventData.edge->getFromNodeId(), eventData.edge->getToNodeId());
    return true;
}

MenuStateManager menuStateManager;

void setup() {
    delay(2000);
    UNITY_BEGIN();

    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);

    Serial.println("Setting up menu.");

    setupMenu(&menuStateManager);
    menuStateManager.dump();

    Serial.println("Setup finished.");
}

std::string serial_menu_cmd;

void loop() {

    while (Serial.available()) {
        char c = (char)Serial.read();

        menuStateManager.handleInput(c);
//        if (c == 10) continue; // skip CR
//        Log.trace("c: %c [%i]\n", c, c);
//
//        if (c == 13) {
//            Log.trace("evaluating input '%s'\n", serial_menu_cmd.c_str());
//            if (!(menuStateManager.handleCommand(serial_menu_cmd))) {
//                Log.warning("Invalid command or command failed: %s\n", serial_menu_cmd.c_str());
//            }
//            serial_menu_cmd.clear();
//            Serial.println(menuStateManager.getMenuString().c_str());
//            break;
//        } else {
//            serial_menu_cmd += c;
//            Log.trace("serial_menu_cmd: '%s'\n", serial_menu_cmd.c_str());
//        }
    }
}
