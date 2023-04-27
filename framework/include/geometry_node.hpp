#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include <string>

#include "node.hpp"
#include "model.hpp"

class GeometryNode : public Node {
public:
    GeometryNode(std::string const& name, model const& geometry);

    model getGeometry() const;
    void setGeometry(model const& geometry);
private:
    model model_;
};

#endif