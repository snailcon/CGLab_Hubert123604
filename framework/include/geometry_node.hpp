#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include <string>

#include "node.hpp"
#include "structs.hpp"

class GeometryNode : public Node {
public:
    GeometryNode(std::string const& name, model_object const& geometry);

    model_object getGeometry() const;
    void setGeometry(model_object const& geometry);
    void printSubGraph(std::ostream& out) const;
private:
    model_object model_;
};

#endif