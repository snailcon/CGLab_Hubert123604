#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include <string>
#include <map>

#include <glm/gtc/type_ptr.hpp>

#include "model.hpp"
#include "node.hpp"
#include "structs.hpp"

// GeometryNode hold the gpu representation of a model
class GeometryNode : public Node {
public:
    GeometryNode(std::string const& name, model_object const& geometry);

    model_object getGeometry() const;
    void setGeometry(model_object const& geometry);

    void setShader(shader_program* shader);
    void setUniformMat4(std::string const& name, glm::mat4 const& mat);
    void uploadUniforms() const;

    void render() const;

    void printSubGraph(std::ostream& out) const;
private:
    model_object model_;
    shader_program* shader_;
    std::map<std::string, glm::mat4> uniforms_;
};

#endif