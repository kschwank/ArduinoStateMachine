#ifndef FIRMWARE_SYSTEM_STATE_MACHINE_H
#define FIRMWARE_SYSTEM_STATE_MACHINE_H

#include <string>
#include <map>
#include <utility>
#include <vector>

#include "ArduinoLog.h"

typedef int32_t node_id_t;

class Node;
class Edge;
class StateManager;

typedef struct {
    Node* node;
    node_id_t nodeId;
    long data;
    void* extData;
    std::vector<std::string> *args;
} node_event_data;

typedef struct {
    Edge* edge;
    long data;
    void *extData;
    std::vector<std::string> *args;
} edge_event_data;

// State Event Definitions
typedef void (*node_enter_event)(node_event_data);
typedef bool (*node_before_enter_event)(node_event_data);
typedef bool (*node_before_exit_event)(node_event_data);

typedef bool (*edge_event)(edge_event_data);

#define DEFAULT_ROOT_NODE_ID 0

class Node {
    friend StateManager;

private:
    node_id_t _id;
    std::string _name;
    node_enter_event _onEnter;
    node_before_enter_event _beforeEnter;
    node_before_exit_event _beforeExit;
    long _data;
    void *_extData;
public:
    Node(node_id_t id, std::string name, node_enter_event onEnter = nullptr, node_before_enter_event beforeEnter = nullptr, node_before_exit_event beforeExit = nullptr, long data = 0, void *extData = nullptr);

    node_id_t getId();
    std::string getName();

    bool beforeEnter(node_id_t fromNode);
    void onEnter(std::vector<std::string> *args = nullptr);
    bool beforeExit(node_id_t toNode);
};

class Edge {
    friend StateManager;

private:
    std::string _name;
    std::string _description;
    std::pair<node_id_t, node_id_t> _fromToTuple;
    edge_event _onTransition;
    long _data;
    void *_extData;
public:
    Edge(std::string name, std::string description, node_id_t startId, node_id_t endId, edge_event onTransition = nullptr, long data = 0, void *extData = nullptr);

    std::string getName();
    std::string getDescription();
    node_id_t getFromNodeId();
    node_id_t getToNodeId();

    bool onTransition(std::vector<std::string> *args = nullptr);
};

class StateManager {
protected:
    Node *_start;
    Node *_active;
    std::map<node_id_t, Node*> _nodes;
    std::map<node_id_t, std::vector<Edge*>> _edges;
    Logging *_logger;

public:
    explicit StateManager(Node *startNode = nullptr, Logging *logger = &Log);

    void restart(bool runOnEnter = true);

    bool transition(Edge *edge, std::vector<std::string> *args = nullptr);

    Node *getActiveNode();

    void addNode(Node *newNode) {
        _logger->debug("adding node with id [%i]\n", newNode->_id);
        _nodes.insert(std::make_pair(newNode->_id, newNode));
    }

    void removeNode(node_id_t nodeId);

    void addEdge(Edge *newEdge);

    std::vector<Edge*> *getPossibleTransitions(Node* node = nullptr);

    Edge *findEdgeByName(const std::string& name, std::vector<Edge*> *transitions);

    void dump();
};

#endif //FIRMWARE_SYSTEM_STATE_MACHINE_H
