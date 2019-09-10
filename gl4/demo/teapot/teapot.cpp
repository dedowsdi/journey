#include "app.h"

#include <sstream>

#include <glm/gtc/type_ptr.hpp>
#include "bitmap_text.h"
#include "geometry.h"
#include "teapot.h"
#include "program.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

geometry_base teapot;

vec4 tess_level_out = vec4(16);
vec2 tess_level_inner = vec2(16);

glm::mat4 v_mat;
glm::mat4 p_mat;

class program_name : public program
{
public:

  GLint ul_mvp_mat;
  GLint ul_color;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/dummy.vs.glsl");
    attach(GL_TESS_EVALUATION_SHADER, "shader4/teapot.te.glsl");
    attach(GL_FRAGMENT_SHADER, {"#version 430 core"}, "shader4/lightless.fs.glsl");
  }

  void bind_uniform_locations()
  {
    ul_mvp_mat = get_uniform_location("mvp_mat");
    ul_color = get_uniform_location("color");
  }

  void bind_attrib_locations()
  {
  }
} prg;

class teapot_app : public app
{
protected:

public:
  virtual void init_info();
  virtual void create_scene();

  virtual void update();

  virtual void display();

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h);

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods);
};

void teapot_app::init_info()
{
  app::init_info();
  m_info.title = "teapot_app";
  m_info.wnd_width = WIDTH;
  m_info.wnd_height = HEIGHT;
}

void teapot_app::create_scene()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  auto vertices = std::make_unique<vec3_array>();
  auto vertex_ptr = reinterpret_cast<vec3*>(const_cast<float(*)[3]>(TeapotVertices)) ;
  vertices->assign(vertex_ptr, vertex_ptr + NumTeapotVertices);
  teapot.set_attrib_array(0, std::move(vertices));

  auto elements = std::make_unique<uint_array>();
  auto index_ptr = reinterpret_cast<GLuint*>(const_cast<int(*)[4][4]>(TeapotIndices)) ;
  elements->assign(index_ptr, index_ptr + NumTeapotIndices);
  teapot.set_element_array(std::move(elements));

  teapot.add_primitive_set(std::make_shared<draw_elements>(
    GL_PATCHES, NumTeapotVertices, GL_UNSIGNED_INT, 0));

  prg.init();
  p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
  v_mat = glm::lookAt(vec3(0, -20, 0), vec3(0), pza);
  set_v_mat(&v_mat);
}

void teapot_app::update() {}

void teapot_app::display()
{
  glClear(GL_COLOR_BUFFER_BIT);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  prg.use();
  glPatchParameteri(GL_PATCH_VERTICES, NumTeapotVerticesPerPatch);
  glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, glm::value_ptr(tess_level_out));
  glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, glm::value_ptr(tess_level_inner));

  mat4 mvp_mat = p_mat * v_mat * glm::rotate(fpi2, pxa);
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
  glUniform4f(prg.ul_color, 1, 1, 1, 1);

  teapot.draw();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "";
  m_text.print(ss.str(), 10, wnd_height()- 20);
  glDisable(GL_BLEND);
}

void teapot_app::glfw_resize(GLFWwindow *wnd, int w, int h)
{
  app::glfw_resize(wnd, w, h);
}

void teapot_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (key)
    {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

}

int main(int argc, char *argv[])
{
  zxd::teapot_app app;
  app.run();
}
