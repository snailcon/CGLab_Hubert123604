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
    
    // recursive function to get all nodes
    root_->getChildren(all_nodes);

    std::vector<GeometryNode*> geom_nodes;

    // filter out non-GeometryNodes with dynamic typing
    for (auto& child : all_nodes) {
        GeometryNode* geom_node = dynamic_cast<GeometryNode*>(child);
        if (geom_node != nullptr) {
            geom_nodes.push_back(geom_node);
        }
    }

    return geom_nodes;
}

std::vector<PointlightNode*> SceneGraph::getPointlightNodes() const{
    std::vector<Node*> all_nodes;
    
    // recursive function to get all nodes
    root_->getChildren(all_nodes);

    std::vector<PointlightNode*> pl_nodes;

    // filter out non-GeometryNodes with dynamic typing
    for (auto& child : all_nodes) {
        PointlightNode* pl_node = dynamic_cast<PointlightNode*>(child);
        if (pl_node != nullptr) {
            pl_nodes.push_back(pl_node);
        }
    }

    return pl_nodes;
}

void SceneGraph::printGraph() {
    root_->printSubGraph(std::cout);
}