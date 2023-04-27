#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <string>

#include "node.hpp"

class SceneGraph {
public:
    /*
    Singleton Pattern Implementation from
    https://stackoverflow.com/questions/43523509/simple-singleton-example-in-c
    */
    static SceneGraph& getInstance();

    std::string getName() const;
    void setName(std::string const& name);
    Node getRoot() const;
    void setRoot(Node const& node);

    void printGraph();
private:
    SceneGraph() {};

    std::string name_;
    Node root_;
};

#endif