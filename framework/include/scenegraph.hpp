#ifndef SCENEGRAPH_HPP
#define SCENEGRAPH_HPP

#include <vector>
#include <string>
#include <iostream>
#include <memory>

#include "node.hpp"
#include "geometry_node.hpp"
#include "pointlight_node.hpp"
#include "camera_node.hpp"

class SceneGraph {
public:
    /*
    Singleton Pattern Implementation from
    https://stackoverflow.com/questions/43523509/simple-singleton-example-in-c
    */
    static SceneGraph& getInstance();

    std::string getName() const;
    void setName(std::string const& name);
    Node* getRoot() const;
    void setRoot(std::shared_ptr<Node> node);

    // helper-method to only get nodes with geometry for rendering
    std::vector<GeometryNode*> getGeomNodes() const;
    // helper-method to only get pointlight nodes
    std::vector<PointlightNode*> getPointlightNodes() const;

    void printGraph();
private:
    SceneGraph() {};

    std::string name_;
    std::shared_ptr<Node> root_;
};

#endif