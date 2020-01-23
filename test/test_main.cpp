#include <Arduino.h>
#include <ArduinoLog.h>

#include <unity.h>

#include "system_menu.h"

MenuStateManager *setupMenu() {
    MenuStateManager *menuStateManager = new MenuStateManager();
    menuStateManager->addNode(new MenuItem(1, "01"));
    menuStateManager->addNode(new MenuItem(2, "02"));
    menuStateManager->addNode(new MenuItem(11, "011"));
    menuStateManager->addNode(new MenuItem(12, "012"));
    menuStateManager->addNode(new MenuItem(21, "021"));
    menuStateManager->addNode(new MenuItem(22, "022"));

    menuStateManager->addEdge(new MenuItemLink("1", "0->1", 0, 1));
    menuStateManager->addEdge(new MenuItemLink("0", "1->0", 1, 0));
    menuStateManager->addEdge(new MenuItemLink("2", "0->2", 0, 2));
    menuStateManager->addEdge(new MenuItemLink("0", "2->0", 2, 0));
    menuStateManager->addEdge(new MenuItemLink("11","1->11", 1, 11));
    menuStateManager->addEdge(new MenuItemLink("12","1->12", 1, 12));
    menuStateManager->addEdge(new MenuItemLink("1", "11->1", 11, 1));
    menuStateManager->addEdge(new MenuItemLink("1", "12->1", 12, 1));
    menuStateManager->addEdge(new MenuItemLink("21","2->21", 2, 21));
    menuStateManager->addEdge(new MenuItemLink("22","2->22", 2, 22));
    menuStateManager->addEdge(new MenuItemLink("2", "21->2", 21, 2));
    menuStateManager->addEdge(new MenuItemLink("2", "22->2", 22, 2));

    return menuStateManager;
}

MenuStateManager *menuStateManager;

void setup() {
    delay(2000);
    UNITY_BEGIN();

//    Serial.begin(115200);
    Log.begin(LOG_LEVEL_VERBOSE, &Serial, true);

    Serial.println("Starting Tests");

    menuStateManager = setupMenu();

    Serial.println(menuStateManager->getMenuString().c_str());
    menuStateManager->handleCommand("1");
    Serial.println(menuStateManager->getMenuString().c_str());

    Serial.println("Tests finished.");
}

void loop() {}
