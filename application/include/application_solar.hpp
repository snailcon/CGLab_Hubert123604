#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include <vector>
#include <random>

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"
#include "scenegraph.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
 public:
  // allocate and initialize objects
  ApplicationSolar(std::string const& resource_path);
  // free allocated objects
  ~ApplicationSolar();

  // react to key input
  void keyCallback(int key, int action, int mods);
  // process key input per frame instead of on callback
  void processKeyInput(GLFWwindow* window);
  //handle delta mouse movement input
  void mouseCallback(double pos_x, double pos_y);
  //handle resizing
  void resizeCallback(unsigned width, unsigned height);

  // update all objects / deltaTime
  void update(GLFWwindow* window);
  // draw all objects
  void render() const;

 protected:
  void initializeShaderPrograms();
  void initializeStars(int amount, float radius, float variance);
  void initializeGeometry();
  // update uniform values
  void uploadUniforms(shader_program const& prog) const;
  // upload projection matrix
  void uploadProjection(shader_program const& prog) const;
  // upload view matrix
  void uploadView(shader_program const& prog) const;

  void initializeSolarScenegraph();

  // cpu representation of model
  model_object planet_object;
  
  // stars model holder
  model_object stars_object;

  // orbit model holder
  std::vector<glm::mat4> orbit_model_mats;
  model_object orbit_object;

  // camera transform matrix
  glm::fmat4 m_view_transform;
  // camera projection matrix
  glm::fmat4 m_view_projection;

  // instance of the scenegraph
  SceneGraph scenegraph = SceneGraph::getInstance();

  float deltaTime = 0.0f;
  // required for deltaTime
  float lastFrame = 0.0f;

  bool draw_orbits = true;
};

#endif