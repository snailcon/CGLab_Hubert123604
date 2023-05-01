#include "geometry_node.hpp"

GeometryNode::GeometryNode(std::string const& name, model_object const& geometry) : Node(name), model_(geometry){

}

model_object GeometryNode::getGeometry() const {
    return model_;
}

void GeometryNode::setGeometry(model_object const& geometry) {
    model_ = geometry;
}

void GeometryNode::printSubGraph(std::ostream& out) const {
    out<<"geom: ";
    Node::printSubGraph(out);
}