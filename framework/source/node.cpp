#include "node.hpp"

Node::Node() {

}

Node::Node(std::string const& name) : name_(name), path_(name) {

}

Node* Node::getParent() const {
    return parent_;
}

void Node::setParent(Node* node) {
    parent_ = node;
    depth_ = node->getDepth()+1;
    path_ = node->getPath();
    path_ = path_.append("/");
    path_ = path_.append(name_);
}

Node* Node::getChild(std::string const& name) const {
    auto it = std::find_if(children_.begin(), children_.end(), [name](std::shared_ptr<Node> n) -> bool {return n->getName() == name;});
    return it->get();
}

std::vector<Node*> Node::getChildren() const {
    std::vector<Node*> children;
    for (auto&& child : children_) {
        children.push_back(child.get());
    }
    return children;
}

void Node::getChildren(std::vector<Node*> & nodes) {
    for (auto&& child : children_) {
        child->getChildren(nodes);
    }
    nodes.push_back(this);
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
    if (parent_ != nullptr) {
        setWorldTransform(parent_->getWorldTransform() * localTransform_);
    } else {
        setWorldTransform(localTransform_);
    }

    for (auto&& child : children_) {
        child->setWorldTransform(getWorldTransform() * child->getLocalTransform());
    }
}

glm::mat4 Node::getWorldTransform() const {
    return worldTransform_;
}

void Node::setWorldTransform(glm::mat4 const& worldTransform) {
    worldTransform_ = worldTransform;
}

void Node::addChild(std::shared_ptr<Node> node) {
    children_.push_back(node);
    node->setParent(this);
}

void Node::removeChild(std::string const& name) {
    auto it = std::find_if(children_.begin(), children_.end(), [name](std::shared_ptr<Node> n) -> bool {return n->getName() == name;});
    children_.erase(it);
}

void Node::printSubGraph(std::ostream& out) const {
    out<<path_<<"\n";
    for (auto&& child : children_) {
        child->printSubGraph(out);
    }
}