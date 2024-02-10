#include "Nodes.h"

void Node::addFather(Node *parent) {
    this->father = parent;
}

void Node::addNode(Node* parent, Node* child) {
    this->children.push_back(child);
    //this->parent = parent;
}
Node::Node(string name, uint32_t length, uint32_t offset) {
    pathName = name;
    elementLength = length;
    elementOffset = offset;
}
