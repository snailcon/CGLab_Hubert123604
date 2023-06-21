#include "geometry_node.hpp"

#include "utils.hpp"

GeometryNode::GeometryNode(std::string const& name, model_object const& geometry, shader_program* shader) 
    : Node(name), model_(geometry), shader_(shader){

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
    if (shader_->u_locs.count(name) == 1) {
        uniforms_[name] = std::make_pair(UNIFORM_TYPE::MAT4, std::make_shared<glm::mat4>(glm::mat4(mat)));
    } else {
        // debug output :/
        // std::cout<<"ShaderProgram at handle "<<shader_->handle<<" does not have a Uniform named: "<<name<<std::endl;
    }
}

void GeometryNode::setUniformVec3(std::string const& name, glm::vec3 const& vec) {
    if (shader_->u_locs.count(name) == 1) {
        uniforms_[name] = std::make_pair(UNIFORM_TYPE::VEC3, std::make_shared<glm::vec3>(glm::vec3(vec)));
    } else {
        // debug output :/
        // std::cout<<"ShaderProgram at handle "<<shader_->handle<<" does not have a Uniform named: "<<name<<std::endl;
    }
}

void GeometryNode::setUniformFloat(std::string const& name, float val) {
    if (shader_->u_locs.count(name) == 1) {
        uniforms_[name] = std::make_pair(UNIFORM_TYPE::FLOAT, std::make_shared<float>(val));
    } else {
        // debug output :/
        // std::cout<<"ShaderProgram at handle "<<shader_->handle<<" does not have a Uniform named: "<<name<<std::endl;
    }
}

void GeometryNode::setUniformInt(std::string const& name, int val) {
    if (shader_->u_locs.count(name) == 1) {
        uniforms_[name] = std::make_pair(UNIFORM_TYPE::INT, std::make_shared<int>(val));
    } else {
        // debug output :/
        // std::cout<<"ShaderProgram at handle "<<shader_->handle<<" does not have a Uniform named: "<<name<<std::endl;
    }
}

void GeometryNode::setUniformBool(std::string const& name, bool val) {
    if (shader_->u_locs.count(name) == 1) {
        // just use Type Int as expected by GPU
        uniforms_[name] = std::make_pair(UNIFORM_TYPE::INT, std::make_shared<int>(val));
    } else {
        // debug output :/
        // std::cout<<"ShaderProgram at handle "<<shader_->handle<<" does not have a Uniform named: "<<name<<std::endl;
    }
}

void GeometryNode::uploadUniforms() const {
    glUseProgram(shader_->handle);

    for (auto const& uniform : uniforms_) {
        switch(uniform.second.first) {
            case UNIFORM_TYPE::MAT4: 
                glUniformMatrix4fv(shader_->u_locs.at(uniform.first), 1, GL_FALSE, (GLfloat*)(uniform.second.second).get());
                break;
            case UNIFORM_TYPE::VEC3: 
                glUniform3fv(shader_->u_locs.at(uniform.first), 1, (GLfloat*)(uniform.second.second).get());
                break;
            case UNIFORM_TYPE::FLOAT:
                glUniform1fv(shader_->u_locs.at(uniform.first), 1, (GLfloat*)(uniform.second.second).get());
                break;
            case UNIFORM_TYPE::INT:
                glUniform1iv(shader_->u_locs.at(uniform.first), 1, (GLint*)(uniform.second.second).get());
                break;
        }
    }
}

void GeometryNode::render() const {
    glUseProgram(shader_->handle);
    glBindVertexArray(model_.vertex_AO);
    if (model_.is_indexed) {
        glDrawElements(model_.draw_mode, model_.num_elements, model::INDEX.type, NULL);
    } else {
        glDrawArrays(model_.draw_mode, 0, model_.num_elements);
    }
}

void GeometryNode::setTexture(std::string const& name, unsigned int texture, bool is_cubemap) {
    textures_[name] = std::make_pair(texture, is_cubemap);
}

void GeometryNode::setTextureUniforms() {
    int i = 0;
    for (auto & texture : textures_) {
        setUniformInt(texture.first, i);
        i++;
    }
}

void GeometryNode::bindTextures() {
    glUseProgram(shader_->handle);
    int i = 0;
    for (auto & texture : textures_) {
        glActiveTexture(GL_TEXTURE0 + i);
        if (!texture.second.second) glBindTexture(GL_TEXTURE_2D, texture.second.first);
        else glBindTexture(GL_TEXTURE_CUBE_MAP, texture.second.first);
        ++i;
    }
    for (; i < 16; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
}

void GeometryNode::printSubGraph(std::ostream& out) const {
    out<<"geom: ";
    Node::printSubGraph(out);
}