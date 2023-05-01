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

glm::mat4 CameraNode::getViewMatrix() const {
    return viewMatrix_;
}

void CameraNode::setProjectionMatrix(glm::mat4 const& projectionMatrix) {
    projectionMatrix_ = projectionMatrix;
}

void CameraNode::processKeyboard(Camera_Movement dir, float delta) {
    float velocity = movespeed_ * delta;
    if (dir == FORWARD)
        cameraPos_ = cameraPos_ + cameraFront_ * velocity;
    if (dir == BACKWARD)
        cameraPos_ = cameraPos_ - cameraFront_ * velocity;
    if (dir == LEFT)
        cameraPos_ = cameraPos_ - cameraRight_ * velocity;
    if (dir == RIGHT)
        cameraPos_ = cameraPos_ + cameraRight_ * velocity;
    if (dir == UP)
        cameraPos_ = cameraPos_ - cameraUp_ * velocity;
    if (dir == DOWN)
        cameraPos_ = cameraPos_ + cameraUp_ * velocity;

    viewMatrix_ = glm::lookAt(cameraPos_, cameraPos_+cameraFront_, worldUp_);
}

void CameraNode::processMouse(double pos_x, double pos_y) {
    double xoffset = pos_x;
    double yoffset = pos_y;

    xoffset *= mouseSens_;
    yoffset *= mouseSens_;

    yaw_   += xoffset;
    pitch_ -= yoffset;

    if (pitch_ > 89.0f)
        pitch_ = 89.0f;
    if (pitch_ < -89.0f)
        pitch_ = -89.0f;

    cameraFront_.x = cos(DEG2RAD * yaw_) * cos(DEG2RAD * pitch_);
    cameraFront_.y = sin(DEG2RAD * pitch_);
    cameraFront_.z = sin(DEG2RAD * yaw_) * cos(DEG2RAD * pitch_);
    cameraFront_ = glm::normalize(cameraFront_);
    // also re-calculate the Right and Up vector
    cameraRight_ = glm::normalize(glm::cross(cameraFront_, worldUp_));
    cameraUp_ = glm::normalize(glm::cross(cameraFront_, cameraRight_));

    viewMatrix_ = glm::lookAt(cameraPos_, cameraPos_+cameraFront_, worldUp_);
}

void CameraNode::printSubGraph(std::ostream& out) const {
    out<<"cam: ";
    Node::printSubGraph(out);
}