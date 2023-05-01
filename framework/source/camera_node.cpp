#include "camera_node.hpp"

CameraNode::CameraNode(std::string const& name, glm::mat4 const& projectionMatrix) : Node(name), projectionMatrix_(projectionMatrix_) {

}

bool CameraNode::getPerspective() const {
    return isPerspective_;
}

bool CameraNode::getEnabled() const {
    return isEnabled_;
}

void CameraNode::setEnabled(bool enabled) {
    isEnabled_ = enabled;
}

glm::mat4 CameraNode::getProjectionMatrix() const {
    return projectionMatrix_;
}

void CameraNode::setProjectionMatrix(glm::mat4 const& projectionMatrix) {
    projectionMatrix_ = projectionMatrix;
}

void CameraNode::printSubGraph(std::ostream& out) const {
    out<<"cam: ";
    Node::printSubGraph(out);
}