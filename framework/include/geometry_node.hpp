#ifndef GEOMETRY_NODE_HPP
#define GEOMETRY_NODE_HPP

#include <string>
#include <map>
#include <utility>
#include <memory>

#include <glm/gtc/type_ptr.hpp>

#include "model.hpp"
#include "node.hpp"
#include "structs.hpp"

enum UNIFORM_TYPE {MAT4, VEC3, FLOAT, INT};

// GeometryNode hold the gpu representation of a model
class GeometryNode : public Node {
public:
    GeometryNode(std::string const& name, model_object const& geometry, shader_program* shader);

    model_object getGeometry() const;
    void setGeometry(model_object const& geometry);

    void setShader(shader_program* shader);
    void setUniformMat4(std::string const& name, glm::mat4 const& mat);
    void setUniformVec3(std::string const& name, glm::vec3 const& vec);
    void setUniformFloat(std::string const& name, float val);
    void setUniformInt(std::string const& name, int val);
    void setUniformBool(std::string const& name, bool val);
    void uploadUniforms() const;

    void setTexture(std::string const& name, unsigned int texture, bool is_cubemap = false);
    void setTextureUniforms();
    void bindTextures();

    void render() const;

    void printSubGraph(std::ostream& out) const;
private:
    model_object model_;
    shader_program* shader_;
    std::map<std::string, std::pair<unsigned int, bool>> textures_;
    std::map<std::string, std::pair<UNIFORM_TYPE, std::shared_ptr<void>>> uniforms_;
};

#endif