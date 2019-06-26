#include "app.h"

#include <sstream>

#include "bitmap_text.h"
#include "geometry.h"
#include "string_util.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

glm::mat4 v_mat;
glm::mat4 p_mat;

geometry_base rect;
geometry_base triangle;
geometry_base isoline;
geometry_base* geometry;

geometry_base* geometries[] = {&rect, &triangle, &isoline};
int patch_vertices[] = {4, 3, 4};

vec4 tess_level_outer = vec4(3);
vec2 tess_level_inner = vec2(3);

kcip kci_outer0;
kcip kci_outer1;
kcip kci_outer2;
kcip kci_outer3;

kcip kci_inner0;
kcip kci_inner1;
kcip kci_primitive_type;
kcip kci_winding;
kcip kci_spacing;
kcip kci_point_mode;

const char* primitive_types[] = {"quads", "triangles", "isolines"};
const char* face_windings[] = {"ccw", "cw"};
const char* spacings[] = {"equal_spacing", "fractional_even_spacing", "fractional_odd_spacing"};
const char* te_shaders[] = {"shader4/tessellate_quad.te.glsl",
  "shader4/tessellate_triangle.te.glsl",
  "shader4/tessellate_isoline.te.glsl"};

int primitive_type;
int face_winding;
int spacing;
bool point_mode;

class tessellate_program : public program
{

public:
  GLint ul_color;
  GLint ul_mvp_mat;

  std::string layout;

protected:

  void attach_shaders()
  {
    std::string full_layout = "#define LAYOUT " + layout + "\n";
    attach(GL_VERTEX_SHADER, "shader4/dummy.vs.glsl");
    //attach(GL_TESS_CONTROL_SHADER, "shader4/tessellate_quad.tc.glsl");
    attach(GL_TESS_EVALUATION_SHADER, {"#version 430 core\n", full_layout}, te_shaders[primitive_type]);
    attach(GL_FRAGMENT_SHADER, {"#version 430 core\n"}, "shader4/lightless.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_color, "color");
    uniform_location(&ul_mvp_mat, "mvp_mat");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
  }

}prg ;

void reload_program(const kci* = 0)
{
  primitive_type = kci_primitive_type->get_int();
  face_winding = kci_winding->get_int();
  spacing = kci_spacing->get_int();
  point_mode = kci_point_mode->get_int() == 1;

  std::stringstream ss;
  ss << primitive_types[primitive_type] << ", " << face_windings[face_winding]
     << ", " << spacings[spacing];
  if(point_mode)
    ss << ", point_mode";

  prg.layout = ss.str();
  prg.reload();

  geometry = geometries[primitive_type];

}


class tessellate_app : public app
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

void tessellate_app::init_info()
{
  app::init_info();
  m_info.title = "tessellate_app";
  m_info.wnd_width = WIDTH;
  m_info.wnd_height = HEIGHT;
  m_info.samples = 4;
}

void tessellate_app::create_scene()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


  v_mat = glm::lookAt(vec3(0, -5, 5), vec3(0), pza);
  p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
  set_v_mat(&v_mat);

  {
    auto vertices = std::make_shared<vec2_array>();
    rect.attrib_array(0, vertices);
    vec2 lb = vec2(0);
    vertices->push_back(lb + vec2(-1, -1));
    vertices->push_back(lb + vec2(1, -1));
    vertices->push_back(lb + vec2(1, 1));
    vertices->push_back(lb + vec2(-1, 1));
    rect.bind_and_update_buffer();

    rect.add_primitive_set(new draw_arrays(GL_PATCHES, 0, 4));
  }

  {
    auto vertices = std::make_shared<vec2_array>();
    triangle.attrib_array(0, vertices);
    vec2 lb = vec2(0);
    vertices->push_back(lb + vec2(-1, -1));
    vertices->push_back(lb + vec2(1, -1));
    vertices->push_back(lb + vec2(0, 1));
    triangle.bind_and_update_buffer();

    triangle.add_primitive_set(new draw_arrays(GL_PATCHES, 0, 3));
  }

  {
    auto vertices = std::make_shared<vec2_array>();
    isoline.attrib_array(0, vertices);
    vec2 lb = vec2(0);
    vertices->push_back(lb + vec2(-2, -1));
    vertices->push_back(lb + vec2(2, -1));
    vertices->push_back(lb + vec2(-2, 0));
    vertices->push_back(lb + vec2(2, 0));
    vertices->push_back(lb + vec2(-2, 1));
    vertices->push_back(lb + vec2(2, 1));
    isoline.bind_and_update_buffer();

    isoline.add_primitive_set(new draw_arrays(GL_PATCHES, 0, 4));
  }

  kci_outer0 = m_control.add_control<GLfloat>(GLFW_KEY_Q, tess_level_outer[0], 1, 100, 0.5);
  kci_outer1 = m_control.add_control<GLfloat>(GLFW_KEY_W, tess_level_outer[1], 1, 100, 0.5);
  kci_outer2 = m_control.add_control<GLfloat>(GLFW_KEY_E, tess_level_outer[2], 1, 100, 0.5);
  kci_outer3 = m_control.add_control<GLfloat>(GLFW_KEY_R, tess_level_outer[3], 1, 100, 0.5);

  kci_inner0 = m_control.add_control<GLfloat>(GLFW_KEY_U, tess_level_inner[0], 1, 100, 0.5);
  kci_inner1 = m_control.add_control<GLfloat>(GLFW_KEY_I, tess_level_inner[1], 1, 100, 0.5);

  kci_primitive_type = m_control.add_control(GLFW_KEY_A, 0, 0, 2, 1, reload_program);
  kci_winding = m_control.add_control(GLFW_KEY_S, 0, 0, 1, 1, reload_program);
  kci_spacing = m_control.add_control(GLFW_KEY_D, 0, 0, 2, 1, reload_program);
  kci_point_mode = m_control.add_control(GLFW_KEY_F, 0, 0, 1, 1, reload_program);

  reload_program();
}


void tessellate_app::update()
{
  tess_level_outer = vec4(kci_outer0->get_float(), kci_outer1->get_float(), kci_outer2->get_float(), kci_outer3->get_float());
  tess_level_inner = vec2(kci_inner0->get_float(), kci_inner1->get_float());

  glPatchParameterfv(GL_PATCH_DEFAULT_OUTER_LEVEL, glm::value_ptr(tess_level_outer));
  glPatchParameterfv(GL_PATCH_DEFAULT_INNER_LEVEL, glm::value_ptr(tess_level_inner));
}

void tessellate_app::display()
{
  glClear(GL_COLOR_BUFFER_BIT);

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  prg.use();
  mat4 mvp_mat = p_mat * v_mat;
  glUniform4f(prg.ul_color, 1, 1, 1, 1);
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));

  glPatchParameteri(GL_PATCH_VERTICES, patch_vertices[primitive_type]);
  geometry->draw();

  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "qwer : tess level outer : " << tess_level_outer << std::endl;;
  ss << "ui : tess level inner : " << tess_level_inner << std::endl;;
  ss << "asdf : layout : " << prg.layout << std::endl;
  m_text.print(ss.str(), 10, wnd_height()- 20);
  glDisable(GL_BLEND);
}

void tessellate_app::glfw_resize(GLFWwindow *wnd, int w, int h)
{
  app::glfw_resize(wnd, w, h);
}

void tessellate_app::glfw_key(
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
  zxd::tessellate_app app;
  app.run();
}
