#include "geometry_node.hpp"

GeometryNode::GeometryNode(std::string const& name, model_object const& geometry) : Node(name), model_(geometry){

}

model_object GeometryNode::getGeometry() const {
    return model_;
}

void GeometryNode::setGeometry(model_object const& geometry) {
    model_ = geometry;
}

void GeometryNode::setShader(shader_program* shader) {
    shader_ = shader;
}

void GeometryNode::setUniformMat4(std::string const& name, glm::mat4 const& mat) {
    uniforms_.at(name) = mat;
}

void GeometryNode::uploadUniforms() const {
    glUseProgram(shader_->handle);

    for (auto const& uniform : uniforms_) {
        glUniformMatrix4fv(shader_->u_locs.at(uniform.first),
                           1, GL_FALSE, glm::value_ptr(uniform.second));
    }
}

void GeometryNode::render() const {
    glBindVertexArray(model_.vertex_AO);
    if (model_.is_indexed) {
        glDrawElements(model_.draw_mode, model_.num_elements, model::INDEX.type, NULL);
    } else {
        glDrawArrays(model_.draw_mode, 0, model_.num_elements);
    }
}

void GeometryNode::printSubGraph(std::ostream& out) const {
    out<<"geom: ";
    Node::printSubGraph(out);
}