#include "node.hpp"

Node::Node() {

}

Node::Node(std::string const& name) : name_(name), path_(name) {

}

Node Node::getParent() const {
    return *parent_;
}

void Node::setParent(Node & node) {
    parent_ = &node;
    depth_ = node.getDepth()+1;
    path_ = node.getPath();
    path_ = path_.append("/");
    path_ = path_.append(name_);
}

Node* Node::getChild(std::string const& name) const {
    auto it = std::find_if(children_.begin(), children_.end(), [name](Node* n) -> bool {return n->getName() == name;});
    return *it;
}

std::vector<Node*> Node::getChildren() const {
    return children_;
}

std::string Node::getName() const {
    return name_;
}

std::string Node::getPath() const {
    return path_;
}

int Node::getDepth() const {
    return depth_;
}

glm::mat4 Node::getLocalTransform() const {
    return localTransform_;
}

void Node::setLocalTransform(glm::mat4 const& localTransform) {
    localTransform_ = localTransform;
}

glm::mat4 Node::getWorldTransform() const {
    return worldTransform_;
}

void Node::setWorldTransform(glm::mat4 const& worldTransform) {
    worldTransform_ = worldTransform;
}

void Node::addChild(Node & node) {
    children_.push_back(&node);
    node.setParent(*this);
}

void Node::removeChild(std::string const& name) {
    auto it = std::find_if(children_.begin(), children_.end(), [name](Node* n) -> bool {return n->getName() == name;});
    children_.erase(it);
}

void Node::printSubGraph(std::ostream& out) const {
    out<<path_<<"\n";
    for (Node* child : children_) {
        child->printSubGraph(out);
    }
}