#include "app.h"
#include "bitmaptext.h"
#include "quad.h"
#include "program.h"
#include "texutil.h"
#include <sstream>
#include <boost/filesystem.hpp>
#include <thread>
#include <chrono>
#include <exception>

#define WIDTH 800
#define HEIGHT 800

namespace bfs = boost::filesystem;

void thread_function()
{
  std::cout <<"test thread function" << std::endl;
}


namespace zxd {

kcip a;
kcip b;
kcip n;

struct fs_program : public program{

  GLint ul_resolution;
  GLint ul_a;
  GLint ul_b;
  GLint ul_n;

  fs_program() {}

  void attach_shaders(){
    attach(GL_VERTEX_SHADER, "shader/plot.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/superellipse.fs.glsl");
  }

  void update_uniforms(GLuint tex_index = 0){

  }

  virtual void bind_uniform_locations(){
    uniform_location(&ul_resolution, "resolution");
    uniform_location(&ul_a, "a");
    uniform_location(&ul_b, "b");
    uniform_location(&ul_n, "n");
  }

  virtual void bind_attrib_locations(){
  }
} prg;

class superellipse_app : public app {
protected:
  bitmap_text m_text;
  GLfloat m_time;
  GLboolean m_pause;
  GLboolean m_update_shader;
  GLuint m_vao;
  vec2 m_mouse;

public:

  virtual void init_info() {
    app::init_info();
    m_info.title = "superellipse_app";
    m_info.wnd_width = 800;
    m_info.wnd_height = 800;
    m_info.samples = 4;
  }
  virtual void create_scene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    glGenVertexArrays(1, &m_vao);

    prg.init();

    m_pause = false;
    m_time = 0;
    m_update_shader = GL_FALSE;

    glfwSetWindowPos(m_wnd, 1024, 100);

    a = m_control.add_control<GLfloat>(GLFW_KEY_Q, 1, -10000, 10000, 0.1);
    b = m_control.add_control<GLfloat>(GLFW_KEY_W, 1, -10000, 10000, 0.1);
    n = m_control.add_control<GLfloat>(GLFW_KEY_E, 2, -10000, 10000, 0.1);
  }

  virtual void update() {

    if(m_update_shader)
    {
      prg.reload();
      m_update_shader = GL_FALSE;
    }

    if(m_pause)
      return;

    m_time += m_delta_time;
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniform2f(prg.ul_resolution, wnd_width(), wnd_height());
    glUniform1f(prg.ul_a, a->get_float());
    glUniform1f(prg.ul_b, b->get_float());
    glUniform1f(prg.ul_n, n->get_float());

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : a : " << a->get_float() << std::endl;
    ss << "wW : b : " << b->get_float() << std::endl;
    ss << "eE : n : " << n->get_float() << std::endl;
    m_text.print(ss.str(), 10, HEIGHT - 20, vec4(1, 0, 0, 1));
    glDisable(GL_BLEND);

  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  GLboolean update_shader() const { return m_update_shader; }
  void update_shader(GLboolean v){ m_update_shader = v; }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;

        case GLFW_KEY_P:
          this->m_pause = !this->m_pause;
          break;

        default:
          break;
      }
    }

    app::glfw_key(wnd, key, scancode, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y) 
  {
    m_mouse.x = x;
    m_mouse.y = glfw_to_gl(y);;
  }
};

}


int main(int argc, char *argv[]) {
  zxd::superellipse_app app;
  app.run();
  return 0;
}
