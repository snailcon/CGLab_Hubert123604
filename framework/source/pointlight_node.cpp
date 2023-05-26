#include "pointlight_node.hpp"

PointlightNode::PointlightNode(std::string const& name, glm::vec3 const& color, float intensity)
    : Node(name), color_(color), intensity_(intensity) {

}

void PointlightNode::setIntensity(float intensity) {
    intensity_ = intensity;
}

float PointlightNode::getIntensity() const {
    return intensity_;
}


void PointlightNode::setColor(glm::vec3 const& color) {
    color_ = color;
}

glm::vec3 PointlightNode::getColor() const {
    return color_;
}


void PointlightNode::printSubGraph(std::ostream& out) const {
    out<<"light: ";
    Node::printSubGraph(out);
}