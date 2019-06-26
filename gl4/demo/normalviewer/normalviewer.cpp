#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include "torus.h"
#include "common_program.h"

namespace zxd
{

torus torus;
GLfloat normal_length = 0.1f;
vec4 normal_color(1.0);
vec4 wire_color(0.0, 0.0, 0.0, 1.0);

glm::mat4 v_mat;
glm::mat4 p_mat;

struct wire_program : public zxd::program
  {
  GLint ul_color;
  GLint ul_mvp_mat;

  virtual void update_uniforms(const mat4& mvp_mat)
  {
    glUniformMatrix4fv(
      ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  }
  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/simple.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/color.fs.glsl");
  }
  virtual void bind_uniform_locations()
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_color, "color");
  }

  virtual void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
  };
};

normal_viewer_program program;

wire_program wire_program;

class normal_viewer_app : public app
{
protected:

public:
  normal_viewer_app() {}

protected:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "normal viewer";
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    program.init();
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 50.0f);
    v_mat = glm::lookAt(vec3(0, 5, 5), vec3(0), vec3(0, 0, 1));

    wire_program.init();

    set_v_mat(&v_mat);

    torus.include_normal(true);
    torus.build_mesh();

  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    mat4 model(1.0);
    program.update_uniforms(model, v_mat, p_mat);
    glUniformMatrix4fv( program.ul_p_mat, 1, 0, value_ptr(p_mat));
    glUniform1f(program.ul_normal_length, normal_length);
    glUniform4fv(program.ul_color, 1, value_ptr(normal_color));
    torus.draw();

    glUseProgram(wire_program);

    wire_program.update_uniforms(p_mat * v_mat * model);
    glUniform4fv(wire_program.ul_color, 1, value_ptr(wire_color));
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    torus.draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : normal length : " << normal_length << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;

        case GLFW_KEY_Q:
          if (mods & GLFW_MOD_SHIFT)
          {
            normal_length -= 0.1f;
          } else
          {
            normal_length += 0.1f;
          }
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }

  virtual void glfw_mouse_button(
    GLFWwindow *wnd, int button, int action, int mods)
  {
    app::glfw_mouse_button(wnd, button, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y)
  {
    app::glfw_mouse_move(wnd, x, y);
  }

  virtual void glfw_mouse_wheel(GLFWwindow *wnd, double xoffset, double yoffset)
  {
    app::glfw_mouse_wheel(wnd, xoffset, yoffset);
  }
  virtual void glfw_char(GLFWwindow *wnd, unsigned int codepoint)
  {
    app::glfw_char(wnd, codepoint);
  }
  virtual void glfw_charmod(GLFWwindow *wnd, unsigned int codepoint, int mods)
  {
    app::glfw_charmod(wnd, codepoint, mods);
  }
};
}

int main(int argc, char *argv[])
{
  zxd::normal_viewer_app app;
  app.run();
}
