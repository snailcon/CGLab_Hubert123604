#include "scenegraph.hpp"

SceneGraph& SceneGraph::getInstance() {
    static SceneGraph instance;
    return instance;
}

std::string SceneGraph::getName() const {
    return name_;
}

void SceneGraph::setName(std::string const& name) {
    name_ = name;
}

Node SceneGraph::getRoot() const {
    return root_;
}

void SceneGraph::setRoot(Node const& node) {
    root_ = node;
}

void SceneGraph::printGraph() {

}