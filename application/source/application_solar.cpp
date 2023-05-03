#include "application_solar.hpp"
#include "window_handler.hpp"

#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"

#include <glbinding/gl/gl.h>
// use gl definitions from glbinding 
using namespace gl;

//dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

ApplicationSolar::ApplicationSolar(std::string const& resource_path)
 :Application{resource_path}
 ,planet_object{}
 ,m_view_transform{glm::translate(glm::fmat4{}, glm::fvec3{0.0f, 0.0f, 4.0f})}
 ,m_view_projection{utils::calculate_projection_matrix(initial_aspect_ratio)}
{
  initializeGeometry();
  initializeShaderPrograms();
  initializeSolarScenegraph();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::update(GLFWwindow* window) {
  // update the deltaTime for the current frame
  float currentFrame = glfwGetTime();
  deltaTime = currentFrame - lastFrame;
  lastFrame = currentFrame;

  // process keyboard inputs for movement
  processKeyInput(window);

  // update animation status of planets
  // -----------------------------------------------------------------------------------------------------------------------------
  Node* merc_hold = scenegraph.getRoot()->getChild("merc. hold");
  Node* venu_hold = scenegraph.getRoot()->getChild("venu. hold");
  Node* eart_hold = scenegraph.getRoot()->getChild("eart. hold");
  Node* mars_hold = scenegraph.getRoot()->getChild("mars. hold");
  Node* jupi_hold = scenegraph.getRoot()->getChild("jupi. hold");
  Node* satu_hold = scenegraph.getRoot()->getChild("satu. hold");
  Node* uran_hold = scenegraph.getRoot()->getChild("uran. hold");
  Node* nept_hold = scenegraph.getRoot()->getChild("nept. hold");

  Node* moon_hold = eart_hold->getChild("moon. hold");

  // set the rotation of every planet
  glm::mat4 merc_transf = glm::rotate(glm::mat4(1.0f), glm::radians(9.9f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 venu_transf = glm::rotate(glm::mat4(1.0f), glm::radians(8.1f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 eart_transf = glm::rotate(glm::mat4(1.0f), glm::radians(7.8f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 mars_transf = glm::rotate(glm::mat4(1.0f), glm::radians(5.7f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 jupi_transf = glm::rotate(glm::mat4(1.0f), glm::radians(4.0f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 satu_transf = glm::rotate(glm::mat4(1.0f), glm::radians(2.5f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 uran_transf = glm::rotate(glm::mat4(1.0f), glm::radians(1.5f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
  glm::mat4 nept_transf = glm::rotate(glm::mat4(1.0f), glm::radians(1.0f * (float)glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));

  // and moon
  glm::mat4 moon_transf = glm::rotate(glm::mat4(1.0f), glm::radians(50.0f * (float)glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));

  // offset them from the center (and based on the rotation)
  merc_transf = glm::translate(merc_transf, glm::vec3(1.5f, 0.0f, 0.0f));
  venu_transf = glm::translate(venu_transf, glm::vec3(2.5f, 0.0f, 0.0f));
  eart_transf = glm::translate(eart_transf, glm::vec3(4.0f, 0.0f, 0.0f));
  mars_transf = glm::translate(mars_transf, glm::vec3(6.0f, 0.0f, 0.0f));
  jupi_transf = glm::translate(jupi_transf, glm::vec3(9.0f, 0.0f, 0.0f));
  satu_transf = glm::translate(satu_transf, glm::vec3(14.0f, 0.0f, 0.0f));
  uran_transf = glm::translate(uran_transf, glm::vec3(30.0f, 0.0f, 0.0f));
  nept_transf = glm::translate(nept_transf, glm::vec3(50.0f, 0.0f, 0.0f));

  moon_transf = glm::translate(moon_transf, glm::vec3(1.0f, 0.0f, 0.0f));

  // set the transforms
  merc_hold->setLocalTransform(merc_transf);
  venu_hold->setLocalTransform(venu_transf);
  eart_hold->setLocalTransform(eart_transf);
  mars_hold->setLocalTransform(mars_transf);
  jupi_hold->setLocalTransform(jupi_transf);
  satu_hold->setLocalTransform(satu_transf);
  uran_hold->setLocalTransform(uran_transf);
  nept_hold->setLocalTransform(nept_transf);

  moon_hold->setLocalTransform(moon_transf);
  // -----------------------------------------------------------------------------------------------------------------------------
}

void ApplicationSolar::render() const {
  // bind shader to upload uniforms
  glUseProgram(m_shaders.at("planet").handle);

  // get all renderable nodes
  std::vector<GeometryNode*> geom_nodes = scenegraph.getGeomNodes();
  for (GeometryNode* geom : geom_nodes) {
    glm::fmat4 model_matrix = geom->getWorldTransform(); // use their model matrices for rendering
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ModelMatrix"),
                      1, GL_FALSE, glm::value_ptr(model_matrix));

    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(glm::inverse(m_view_transform) * model_matrix);
    glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("NormalMatrix"),
                      1, GL_FALSE, glm::value_ptr(normal_matrix));

    // the gpu representation is saved in the node, so it can be used here
    // bind the VAO to draw
    glBindVertexArray(geom->getGeometry().vertex_AO);

    // draw bound vertex array using bound shader
    glDrawElements(geom->getGeometry().draw_mode, geom->getGeometry().num_elements, model::INDEX.type, NULL);
  }
}

void ApplicationSolar::uploadView() {
  // vertices are transformed in camera space, so camera transform must be inverted
  // glm::fmat4 view_matrix = glm::inverse(m_view_transform);
  // glm::lookAt() is used for the view viewmatrix, it's already in the correct form
  glm::fmat4 view_matrix = m_view_transform;
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ViewMatrix"),
                     1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::uploadProjection() {
  // upload matrix to gpu
  glUniformMatrix4fv(m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() { 
  // bind shader to which to upload unforms
  glUseProgram(m_shaders.at("planet").handle);
  // upload uniform values to new locations
  uploadView();
  uploadProjection();
}

///////////////////////////// intialisation functions /////////////////////////
// load shader sources
void ApplicationSolar::initializeShaderPrograms() {
  // store shader program objects in container
  m_shaders.emplace("planet", shader_program{{{GL_VERTEX_SHADER,m_resource_path + "shaders/simple.vert"},
                                           {GL_FRAGMENT_SHADER, m_resource_path + "shaders/simple.frag"}}});
  // request uniform locations for shader program
  m_shaders.at("planet").u_locs["NormalMatrix"] = -1;
  m_shaders.at("planet").u_locs["ModelMatrix"] = -1;
  m_shaders.at("planet").u_locs["ViewMatrix"] = -1;
  m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL);

  // generate vertex array object
  glGenVertexArrays(1, &planet_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(planet_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &planet_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * planet_model.data.size(), planet_model.data.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute is 3 floats with no offset & stride
  glVertexAttribPointer(0, model::POSITION.components, model::POSITION.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::POSITION]);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(1, model::NORMAL.components, model::NORMAL.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::NORMAL]);

   // generate generic buffer
  glGenBuffers(1, &planet_object.element_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planet_object.element_BO);
  // configure currently bound array buffer
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, model::INDEX.size * planet_model.indices.size(), planet_model.indices.data(), GL_STATIC_DRAW);

  // store type of primitive to draw
  planet_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  planet_object.num_elements = GLsizei(planet_model.indices.size());
}

void ApplicationSolar::initializeSolarScenegraph() {

  // set up all nodes for the scenegraph
  // ------------------------------------------------------------------------------------------------------------------------------------
  std::shared_ptr<Node> root = std::make_shared<Node>("Root");
  std::shared_ptr<CameraNode> camera = std::make_shared<CameraNode>("Camera", utils::calculate_projection_matrix(initial_aspect_ratio));

  std::shared_ptr<Node> sun_hold = std::make_shared<Node>("sun. hold");
  std::shared_ptr<GeometryNode> sun_geom = std::make_shared<GeometryNode>("sun. geom", planet_object);

  std::shared_ptr<Node> merc_hold = std::make_shared<Node>("merc. hold");
  std::shared_ptr<Node> venu_hold = std::make_shared<Node>("venu. hold");
  std::shared_ptr<Node> eart_hold = std::make_shared<Node>("eart. hold");
  std::shared_ptr<Node> mars_hold = std::make_shared<Node>("mars. hold");
  std::shared_ptr<Node> jupi_hold = std::make_shared<Node>("jupi. hold");
  std::shared_ptr<Node> satu_hold = std::make_shared<Node>("satu. hold");
  std::shared_ptr<Node> uran_hold = std::make_shared<Node>("uran. hold");
  std::shared_ptr<Node> nept_hold = std::make_shared<Node>("nept. hold");

  // hijack the planet_object to use as the model for every geometry node
  std::shared_ptr<GeometryNode> merc_geom = std::make_shared<GeometryNode>("merc. geom", planet_object);
  std::shared_ptr<GeometryNode> venu_geom = std::make_shared<GeometryNode>("venu. geom", planet_object);
  std::shared_ptr<GeometryNode> eart_geom = std::make_shared<GeometryNode>("eart. geom", planet_object);
  std::shared_ptr<GeometryNode> mars_geom = std::make_shared<GeometryNode>("mars. geom", planet_object);
  std::shared_ptr<GeometryNode> jupi_geom = std::make_shared<GeometryNode>("jupi. geom", planet_object);
  std::shared_ptr<GeometryNode> satu_geom = std::make_shared<GeometryNode>("satu. geom", planet_object);
  std::shared_ptr<GeometryNode> uran_geom = std::make_shared<GeometryNode>("uran. geom", planet_object);
  std::shared_ptr<GeometryNode> nept_geom = std::make_shared<GeometryNode>("nept. geom", planet_object);

  std::shared_ptr<Node> moon_hold = std::make_shared<Node>("moon. hold");
  std::shared_ptr<GeometryNode> moon_geom = std::make_shared<GeometryNode>("moon. geom", planet_object);

  // ------------------------------------------------------------------------------------------------------------------------------------

  // set up the hierachy
  // ------------------------------------------------------------------------------------------------------------------------------------
  root->addChild(camera);
  root->addChild(sun_hold);
  root->addChild(merc_hold);
  root->addChild(venu_hold);
  root->addChild(eart_hold);
  root->addChild(mars_hold);
  root->addChild(jupi_hold);
  root->addChild(satu_hold);
  root->addChild(uran_hold);
  root->addChild(nept_hold);
  sun_hold->addChild(sun_geom);
  merc_hold->addChild(merc_geom);
  venu_hold->addChild(venu_geom);
  eart_hold->addChild(eart_geom);
  mars_hold->addChild(mars_geom);
  jupi_hold->addChild(jupi_geom);
  satu_hold->addChild(satu_geom);
  uran_hold->addChild(uran_geom);
  nept_hold->addChild(nept_geom);

  eart_hold->addChild(moon_hold);
  moon_hold->addChild(moon_geom);
  // ------------------------------------------------------------------------------------------------------------------------------------

  // set up initial scaling of the models
  // ------------------------------------------------------------------------------------------------------------------------------------
  merc_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
  venu_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.2f)));
  eart_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.5f)));
  mars_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.8f)));
  jupi_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(1.2f)));
  satu_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(1.6f)));
  uran_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(2.2f)));
  nept_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(3.0f)));

  moon_geom->setLocalTransform(glm::scale(glm::mat4(1.0f), glm::vec3(0.1f)));
  // ------------------------------------------------------------------------------------------------------------------------------------

  // set the root
  scenegraph.setRoot(root);

  // debug: print some interpretation of the scenegraph
  scenegraph.printGraph();
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
  // using this to keep track of keystates is probably better than hijacking the window and using glfwGetKey
  // but the other solution was easier
}

// handle per frame key input
void ApplicationSolar::processKeyInput(GLFWwindow* window) {
  // get a pointer to the camera (if multiple: this should get the enabled camera)
  CameraNode* cam = ((CameraNode*)(scenegraph.getRoot()->getChild("Camera")));
  
  // if given key is held down the camera processes it
  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    cam->processKeyboard(FORWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    cam->processKeyboard(BACKWARD, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    cam->processKeyboard(LEFT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    cam->processKeyboard(RIGHT, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    cam->processKeyboard(UP, deltaTime);
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    cam->processKeyboard(DOWN, deltaTime);
  
  // upload the updated view matrix
  m_view_transform = cam->getViewMatrix();
  uploadView();
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // get a pointer to the camera (if multiple: this should get the enabled camera)
  CameraNode* cam = ((CameraNode*)(scenegraph.getRoot()->getChild("Camera")));
  // pos_x, pos_y are offsets, since the mouse is reset to 0,0 every frame
  cam->processMouse(pos_x, pos_y);

  // upload the updated view matrix
  m_view_transform = cam->getViewMatrix();
  uploadView();
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  // get a pointer to the camera (if multiple: this should get the enabled camera)
  CameraNode* cam = ((CameraNode*)(scenegraph.getRoot()->getChild("Camera")));
  // use given utils to calculate the correct projection matrix
  cam->setProjectionMatrix(utils::calculate_projection_matrix(float(width) / float(height)));
  m_view_projection = cam->getProjectionMatrix();
  // upload new projection matrix
  uploadProjection();
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}