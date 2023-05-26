#ifndef POINTLIGHT_NODE_HPP
#define POINTLIGHT_NODE_HPP

#include <string>

#include "node.hpp"

// PointlightNode hold the light information
class PointlightNode : public Node {
public:
    PointlightNode(std::string const& name, glm::vec3 const& color, float intensity);

    void setIntensity(float intensity);
    float getIntensity() const;

    void setColor(glm::vec3 const& color);
    glm::vec3 getColor() const;

    void printSubGraph(std::ostream& out) const;
private:
    float intensity_;
    glm::vec3 color_;
};

#endif