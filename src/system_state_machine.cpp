#include "system_state_machine.h"

Node::Node(node_id_t id, std::string name, node_enter_event onEnter, node_before_enter_event beforeEnter,
           node_before_exit_event beforeExit, long data, void *extData) : _id(id), _name(std::move(name)), _onEnter(onEnter), _beforeEnter(beforeEnter), _beforeExit(beforeExit), _data(data), _extData(extData) {}

node_id_t Node::getId() { return _id; }

std::string Node::getName() { return _name; }

bool Node::beforeEnter(node_id_t fromNode) { if (_beforeEnter) return _beforeEnter({ .node =  this, .nodeId = fromNode, .data = this->_data, .extData = _extData }); return true; }

void Node::onEnter(std::vector<std::string> *args) { if (_onEnter) _onEnter({ .node =  this, .nodeId = this->_id, .data = this->_data, .extData = _extData, .args = args }); }

bool Node::beforeExit(node_id_t toNode) { if (_beforeExit) return _beforeExit({ .node =  this, .nodeId = toNode, .data = this->_data, .extData = _extData }); return true; }

Edge::Edge(std::string name, std::string description, node_id_t startId, node_id_t endId, edge_event onTransition,
           long data, void *extData)
        : _name(std::move(name)), _description(std::move(description)), _fromToTuple(std::make_pair(startId, endId)), _onTransition(onTransition), _data(data), _extData(extData) { }

std::string Edge::getName() { return _name; }

std::string Edge::getDescription() { return _description; }

node_id_t Edge::getFromNodeId() { return _fromToTuple.first; }

node_id_t Edge::getToNodeId() { return _fromToTuple.second; }

bool Edge::onTransition(std::vector<std::string> *args) { if (_onTransition) { return _onTransition({ .edge = this, .data = _data, .extData = _extData, .args = args }); } return true; }

StateManager::StateManager(Node *startNode, Logging *logger) : _start(startNode), _logger(logger) {
    if (!_start) {
        _start = new Node(DEFAULT_ROOT_NODE_ID, "");
    }
    _nodes = { { _start->_id, _start } };
    _edges = { };
    _active = _start;
}

void StateManager::restart(bool runOnEnter) {
    _active->beforeExit(DEFAULT_ROOT_NODE_ID);

    _active = _start;
    if (runOnEnter) {
        _active->onEnter();
    }
}

bool StateManager::transition(Edge *edge, std::vector<std::string> *args) {
    if (edge && (_active->_id == edge->getFromNodeId())) {
        _logger->debug("looking for target node [id:%i]\n", edge->getToNodeId());
        Node *toNode = _nodes.at(edge->getToNodeId());
        if (!toNode) {
            _logger->debug("target node does not exist.\n");
            return false;
        }
        _logger->debug("found target node.\n");
        _logger->debug("calling beforeExit() on node [%i]\n", _active->_id);
        if (!_active->beforeExit(edge->getToNodeId())) {
            _logger->debug("beforeExit returned false. Aborting transition.");
            return false;
        }
        _logger->debug("calling onTransition() on edge [%i->%i]\n", edge->getFromNodeId(), edge->getToNodeId());
        if (!edge->onTransition(args)) {
            _logger->debug("onTransition() returned false. Aborting transition.");
            return false;
        }
        _logger->debug("calling beforeEnter() on node [%i]\n", edge->getToNodeId());
        if (!toNode->beforeEnter(edge->getFromNodeId())) {
            _logger->debug("beforeEnter returned false. Aborting transition.");
            return false;
        }
        _logger->debug("activating node [%i]\n", edge->getToNodeId());
        _active = toNode;
        _logger->debug("calling onEnter() on node [%i]\n", _active->_id);
        _active->onEnter(args);
    } else if (!edge) {
        _logger->debug("no edge passed, executing onEnter of current node [%i]\n", _active->_id);
        _active->onEnter(args);
    } else {
        _logger->debug("edge does not start from node [%i]\n", _active->_id);
        return false;
    }
    return true;
}

Node *StateManager::getActiveNode() {
    _logger->debug("active node is '%s'\n", _active->getName().c_str());
    return _active;
}

void StateManager::removeNode(node_id_t nodeId) {
    _logger->warn("removeNode is not fully implemented, yet. Edges might still point to this node!");
    _nodes.erase(nodeId);
}

void StateManager::addEdge(Edge *newEdge) {
    _logger->debug("adding edge %s (%s) [%i->%i]\n", newEdge->getName().c_str(), newEdge->getDescription().c_str(), newEdge->getFromNodeId(), newEdge->getToNodeId());
    _edges[newEdge->getFromNodeId()].push_back(newEdge);
}

std::vector<Edge *> *StateManager::getPossibleTransitions(Node *node) {
    if (node == nullptr) node = _active;

    _logger->debug("getting available transition for node [%i]\n", node->_id);
    return &_edges[node->_id];
}

Edge *StateManager::findEdgeByName(const std::string &name, std::vector<Edge *> *transitions) {
    _logger->debug("searching for edge by name '%s'\n", name.c_str());
    transitions->begin();
    for (auto edge : *transitions) {
        if (name == edge->getName()) {
            return edge;
        }
    }
    return nullptr;
}

void StateManager::dump() {
    _nodes.begin();
    for (auto node : _nodes) {
        _logger->debug("node: { id: %i, name: %s }\n", node.second->getId(), node.second->getName().c_str());
    }

    _edges.begin();
    for (auto const &edgeentry : _edges) {
        for (auto edge : edgeentry.second) {
            _logger->debug("edge %s (%s) [%i->%i]\n", edge->getName().c_str(), edge->getDescription().c_str(), edge->getFromNodeId(), edge->getToNodeId());
        }
    }
}
