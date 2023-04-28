#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include <string>

#include "node.hpp"

class CameraNode : public Node {
public:
    CameraNode(std::string const& name, glm::mat4 const& projectionMatrix);

    bool getPerspective() const;
    bool getEnabled() const;
    void setEnabled(bool enabled);

    glm::mat4 getProjectionMatrix() const;
    void setProjectionMatrix(glm::mat4 const& projectionMatrix);
private:
    bool isPerspective_ = true;
    bool isEnabled_ = false;
    glm::mat4 projectionMatrix_;
};

#endif