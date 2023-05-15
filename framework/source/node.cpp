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
    // update depth and path information from new parent
    depth_ = node->getDepth()+1;
    path_ = node->getPath();
    path_ = path_.append("/");
    path_ = path_.append(name_);
}

Node* Node::getChild(std::string const& name) const {
    // find the first child with the given name
    auto it = std::find_if(children_.begin(), children_.end(), [name](std::shared_ptr<Node> n) -> bool {return n->getName() == name;});
    return it->get();
}

std::vector<Node*> Node::getChildren() const {
    // I don't want to return a vector of shared pointers
    // so I get the raw pointers first
    // the pointers should always point to valid memory
    std::vector<Node*> children;
    for (auto&& child : children_) {
        children.push_back(child.get());
    }
    return children;
}

// gets all children from this point of the tree recursivly
// should have another name, but couldn't think of one
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

void Node::translate(glm::vec3 const& offset) {
    translation_ = glm::translate(translation_, offset);
    updateLocalTransform();
}

void Node::rotate(glm::vec3 const& axis, float angle) {
    rotation_ = glm::rotate(rotation_, angle, axis);
    updateLocalTransform();
}

void Node::setTranslation(glm::vec3 const& position) {
    translation_ = glm::translate(glm::mat4(1.0f), position);
    updateLocalTransform();
}

void Node::setRotation(glm::vec3 const& axis, float angle) {
    rotation_ = glm::rotate(glm::mat4(1.0f), angle, axis);
    updateLocalTransform();
}

void Node::setScale(glm::vec3 const& scale) {
    scale_ = glm::scale(glm::mat4(1.0f), scale);
    updateLocalTransform();
}


glm::mat4 Node::getTranslation() const {
    return translation_;
}

glm::mat4 Node::getRotation() const {
    return rotation_;
}

glm::mat4 Node::getScale() const {
    return scale_;
}

glm::mat4 Node::getLocalTransform() const {
    return localTransform_;
}

void Node::updateLocalTransform() {
    localTransform_ = translation_ * rotation_ * scale_;
    if (parent_ != nullptr) {
        setWorldTransform(parent_->getWorldTransform() * localTransform_);
    } else { // if no parent exists, the worldtransform is just the localtransform
        setWorldTransform(localTransform_);
    }
}

glm::mat4 Node::getWorldTransform() const {
    return worldTransform_;
}

void Node::setWorldTransform(glm::mat4 const& worldTransform) {
    worldTransform_ = worldTransform;

    // when the worldtransform changes, the children need to be updated
    for (auto&& child : children_) {
        child->setWorldTransform(worldTransform_ * child->getLocalTransform());
    }
}

void Node::addChild(std::shared_ptr<Node> node) {
    children_.push_back(node);
    // when a child is added, it should know its parent node
    node->setParent(this);
}

void Node::removeChild(std::string const& name) {
    // find the first child with the given name and erase it
    auto it = std::find_if(children_.begin(), children_.end(), [name](std::shared_ptr<Node> n) -> bool {return n->getName() == name;});
    children_.erase(it);
}

void Node::printSubGraph(std::ostream& out) const {
    out<<path_<<"\n";
    for (auto&& child : children_) {
        child->printSubGraph(out);
    }
}