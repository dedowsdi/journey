#include <sstream>
#include <algorithm>

#include <app.h>
#include <common_program.h>
#include <filter.h>
#include <sphere.h>
#include <pingpong.h>
#include <quad.h>

namespace zxd {

const GLint width = 800;
const GLint height = 800;
const GLint num_models = 100;
const GLfloat scene_radius = 30;
const vec2 near_far = vec2(0.1f, 1000.0f);
const vec2 field = vec2(5.0f, 10.0f); // start distance, range

mat4 p_mat;
mat4 v_mat;

gaussian_blur_filter blur_filter;
blinn_program light_prg;

light_model lm;
std::vector<zxd::light_source> lights;
material mtl;

std::vector<mat4> models;
sphere geom;

pingpong blur_map;
GLuint depth_map;
GLuint color_map;
GLuint fbo;

class depthinfield_program : public program
{
public:

  GLint ul_color_map = 0;
  GLint ul_depth_map = 1;
  GLint ul_blur_map  = 2;
  GLint ul_near_far  = 3;
  GLint ul_field     = 4;

  void update_uniforms(GLint color, GLint depth, GLint blur,
    const vec2& near_far, const vec2& field)
  {
    glUniform1i(ul_color_map, color);
    glUniform1i(ul_depth_map, depth);
    glUniform1i(ul_blur_map, blur);
    glUniform2f(ul_near_far, near_far.x, near_far.y);
    glUniform2f(ul_field, field.x, field.y);
  }

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "depthoffield_data/depthoffield.fs.glsl");
  }


} dof_prg;

class depth_of_field_app : public app {
private:

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) override;
  void glfw_mouse_button(GLFWwindow *wnd, int button, int action,
    int mods) override;

  void glfw_mouse_move(GLFWwindow *wnd, double x, double y) override;
};

void depth_of_field_app::init_info() {
  app::init_info();
  m_info.title = "depth_of_field_app";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

GLuint create_texture()
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return tex;
}

void depth_of_field_app::create_scene()
{
  // prg.init();
  models.reserve(num_models);
  std::generate_n(std::back_inserter(models), num_models, []() -> mat4 {
    auto pos = glm::ballRand(scene_radius);
    return glm::translate(pos);
  });

  light_source light;
  light.position = vec4(1, -1, 1, 0.0f);
  light.ambient = vec4(0.2);
  light.diffuse = vec4(0.8);
  light.specular = vec4(0.5);
  lights.push_back(light);
  
  mtl.shininess = 50;
  mtl.specular = vec4(0.8, 0.8, 0.8, 1.0f);

  lm.local_viewer = true;

  light_prg.init();
  light_prg.bind_lighting_uniform_locations(lights, lm, mtl);

  dof_prg.init();

  geom.slice(64);
  geom.stack(64);
  geom.build_mesh({attrib_semantic::vertex, attrib_semantic::normal});

  p_mat = glm::perspective(fpi4, wnd_aspect(), near_far.x, near_far.y);
  v_mat = zxd::isometric_projection(60);
  set_v_mat(&v_mat);

  depth_map = create_texture();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
    GL_DEPTH_COMPONENT, GL_FLOAT, 0);

  color_map = create_texture();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, 0);

  for (unsigned i = 0; i < 2; ++i)
  {
    blur_map[i] = create_texture();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, 0);
  }

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void depth_of_field_app::update() {}

void depth_of_field_app::display() {
  // render to color rex and depth tex
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_map, 0);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("incomplete frame buffer\n");

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  light_prg.use();
  light_prg.update_lighting_uniforms(lights, lm, mtl, v_mat);
  for (const auto& m_mat : models)
  {
    light_prg.update_uniforms(m_mat, v_mat, p_mat);
    geom.draw();
  }

  glDisable(GL_DEPTH_TEST);
  // blur depth tex
  blur_filter.filter(color_map, blur_map, 3);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  dof_prg.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, color_map);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, depth_map);
  glActiveTexture(GL_TEXTURE2);
  glBindTexture(GL_TEXTURE_2D, blur_map.pong());

  dof_prg.update_uniforms(0, 1, 2, near_far, field);
  draw_quad();

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "";
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void depth_of_field_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
}

void depth_of_field_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void depth_of_field_app::glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                               int mods)
{
  app::glfw_mouse_button(wnd, button, action, mods);
}

void depth_of_field_app::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
}

}

int main(int argc, char *argv[]) {
  zxd::depth_of_field_app app;
  app.run();
}
