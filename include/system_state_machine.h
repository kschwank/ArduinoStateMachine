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

// State Event Definitions
typedef void (*node_enter_event)(Node *node);
typedef bool (*node_before_enter_event)(node_id_t);
typedef bool (*node_before_exit_event)(node_id_t);

typedef bool (*edge_event)(Edge *edge);

class Node {
    friend StateManager;

private:
    node_id_t _id;
    std::string _name;
    node_enter_event _onEnter;
    node_before_enter_event _beforeEnter;
    node_before_exit_event _beforeExit;
public:
    Node(node_id_t id, std::string name, node_enter_event onEnter = nullptr, node_before_enter_event beforeEnter = nullptr, node_before_exit_event beforeExit = nullptr): _id(id), _name(name), _onEnter(onEnter), _beforeEnter(beforeEnter), _beforeExit(beforeEnter) {}

    node_id_t getId() { return _id; }
    std::string getName() { return _name; }

    bool beforeEnter(node_id_t fromNode) { if (_beforeEnter) return beforeEnter(fromNode); return true; }
    void onEnter() { if (_onEnter) _onEnter(this); }
    bool beforeExit(node_id_t toNode) { if (_beforeExit) return _beforeExit(toNode); return true; }
};

class Edge {
    friend StateManager;

private:
    std::pair<node_id_t, node_id_t> _fromToTuple;
    edge_event _onTransition;
public:
    Edge(node_id_t startId, node_id_t endId, edge_event onTransition = nullptr): _fromToTuple(std::make_pair(startId, endId)), _onTransition(onTransition) { }

    node_id_t getFromNodeId() { return _fromToTuple.first; }
    node_id_t getToNodeId() { return _fromToTuple.second; }

    bool onTransition() { if (_onTransition) { return _onTransition(this); } return true; }
};

class StateManager {
private:
    Node *_start;
    Node *_active;
    std::map<node_id_t, Node*> _nodes;
    std::map<node_id_t, std::vector<Edge*>> _edges;
public:
    explicit StateManager(Node *startNode): _start(startNode) {
        _nodes = { {startNode->_id, startNode } };
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
        Log.trace("active node is %i", (void*)_active);
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
        Log.trace("adding edge [%i->%i]\n", newEdge->getFromNodeId(), newEdge->getToNodeId());
        _edges[newEdge->getFromNodeId()].push_back(newEdge);
    }

    std::vector<Edge*> *getPossibleTransitions(Node* node = nullptr) {
        if (node == nullptr) node = _active;

        Log.trace("getting available transition for node [%i]\n", node->_id);
        return &_edges[node->_id];
    }
};

#endif //FIRMWARE_SYSTEM_STATE_MACHINE_H
