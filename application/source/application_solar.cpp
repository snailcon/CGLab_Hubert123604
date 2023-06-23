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
  initializeStars(3000, 350.0f, 50.0f);
  initializeGeometry();
  initializeShaderPrograms();
  initializeSkyboxObject();
  initializeFramebuffer();
  reloadShaders(false);
  initializeSolarScenegraph();
}

ApplicationSolar::~ApplicationSolar() {
  glDeleteBuffers(1, &planet_object.vertex_BO);
  glDeleteBuffers(1, &planet_object.element_BO);
  glDeleteVertexArrays(1, &planet_object.vertex_AO);
}

void ApplicationSolar::loadTexture(unsigned int texture, std::string path) {
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  pixel_data tex = texture_loader::file(m_resource_path + "textures/" + path);
  if (tex.channels == GL_RED) {
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED);
  }
  glTexImage2D(GL_TEXTURE_2D, 0, tex.channels, tex.width, tex.height, 0, tex.channels, tex.channel_type, tex.pixels.data());
  glGenerateMipmap(GL_TEXTURE_2D);
}

void ApplicationSolar::loadCubemap(unsigned int cubemap, std::string path) {
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  int i = 0;
  std::vector<std::string> sides = {"right.png", "left.png", "bottom.png", "top.png", "front.png", "back.png"};
  for (std::string side : sides) {
    pixel_data tex = texture_loader::file(m_resource_path + "textures/" + path + "/" + side);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, tex.channels, tex.width, tex.height, 0, tex.channels, tex.channel_type, tex.pixels.data());
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    ++i;
  }
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
  Node* merc_geom = merc_hold->getChild("merc. geom");
  Node* venu_geom = venu_hold->getChild("venu. geom");
  Node* eart_geom = eart_hold->getChild("eart. geom");
  Node* mars_geom = mars_hold->getChild("mars. geom");
  Node* jupi_geom = jupi_hold->getChild("jupi. geom");
  Node* satu_geom = satu_hold->getChild("satu. geom");
  Node* uran_geom = uran_hold->getChild("uran. geom");
  Node* nept_geom = nept_hold->getChild("nept. geom");
  Node* moon_geom = moon_hold->getChild("moon. geom");

  // update the rotations
  merc_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 5.0f));
  venu_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 10.0f));
  eart_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 5.0f));
  mars_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 20.0f));
  jupi_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 25.0f));
  satu_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 30.0f));
  uran_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 35.0f));
  nept_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 40.0f));
  moon_hold->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 25.0f));

  merc_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  venu_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  eart_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  mars_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  jupi_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  satu_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  uran_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  nept_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  moon_geom->rotate(glm::vec3(0.0f, 1.0f, 0.0f), glm::radians(timescale * deltaTime * 50.0f));
  // -----------------------------------------------------------------------------------------------------------------------------
}

void ApplicationSolar::render() const {
  // bind shader to upload uniforms
  uploadUniforms(m_shaders.at("planet"));
  uploadUniforms(m_shaders.at("stars"), false);
  uploadUniforms(m_shaders.at("orbit"));
  uploadUniforms(m_shaders.at("skybox"), false);

  // get all light nodes
  std::vector<PointlightNode*> light_nodes = scenegraph.getPointlightNodes();
  glm::mat4 l_trans = light_nodes.at(0)->getWorldTransform();
  glm::vec3 light_pos = glm::vec3(l_trans[3][0], l_trans[3][1], l_trans[3][2]);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  // get all renderable nodes
  std::vector<GeometryNode*> geom_nodes = scenegraph.getGeomNodes();
  for (GeometryNode* geom : geom_nodes) {
    // extra matrix for normal transformation to keep them orthogonal to surface
    glm::fmat4 normal_matrix = glm::inverseTranspose(geom->getWorldTransform());
    geom->setUniformMat4("ModelMatrix", geom->getWorldTransform());
    geom->setUniformMat4("NormalMatrix", normal_matrix);
    geom->setUniformVec3("light_pos", light_pos);
    geom->setUniformVec3("light_col", light_nodes.at(0)->getColor());
    geom->setUniformFloat("light_intensity", light_nodes.at(0)->getIntensity());

    geom->setUniformBool("is_toon", is_toon);
    geom->setUniformFloat("toon_steps", toon_steps);

    geom->setUniformFloat("normal_strength", normal_strength); 

    geom->uploadUniforms();

    geom->bindTextures();

    geom->render();
  }

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glDisable(GL_DEPTH_TEST);

  glUseProgram(m_shaders.at("screen").handle);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex_handle);

  glBindVertexArray(screen_object.vertex_AO);
  glDrawArrays(screen_object.draw_mode, 0, screen_object.num_elements);
}

void ApplicationSolar::uploadView(shader_program const& prog, bool do_translate) const {
  // upload matrix to gpu
  if (do_translate)
    glUniformMatrix4fv(prog.u_locs.at("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(m_view_transform));
  else
    glUniformMatrix4fv(prog.u_locs.at("ViewMatrix"), 1, GL_FALSE, glm::value_ptr(glm::mat4(glm::mat3(m_view_transform))));
}

void ApplicationSolar::uploadProjection(shader_program const& prog) const {
  // upload matrix to gpu
  glUniformMatrix4fv(prog.u_locs.at("ProjectionMatrix"),
                     1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms(shader_program const& prog, bool do_translate) const { 
  // bind shader to which to upload unforms
  glUseProgram(prog.handle);
  // upload uniform values to new locations
  uploadView(prog, do_translate);
  uploadProjection(prog);
}

///////////////////////////// intialisation functions /////////////////////////

void ApplicationSolar::initializeFramebuffer() {
  glGenRenderbuffers(1, &rb_handle);
  glBindRenderbuffer(GL_RENDERBUFFER, rb_handle);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH32F_STENCIL8, initial_resolution.x, initial_resolution.y);

  glGenTextures(1, &tex_handle);
  glBindTexture(GL_TEXTURE_2D, tex_handle);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, initial_resolution.x, initial_resolution.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(GL_TEXTURE_2D, 0);

  glGenFramebuffers(1, &fbo_handle);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo_handle);
  glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex_handle, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rb_handle);

  // glDrawBuffer(GL_COLOR_ATTACHMENT0);
  // glDrawBuffer(GL_DEPTH_ATTACHMENT);

  glBindRenderbuffer(GL_RENDERBUFFER, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  float points[] = {
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,

     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
  };

  // generate vertex array object
  glGenVertexArrays(1, &screen_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(screen_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &screen_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, screen_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 5 * 6, points, GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute (position)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute (color)
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(3*sizeof(float)));

  glBindVertexArray(0);

  // store type of primitive to draw
  screen_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  screen_object.num_elements = GLsizei(6);
}

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
  m_shaders.at("planet").u_locs["color"] = -1;
  m_shaders.at("planet").u_locs["light_pos"] = -1;
  m_shaders.at("planet").u_locs["light_col"] = -1;
  m_shaders.at("planet").u_locs["light_intensity"] = -1;
  m_shaders.at("planet").u_locs["is_toon"] = -1;
  m_shaders.at("planet").u_locs["toon_steps"] = -1;
  m_shaders.at("planet").u_locs["diffuse_texture"] = -1;
  m_shaders.at("planet").u_locs["normal_texture"] = -1;
  m_shaders.at("planet").u_locs["normal_strength"] = -1;

  // star shader
  m_shaders.emplace("stars", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/vao.vert"},
                                          {GL_FRAGMENT_SHADER,  m_resource_path + "shaders/vao.frag"}}});
  m_shaders.at("stars").u_locs["ViewMatrix"] = -1;
  m_shaders.at("stars").u_locs["ProjectionMatrix"] = -1;

  // star shader
  m_shaders.emplace("orbit", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/orbit.vert"},
                                          {GL_FRAGMENT_SHADER,  m_resource_path + "shaders/vao.frag"}}});
  m_shaders.at("orbit").u_locs["ModelMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ViewMatrix"] = -1;
  m_shaders.at("orbit").u_locs["ProjectionMatrix"] = -1;

  // skybox shader
  m_shaders.emplace("skybox", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/skybox.vert"},
                                          {GL_FRAGMENT_SHADER,  m_resource_path + "shaders/skybox.frag"}}});
  m_shaders.at("skybox").u_locs["ViewMatrix"] = -1;
  m_shaders.at("skybox").u_locs["ProjectionMatrix"] = -1;

  // screen blit shader
  m_shaders.emplace("screen", shader_program{{{GL_VERTEX_SHADER, m_resource_path + "shaders/screen.vert"},
                                          {GL_FRAGMENT_SHADER,  m_resource_path + "shaders/screen.frag"}}});
  m_shaders.at("screen").u_locs["screen_tex"] = -1;

}

// set up star positions and color
void ApplicationSolar::initializeStars(int amount, float radius, float variance) {
  // uniform distribution on a sphere: http://corysimon.github.io/articles/uniformdistn-on-sphere/
  // set up random generation
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_real_distribution<float> randomfloat(0.0f, 1.0f);

  // 6 floats per star (XYZ|RGB)
  std::vector<float> stars = std::vector<float>(amount*6);
  for (int i = 0; i < amount; ++i) {
    float theta = 2.0f * PI * randomfloat(mt);
    float phi = acosf(1.0f - 2.0f * randomfloat(mt));
    float var_radius = radius - variance * (1.0f - 2.0f * randomfloat(mt));
    float x = sinf(phi) * sinf(theta) * var_radius;
    float y = cosf(phi) * var_radius;
    float z = sinf(phi) * cosf(theta) * var_radius;
    float r = randomfloat(mt);
    float g = randomfloat(mt);
    float b = randomfloat(mt);
    stars.at(i * 6) = x;
    stars.at(i * 6 + 1) = y;
    stars.at(i * 6 + 2) = z;
    stars.at(i * 6 + 3) = r;
    stars.at(i * 6 + 4) = g;
    stars.at(i * 6 + 5) = b;
  }

  // generate vertex array object
  glGenVertexArrays(1, &stars_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(stars_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &stars_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, stars_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * amount * 6, stars.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute (position)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, 0);
  // activate second attribute on gpu
  glEnableVertexAttribArray(1);
  // second attribute (color)
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(3*sizeof(float)));

  glBindVertexArray(0);

  // store type of primitive to draw
  stars_object.draw_mode = GL_POINTS;
  // transfer number of indices to model object 
  stars_object.num_elements = GLsizei(amount);
}

// load models
void ApplicationSolar::initializeGeometry() {
  model planet_model = model_loader::obj(m_resource_path + "models/sphere.obj", model::NORMAL | model::TEXCOORD);

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
    // activate second attribute on gpu
  glEnableVertexAttribArray(2);
  // second attribute is 3 floats with no offset & stride
  glVertexAttribPointer(2, model::TEXCOORD.components, model::TEXCOORD.type, GL_FALSE, planet_model.vertex_bytes, planet_model.offsets[model::TEXCOORD]);

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
  planet_object.is_indexed = true;
}

void ApplicationSolar::initializeSkyboxObject() {
  float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
  };

  // generate vertex array object
  glGenVertexArrays(1, &skybox_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(skybox_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &skybox_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, skybox_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 6 * 3, skyboxVertices, GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute (position)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindVertexArray(0);

  // store type of primitive to draw
  skybox_object.draw_mode = GL_TRIANGLES;
  // transfer number of indices to model object 
  skybox_object.num_elements = GLsizei(6 * 6);
}

void ApplicationSolar::initializeSolarScenegraph() {

  // prepare orbit object
  // ------------------------------------------------------------------------------------------------------------------------------------
  int orbit_resolution = 180;
  std::vector<float> orbit = std::vector<float>(orbit_resolution * 3);

  for (int i = 0; i < orbit_resolution; ++i) {
    float arc = (i / (float)orbit_resolution) * 2.0f * PI;
    orbit.at(i * 3) = sinf(arc);
    orbit.at(i * 3 + 1) = 0.0f;
    orbit.at(i * 3 + 2) = cosf(arc);
  }

  // generate vertex array object
  glGenVertexArrays(1, &orbit_object.vertex_AO);
  // bind the array for attaching buffers
  glBindVertexArray(orbit_object.vertex_AO);

  // generate generic buffer
  glGenBuffers(1, &orbit_object.vertex_BO);
  // bind this as an vertex array buffer containing all attributes
  glBindBuffer(GL_ARRAY_BUFFER, orbit_object.vertex_BO);
  // configure currently bound array buffer
  glBufferData(GL_ARRAY_BUFFER, sizeof(float) * orbit_resolution * 3, orbit.data(), GL_STATIC_DRAW);

  // activate first attribute on gpu
  glEnableVertexAttribArray(0);
  // first attribute (position)
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

  glBindVertexArray(0);

  // store type of primitive to draw
  orbit_object.draw_mode = GL_LINE_LOOP;
  // transfer number of indices to model object 
  orbit_object.num_elements = GLsizei(orbit_resolution);
  // ------------------------------------------------------------------------------------------------------------------------------------

  // set up all nodes for the scenegraph
  // ------------------------------------------------------------------------------------------------------------------------------------
  std::shared_ptr<Node> root = std::make_shared<Node>("Root");
  std::shared_ptr<CameraNode> camera = std::make_shared<CameraNode>("Camera", utils::calculate_projection_matrix(initial_aspect_ratio));

  std::shared_ptr<Node> sun_hold = std::make_shared<Node>("sun. hold");
  std::shared_ptr<GeometryNode> sun_geom = std::make_shared<GeometryNode>("sun. geom", planet_object, &(m_shaders.at("planet")));
  std::shared_ptr<PointlightNode> sun_light = std::make_shared<PointlightNode>("sun. ligth", glm::vec3(1.0f), 1.0f);

  std::shared_ptr<Node> merc_hold = std::make_shared<Node>("merc. hold");
  std::shared_ptr<Node> venu_hold = std::make_shared<Node>("venu. hold");
  std::shared_ptr<Node> eart_hold = std::make_shared<Node>("eart. hold");
  std::shared_ptr<Node> mars_hold = std::make_shared<Node>("mars. hold");
  std::shared_ptr<Node> jupi_hold = std::make_shared<Node>("jupi. hold");
  std::shared_ptr<Node> satu_hold = std::make_shared<Node>("satu. hold");
  std::shared_ptr<Node> uran_hold = std::make_shared<Node>("uran. hold");
  std::shared_ptr<Node> nept_hold = std::make_shared<Node>("nept. hold");

  std::shared_ptr<GeometryNode> merc_orbi = std::make_shared<GeometryNode>("merc. orbi", orbit_object, &(m_shaders.at("orbit")));
  std::shared_ptr<GeometryNode> venu_orbi = std::make_shared<GeometryNode>("venu. orbi", orbit_object, &(m_shaders.at("orbit")));
  std::shared_ptr<GeometryNode> eart_orbi = std::make_shared<GeometryNode>("eart. orbi", orbit_object, &(m_shaders.at("orbit")));
  std::shared_ptr<GeometryNode> mars_orbi = std::make_shared<GeometryNode>("mars. orbi", orbit_object, &(m_shaders.at("orbit")));
  std::shared_ptr<GeometryNode> jupi_orbi = std::make_shared<GeometryNode>("jupi. orbi", orbit_object, &(m_shaders.at("orbit")));
  std::shared_ptr<GeometryNode> satu_orbi = std::make_shared<GeometryNode>("satu. orbi", orbit_object, &(m_shaders.at("orbit")));
  std::shared_ptr<GeometryNode> uran_orbi = std::make_shared<GeometryNode>("uran. orbi", orbit_object, &(m_shaders.at("orbit")));
  std::shared_ptr<GeometryNode> nept_orbi = std::make_shared<GeometryNode>("nept. orbi", orbit_object, &(m_shaders.at("orbit")));

  // use the planet_object to use as the model for every geometry node
  std::shared_ptr<GeometryNode> merc_geom = std::make_shared<GeometryNode>("merc. geom", planet_object, &(m_shaders.at("planet")));
  std::shared_ptr<GeometryNode> venu_geom = std::make_shared<GeometryNode>("venu. geom", planet_object, &(m_shaders.at("planet")));
  std::shared_ptr<GeometryNode> eart_geom = std::make_shared<GeometryNode>("eart. geom", planet_object, &(m_shaders.at("planet")));
  std::shared_ptr<GeometryNode> mars_geom = std::make_shared<GeometryNode>("mars. geom", planet_object, &(m_shaders.at("planet")));
  std::shared_ptr<GeometryNode> jupi_geom = std::make_shared<GeometryNode>("jupi. geom", planet_object, &(m_shaders.at("planet")));
  std::shared_ptr<GeometryNode> satu_geom = std::make_shared<GeometryNode>("satu. geom", planet_object, &(m_shaders.at("planet")));
  std::shared_ptr<GeometryNode> uran_geom = std::make_shared<GeometryNode>("uran. geom", planet_object, &(m_shaders.at("planet")));
  std::shared_ptr<GeometryNode> nept_geom = std::make_shared<GeometryNode>("nept. geom", planet_object, &(m_shaders.at("planet")));

  std::shared_ptr<Node> moon_hold = std::make_shared<Node>("moon. hold");
  std::shared_ptr<GeometryNode> moon_orbi = std::make_shared<GeometryNode>("moon. orbi", orbit_object, &(m_shaders.at("orbit")));
  std::shared_ptr<GeometryNode> moon_geom = std::make_shared<GeometryNode>("moon. geom", planet_object, &(m_shaders.at("planet")));

  std::shared_ptr<GeometryNode> star_geom = std::make_shared<GeometryNode>("star. geom", stars_object, &(m_shaders.at("stars")));

  std::shared_ptr<GeometryNode> skybox = std::make_shared<GeometryNode>("skyb. geom", skybox_object, &(m_shaders.at("skybox")));
  // ------------------------------------------------------------------------------------------------------------------------------------

  // set up the hierachy
  // ------------------------------------------------------------------------------------------------------------------------------------
  root->addChild(camera);
  root->addChild(star_geom);
  root->addChild(skybox);
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
  sun_hold->addChild(sun_light);
  merc_hold->addChild(merc_orbi);
  venu_hold->addChild(venu_orbi);
  eart_hold->addChild(eart_orbi);
  mars_hold->addChild(mars_orbi);
  jupi_hold->addChild(jupi_orbi);
  satu_hold->addChild(satu_orbi);
  uran_hold->addChild(uran_orbi);
  nept_hold->addChild(nept_orbi);
  merc_hold->addChild(merc_geom);
  venu_hold->addChild(venu_geom);
  eart_hold->addChild(eart_geom);
  mars_hold->addChild(mars_geom);
  jupi_hold->addChild(jupi_geom);
  satu_hold->addChild(satu_geom);
  uran_hold->addChild(uran_geom);
  nept_hold->addChild(nept_geom);

  eart_hold->addChild(moon_hold);
  moon_hold->addChild(moon_orbi);
  moon_hold->addChild(moon_geom);
  // ------------------------------------------------------------------------------------------------------------------------------------

  // set up initial scaling of the models
  // ------------------------------------------------------------------------------------------------------------------------------------
  merc_geom->setScale(glm::vec3(0.1f));
  venu_geom->setScale(glm::vec3(0.2f));
  eart_geom->setScale(glm::vec3(0.5f));
  mars_geom->setScale(glm::vec3(0.8f));
  jupi_geom->setScale(glm::vec3(1.2f));
  satu_geom->setScale(glm::vec3(1.6f));
  uran_geom->setScale(glm::vec3(2.2f));
  nept_geom->setScale(glm::vec3(3.0f));

  moon_geom->setScale(glm::vec3(0.1f));
  // ------------------------------------------------------------------------------------------------------------------------------------

  // set up initial rotations
  // ------------------------------------------------------------------------------------------------------------------------------------
  merc_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(0.0f));
  venu_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(30.0f));
  eart_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(60.0f));
  mars_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(90.0f));
  jupi_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(120.0f));
  satu_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(150.0f));
  uran_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(180.0f));
  nept_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(210.0f));
  moon_hold->setRotation(glm::vec3(1.0f, 0.0f, 0.0f), glm::radians(50.0f));
  // ------------------------------------------------------------------------------------------------------------------------------------

  // set up initial positions
  // ------------------------------------------------------------------------------------------------------------------------------------
  merc_geom->setTranslation(glm::vec3(1.5f, 0.0f, 0.0f));
  venu_geom->setTranslation(glm::vec3(2.5f, 0.0f, 0.0f));
  eart_geom->setTranslation(glm::vec3(4.0f, 0.0f, 0.0f));
  mars_geom->setTranslation(glm::vec3(6.0f, 0.0f, 0.0f));
  jupi_geom->setTranslation(glm::vec3(9.0f, 0.0f, 0.0f));
  satu_geom->setTranslation(glm::vec3(14.0f, 0.0f, 0.0f));
  uran_geom->setTranslation(glm::vec3(30.0f, 0.0f, 0.0f));
  nept_geom->setTranslation(glm::vec3(50.0f, 0.0f, 0.0f));

  moon_hold->setTranslation(glm::vec3(4.0f, 0.0f, 0.0f));
  moon_geom->setTranslation(glm::vec3(1.0f, 0.0f, 0.0f));
  // ------------------------------------------------------------------------------------------------------------------------------------

  // set up orbits
  // ------------------------------------------------------------------------------------------------------------------------------------
  merc_orbi->setScale(glm::vec3(1.5f));
  venu_orbi->setScale(glm::vec3(2.5f));
  eart_orbi->setScale(glm::vec3(4.0f));
  mars_orbi->setScale(glm::vec3(6.0f));
  jupi_orbi->setScale(glm::vec3(9.0f));
  satu_orbi->setScale(glm::vec3(14.0f));
  uran_orbi->setScale(glm::vec3(30.0f));
  nept_orbi->setScale(glm::vec3(50.0f));

  moon_orbi->setScale(glm::vec3(1.0f));
  // ------------------------------------------------------------------------------------------------------------------------------------

  // setup colors
  // -------------------------------------------------
  sun_geom->setUniformVec3("color", glm::vec3(1.0f, 1.0f, 0.0f));
  merc_geom->setUniformVec3("color", glm::vec3(0.7f, 0.8f, 0.0f));
  venu_geom->setUniformVec3("color", glm::vec3(0.9f, 0.6f, 0.0f));
  eart_geom->setUniformVec3("color", glm::vec3(0.2f, 0.4f, 1.0f));
  mars_geom->setUniformVec3("color", glm::vec3(1.0f, 0.3f, 0.0f));
  jupi_geom->setUniformVec3("color", glm::vec3(0.5f, 0.4f, 0.0f));
  satu_geom->setUniformVec3("color", glm::vec3(0.7f, 0.5f, 0.0f));
  uran_geom->setUniformVec3("color", glm::vec3(0.0f, 0.3f, 0.6f));
  nept_geom->setUniformVec3("color", glm::vec3(0.0f, 0.1f, 1.0f));
  moon_geom->setUniformVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
  // -------------------------------------------------

  // load and set textures
  // -------------------------------------------------
  unsigned int sun_texture;
  unsigned int merc_texture;
  unsigned int venu_texture;
  unsigned int eart_texture;
  unsigned int mars_texture;
  unsigned int jupi_texture;
  unsigned int satu_texture;
  unsigned int uran_texture;
  unsigned int nept_texture;
  unsigned int moon_texture;
  glGenTextures(1, &sun_texture);  
  glGenTextures(1, &merc_texture);  
  glGenTextures(1, &venu_texture);  
  glGenTextures(1, &eart_texture);  
  glGenTextures(1, &mars_texture);  
  glGenTextures(1, &jupi_texture);  
  glGenTextures(1, &satu_texture);  
  glGenTextures(1, &uran_texture);  
  glGenTextures(1, &nept_texture);  
  glGenTextures(1, &moon_texture);  
  loadTexture(sun_texture, "sunmap.jpg");
  loadTexture(merc_texture, "mercurymap.jpg");
  loadTexture(venu_texture, "venusmap.jpg");
  loadTexture(eart_texture, "earthmap1k.jpg");
  loadTexture(mars_texture, "mars_1k_color.jpg");
  loadTexture(jupi_texture, "jupitermap.jpg");
  loadTexture(satu_texture, "saturnmap.jpg");
  loadTexture(uran_texture, "uranusmap.jpg");
  loadTexture(nept_texture, "neptunemap.jpg");
  loadTexture(moon_texture, "moonmap1k.jpg");
  sun_geom->setTexture("diffuse_texture", sun_texture);
  merc_geom->setTexture("diffuse_texture", merc_texture);
  venu_geom->setTexture("diffuse_texture", venu_texture);
  eart_geom->setTexture("diffuse_texture", eart_texture);
  mars_geom->setTexture("diffuse_texture", mars_texture);
  jupi_geom->setTexture("diffuse_texture", jupi_texture);
  satu_geom->setTexture("diffuse_texture", satu_texture);
  uran_geom->setTexture("diffuse_texture", uran_texture);
  nept_geom->setTexture("diffuse_texture", nept_texture);
  moon_geom->setTexture("diffuse_texture", moon_texture);
  unsigned int merc_norm_texture;
  unsigned int venu_norm_texture;
  unsigned int eart_norm_texture;
  unsigned int mars_norm_texture;
  unsigned int moon_norm_texture;
  glGenTextures(1, &merc_norm_texture);
  glGenTextures(1, &venu_norm_texture);
  glGenTextures(1, &eart_norm_texture);
  glGenTextures(1, &mars_norm_texture);
  glGenTextures(1, &moon_norm_texture);
  loadTexture(merc_norm_texture, "mercurynormal.jpg");
  loadTexture(venu_norm_texture, "venusnormal.jpg");
  loadTexture(eart_norm_texture, "earthnormal.jpg");
  loadTexture(mars_norm_texture, "mars_1k_normal.jpg");
  loadTexture(moon_norm_texture, "moonnormal.jpg");
  merc_geom->setTexture("normal_texture", merc_norm_texture);
  venu_geom->setTexture("normal_texture", venu_norm_texture);
  eart_geom->setTexture("normal_texture", eart_norm_texture);
  mars_geom->setTexture("normal_texture", mars_norm_texture);
  moon_geom->setTexture("normal_texture", moon_norm_texture);
  sun_geom->setTextureUniforms();
  merc_geom->setTextureUniforms();
  venu_geom->setTextureUniforms();
  eart_geom->setTextureUniforms();
  mars_geom->setTextureUniforms();
  jupi_geom->setTextureUniforms();
  satu_geom->setTextureUniforms();
  uran_geom->setTextureUniforms();
  nept_geom->setTextureUniforms();
  moon_geom->setTextureUniforms();
  // -------------------------------------------------

  // load skybox
  // -------------------------------------------------
  unsigned int skybox_tex;
  glGenTextures(1, &skybox_tex);
  loadCubemap(skybox_tex, "skybox_galaxy");
  skybox->setTexture("cubemap", skybox_tex, true);
  skybox->setTextureUniforms();
  // -------------------------------------------------

  // set the root
  scenegraph.setRoot(root);

  // debug: print some interpretation of the scenegraph
  scenegraph.printGraph();
}

///////////////////////////// callback functions for window events ////////////
// handle key input
void ApplicationSolar::keyCallback(int key, int action, int mods) {
  if (key == GLFW_KEY_O  && (action == GLFW_PRESS)) {
    draw_orbits = !draw_orbits;
  }
  if (key == GLFW_KEY_T  && (action == GLFW_PRESS)) {
    is_toon = !is_toon;
  }

  if (key == GLFW_KEY_KP_ADD  && (action == GLFW_PRESS) && is_toon) {
    toon_steps += 1.0f;
  }
  if (key == GLFW_KEY_KP_SUBTRACT  && (action == GLFW_PRESS) && is_toon && toon_steps > 1.1f) {
    toon_steps -= 1.0f;
  }
  if (key == GLFW_KEY_KP_6  && (action == GLFW_PRESS)) {
    normal_strength += 0.5f;
  }
  if (key == GLFW_KEY_KP_5  && (action == GLFW_PRESS)) {
    normal_strength -= 0.5f;
  }
  if (key == GLFW_KEY_KP_3  && (action == GLFW_PRESS)) {
    timescale += 0.2f;
  }
  if (key == GLFW_KEY_KP_2  && (action == GLFW_PRESS)) {
    timescale -= 0.2f;
  }
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
}

//handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
  // get a pointer to the camera (if multiple: this should get the enabled camera)
  CameraNode* cam = ((CameraNode*)(scenegraph.getRoot()->getChild("Camera")));
  // pos_x, pos_y are offsets, since the mouse is reset to 0,0 every frame
  cam->processMouse(pos_x, pos_y);

  // upload the updated view matrix
  m_view_transform = cam->getViewMatrix();
}

//handle resizing
void ApplicationSolar::resizeCallback(unsigned width, unsigned height) {
  // recalculate projection matrix for new aspect ration
  // get a pointer to the camera (if multiple: this should get the enabled camera)
  CameraNode* cam = ((CameraNode*)(scenegraph.getRoot()->getChild("Camera")));
  // use given utils to calculate the correct projection matrix
  cam->setProjectionMatrix(utils::calculate_projection_matrix(float(width) / float(height)));
  m_view_projection = cam->getProjectionMatrix();
}


// exe entry point
int main(int argc, char* argv[]) {
  Application::run<ApplicationSolar>(argc, argv, 3, 2);
}