#ifndef FIRMWARE_SYSTEM_STATE_MACHINE_H
#define FIRMWARE_SYSTEM_STATE_MACHINE_H

#include <string>
#include <map>
#include <vector>

#include "ArduinoLog.h"

typedef int32_t node_id_t;
typedef int32_t edge_id_t;

class Node;
class Edge;
class StateManager;

typedef struct {
    Node* node;
    node_id_t nodeId;
    long data;
    void* extData;
} node_event_data;

typedef struct {
    Edge* edge;
    long data;
    void *extData;
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
    Node(node_id_t id, std::string name, node_enter_event onEnter = nullptr, node_before_enter_event beforeEnter = nullptr, node_before_exit_event beforeExit = nullptr, long data = 0, void *extData = nullptr): _id(id), _name(name), _onEnter(onEnter), _beforeEnter(beforeEnter), _beforeExit(beforeEnter), _data(data), _extData(extData) {}

    node_id_t getId() { return _id; }
    std::string getName() { return _name; }

    bool beforeEnter(node_id_t fromNode) { if (_beforeEnter) return _beforeEnter({ .node =  this, .nodeId = fromNode, .data = this->_data, .extData = _extData }); return true; }
    void onEnter() { if (_onEnter) _onEnter({ .node =  this, .nodeId = this->_id, .data = this->_data, .extData = _extData }); }
    bool beforeExit(node_id_t toNode) { if (_beforeExit) return _beforeExit({ .node =  this, .nodeId = toNode, .data = this->_data, .extData = _extData }); return true; }
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
    Edge(std::string name, std::string description, node_id_t startId, node_id_t endId, edge_event onTransition = nullptr, long data = 0, void *extData = nullptr)
    : _name(name), _description(description), _fromToTuple(std::make_pair(startId, endId)), _onTransition(onTransition), _data(data), _extData(extData) { }

    std::string getName() { return _name; }
    std::string getDescription() { return _description; }
    node_id_t getFromNodeId() { return _fromToTuple.first; }
    node_id_t getToNodeId() { return _fromToTuple.second; }

    bool onTransition() { if (_onTransition) { return _onTransition({ .edge = this, .data = _data, .extData = _extData }); } return true; }
};

class StateManager {
protected:
    Node *_start;
    Node *_active;
    std::map<node_id_t, Node*> _nodes;
    std::map<node_id_t, std::vector<Edge*>> _edges;
public:
    explicit StateManager(Node *startNode = nullptr): _start(startNode) {
        if (!_start) {
            _start = new Node(DEFAULT_ROOT_NODE_ID, "");
        }
        _nodes = { { _start->_id, _start } };
        _edges = { };
        _active = _start;
    }

    void restart(bool runOnEnter = true) {
        _active = _start;
        if (runOnEnter) {
            _active->onEnter();
        }
    }

    void setStartNode(Node *startNode) {
        _active = startNode;
    }

    bool transition(Edge *edge) {
        if (_active->_id == edge->getFromNodeId()) {
            Log.trace("looking for target node [id:%i]\n", edge->getToNodeId());
            Node *toNode = _nodes.at(edge->getToNodeId());
            if (!toNode) {
                Log.trace("target node does not exist.\n");
                return false;
            }
            Log.trace("found target node.\n");
            Log.trace("calling beforeExit() on node [%i]\n", _active->_id);
            if (!_active->beforeExit(edge->getToNodeId())) {
                Log.trace("beforeExit returned false. Aborting transition.");
                return false;
            }
            Log.trace("calling onTransition() on edge [%i->%i]\n", edge->getFromNodeId(), edge->getToNodeId());
            if (!edge->onTransition()) {
                Log.trace("onTransition() returned false. Aborting transition.");
                return false;
            }
            Log.trace("calling beforeEnter() on node [%i]\n", edge->getToNodeId());
            if (!toNode->beforeEnter(edge->getFromNodeId())) {
                Log.trace("beforeEnter returned false. Aborting transition.");
                return false;
            }
            Log.trace("activating node [%i]\n", edge->getToNodeId());
            _active = toNode;
            Log.trace("calling onEnter() on node [%i]\n", _active->_id);
            _active->onEnter();
        } else {
            Log.trace("edge does not start from node [%i]\n", _active->_id);
            return false;
        }
        return true;
    }

    Node *getActiveNode() {
        Log.trace("active node is '%s'\n", _active->getName().c_str());
        return _active;
    }

    void addNode(Node *newNode) {
        Log.trace("adding node with id [%i]\n", newNode->_id);
        _nodes.insert(std::make_pair(newNode->_id, newNode));
    }

    void removeNode(node_id_t nodeId) {
        Log.warning("removeNode is not fully implemented, yet. Edges might still point to this node!");
        _nodes.erase(nodeId);
    }

    void addEdge(Edge *newEdge) {
        Log.trace("adding edge %s (%s) [%i->%i]\n", newEdge->getName().c_str(), newEdge->getDescription().c_str(), newEdge->getFromNodeId(), newEdge->getToNodeId());
        _edges[newEdge->getFromNodeId()].push_back(newEdge);
    }

    std::vector<Edge*> *getPossibleTransitions(Node* node = nullptr) {
        if (node == nullptr) node = _active;

        Log.trace("getting available transition for node [%i]\n", node->_id);
        return &_edges[node->_id];
    }

    Edge *findEdgeByName(std::string name, std::vector<Edge*> *transitions) {
        Log.trace("searching for edge by name '%s'\n", name.c_str());
        transitions->begin();
        for (auto edge : *transitions) {
            if (name == edge->getName()) {
                return edge;
            }
        }
        return nullptr;
    }

    void dump() {
        _nodes.begin();
        for (auto node : _nodes) {
            Log.trace("node: { id: %i, name: %s }\n", node.second->getId(), node.second->getName().c_str());
        }

        _edges.begin();
        for (auto const &edgeentry : _edges) {
            for (auto edge : edgeentry.second) {
                Log.trace("edge %s (%s) [%i->%i]\n", edge->getName().c_str(), edge->getDescription().c_str(), edge->getFromNodeId(), edge->getToNodeId());
            }
        }
    }
};

#endif //FIRMWARE_SYSTEM_STATE_MACHINE_H
