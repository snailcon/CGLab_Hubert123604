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

Node* SceneGraph::getRoot() const {
    return root_.get();
}

void SceneGraph::setRoot(std::shared_ptr<Node> node) {
    root_ = node;
}

std::vector<GeometryNode*> SceneGraph::getGeomNodes() const{
    std::vector<Node*> all_nodes;
    
    root_->getChildren(all_nodes);

    std::vector<GeometryNode*> geom_nodes;

    for (auto& child : all_nodes) {
        GeometryNode* geom_node = dynamic_cast<GeometryNode*>(child);
        if (geom_node != nullptr) {
            geom_nodes.push_back(geom_node);
        }
    }

    return geom_nodes;
}

void SceneGraph::printGraph() {
    root_->printSubGraph(std::cout);
}