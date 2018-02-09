#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "common.h"
#include "torus.h"

namespace zxd {

torus torus;
GLfloat normal_length = 0.1f;
vec4 normal_color(1.0);
vec4 wire_color(0.0, 0.0, 0.0, 1.0);

struct normal_viewer_program : public zxd::program {
  GLint al_vertex;
  GLint al_normal;

  GLint ul_normal_length;
  GLint ul_color;

  virtual void update_model(const mat4 &_m_mat) {
    m_mat = _m_mat;
    mv_mat = v_mat * m_mat;
    mv_mat_it =
      glm::inverse(glm::transpose(mv_mat));

    glUniformMatrix4fv(ul_mv_mat_it, 1, 0,
      value_ptr(mv_mat_it));
    glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
  }
  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/normal_viewer.vs.glsl");
    attach(GL_GEOMETRY_SHADER, "data/shader/normal_viewer.gs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/color.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_p_mat, "p_mat");
    uniform_location(&ul_mv_mat, "mv_mat");
    uniform_location(
      &ul_mv_mat_it,
      "mv_mat_it");

    uniform_location(&ul_normal_length, "normal_length");
    uniform_location(&ul_color, "color");
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_normal = attrib_location("normal");
  };
};

struct wire_program : public zxd::program {
  GLint al_vertex;
  GLint ul_color;

  virtual void update_model(const mat4 &_m_mat) {
    m_mat = _m_mat;
    mvp_mat = p_mat * v_mat * m_mat;

    glUniformMatrix4fv(
      ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  }
  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/simple.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/color.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_color, "color");
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
  };
};

normal_viewer_program program;

wire_program wire_program;

class normal_viewer_app : public app {
protected:
  bitmap_text m_text;

public:
  normal_viewer_app() {}

protected:
  virtual void init_info() {
    app::init_info();
    m_info.title = "normal viewer";
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    program.init();
    program.p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 50.0f);
    program.v_mat = glm::lookAt(vec3(0, 5, 5), vec3(0), vec3(0, 0, 1));

    wire_program.init();
    wire_program.p_mat = program.p_mat;

    set_v_mat(&program.v_mat);

    torus.build_mesh();
    torus.bind(program.al_vertex, program.al_normal);

    torus.bind(wire_program.al_vertex);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    mat4 model(1.0);
    program.update_model(model);
    glUniformMatrix4fv(
      program.ul_p_mat, 1, 0, value_ptr(program.p_mat));
    glUniform1f(program.ul_normal_length, normal_length);
    glUniform4fv(program.ul_color, 1, value_ptr(normal_color));
    torus.draw();

    glUseProgram(wire_program);

    wire_program.v_mat = program.v_mat;
    wire_program.update_model(model);
    glUniform4fv(wire_program.ul_color, 1, value_ptr(wire_color));
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    torus.draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : normal length : " << normal_length << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(w, h);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;

        case GLFW_KEY_Q:
          if (mods & GLFW_MOD_SHIFT) {
            normal_length -= 0.1f;
          } else {
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
    GLFWwindow *wnd, int button, int action, int mods) {
    app::glfw_mouse_button(wnd, button, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y) {
    app::glfw_mouse_move(wnd, x, y);
  }

  virtual void glfw_mouse_wheel(GLFWwindow *wnd, double xoffset, double yoffset) {
    app::glfw_mouse_wheel(wnd, xoffset, yoffset);
  }
  virtual void glfw_char(GLFWwindow *wnd, unsigned int codepoint) {
    app::glfw_char(wnd, codepoint);
  }
  virtual void glfw_charmod(GLFWwindow *wnd, unsigned int codepoint, int mods) {
    app::glfw_charmod(wnd, codepoint, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::normal_viewer_app app;
  app.run();
}
