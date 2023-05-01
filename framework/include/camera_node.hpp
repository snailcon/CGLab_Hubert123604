#ifndef CAMERA_NODE_HPP
#define CAMERA_NODE_HPP

#include <string>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "node.hpp"

#define PI 3.14159265359
#define DEG2RAD (PI / 180.0)

enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

class CameraNode : public Node {
public:
    CameraNode(std::string const& name, glm::mat4 const& projectionMatrix);

    bool getPerspective() const;
    bool getEnabled() const;
    void setEnabled(bool enabled);

    glm::mat4 getProjectionMatrix() const;
    glm::mat4 getViewMatrix() const;

    void setProjectionMatrix(glm::mat4 const& projectionMatrix);
    
    void processKeyboard(Camera_Movement dir, float delta);
    void processMouse(double pos_x, double pos_y);

    void printSubGraph(std::ostream& out) const;
private:
    bool isPerspective_ = true;
    bool isEnabled_ = false;
    glm::mat4 projectionMatrix_;

    float yaw_ =  -90.0f;
    float pitch_ = 0.0f;

    float movespeed_ = 10.0f;
    float mouseSens_ = 0.2f;

    glm::vec3 cameraPos_ = { 0.0f, 0.0f, 4.0f };
    glm::vec3 cameraFront_ = { 0.0f, 0.0f, -1.0f };

    glm::vec3 worldUp_ = { 0.0f, 1.0f, 0.0f }; 
    glm::vec3 cameraRight_ = glm::normalize(glm::cross(worldUp_, cameraFront_));
    glm::vec3 cameraUp_ = glm::normalize(glm::cross(cameraFront_, cameraRight_));
    glm::mat4 viewMatrix_ = glm::lookAt(cameraPos_, cameraPos_+cameraFront_, worldUp_);
};

#endif