#ifndef NODE_HPP
#define NODE_HPP

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <memory>

#include <glm/mat4x4.hpp>

class Node {
public:
    Node();
    Node(std::string const& name);

    Node* getParent() const;

    Node* getChild(std::string const& name) const;
    std::vector<Node*> getChildren() const;
    void getChildren(std::vector<Node*> & nodes);

    std::string getName() const;
    std::string getPath() const;
    int getDepth() const;

    glm::mat4 getLocalTransform() const;
    void setLocalTransform(glm::mat4 const& localTransform);

    glm::mat4 getWorldTransform() const;
    
    void addChild(std::shared_ptr<Node> node);
    void removeChild(std::string const& name);

    virtual void printSubGraph(std::ostream& out) const;

protected:
    void setWorldTransform(glm::mat4 const& worldTransform);
    void setParent(Node* node);

private:
    Node* parent_ = nullptr;
    std::vector<std::shared_ptr<Node>> children_;
    std::string name_;
    std::string path_;
    int depth_ = 0;
    glm::mat4 localTransform_ = glm::mat4(1.0f);
    glm::mat4 worldTransform_ = glm::mat4(1.0f);
};

#endif