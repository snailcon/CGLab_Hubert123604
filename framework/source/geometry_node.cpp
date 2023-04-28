#include "geometry_node.hpp"

GeometryNode::GeometryNode(std::string const& name, model const& geometry) : Node(name), model_(geometry){

}

model GeometryNode::getGeometry() const {
    return model_;
}

void GeometryNode::setGeometry(model const& geometry) {
    model_ = geometry;
}