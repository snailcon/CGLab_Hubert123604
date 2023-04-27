#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <vector>
#include <glm/mat4x4.hpp>

class Node {
public:
    Node();
    Node(std::string const& name);

    Node getParent() const;
    void setParent(Node const& node);

    Node getChild(std::string const& name) const;
    std::vector<Node&> getChildren() const;

    std::string getName() const;
    std::string getPath() const;
    int getDepth() const;

    glm::mat4 getLocalTransform() const;
    void setLocalTransform(glm::mat4 const& localtransForm);

    glm::mat4 getWorldTransform() const;
    void setWorldTransform(glm::mat4 const& localtransForm);

    void addChild(Node const& node);
    void removeChild(std::string const& name);
private:
    Node* parent_ = nullptr;
    std::vector<Node&> children_;
    std::string name_;
    std::string path_;
    int depth;
    glm::mat4 localTransform_;
    glm::mat4 worldTransform_;
};

#endif