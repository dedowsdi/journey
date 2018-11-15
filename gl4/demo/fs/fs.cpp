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

namespace bfs = boost::filesystem;

void thread_function()
{
  std::cout <<"test thread function" << std::endl;
}


namespace zxd {

std::string fragment_shader;

struct fs_program : public program{

  GLint ul_resolution;
  GLint ul_time;
  GLint ul_mouse;

  fs_program() {}

  void attach_shaders(){
    attach(GL_VERTEX_SHADER, "data/shader/plot.vs.glsl");
    attach(GL_FRAGMENT_SHADER, fragment_shader);
  }

  void update_uniforms(GLuint tex_index = 0){
  }


  virtual void bind_uniform_locations(){
    uniform_location(&ul_resolution, "resolution");
    uniform_location(&ul_time, "time");
    uniform_location(&ul_mouse, "mouse");
  }

  virtual void bind_attrib_locations(){
  }
} prg;

class fs : public app {
protected:
  bitmap_text m_text;
  GLfloat m_time;
  GLboolean m_pause;
  GLboolean m_update_shader;
  GLboolean m_display_uniform;
  GLuint m_vao;
  vec2 m_mouse;

public:

  virtual void init_info() {
    app::init_info();
    m_info.title = "fs";
    m_info.samples = 16;
  }
  virtual void create_scene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    glGenVertexArrays(1, &m_vao);

    prg.init();

    m_pause = false;
    m_time = 0;
    m_update_shader = GL_FALSE;

    glfwSetWindowPos(m_wnd, 1024, 100);

    m_display_uniform = GL_TRUE;
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
    glUniform2f(prg.ul_resolution, m_info.wnd_width, m_info.wnd_height);
    glUniform1f(prg.ul_time, m_time);
    glUniform2f(prg.ul_mouse, m_mouse.x, m_mouse.y);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if(!m_display_uniform)
      return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "resolution : " << m_info.wnd_width << " " << m_info.wnd_height << std::endl;
    ss << "mouse : " << m_mouse.x << " " << m_mouse.y << std::endl;
    ss << "time : " << m_time << std::endl;
    ss << "samples : " << m_info.samples << std::endl;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, 492, vec4(1, 0, 0, 1));
    glDisable(GL_BLEND);

  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
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

        case GLFW_KEY_U:
          m_display_uniform ^= 1;
          break;

        default:
          break;
      }
    }
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y) 
  {
    m_mouse.x = x;
    m_mouse.y = glfw_to_gl(y);;
  }
};

class ShaderObserver
{
private:
  fs* m_app;
  std::time_t m_last_w_time;

public:

  ShaderObserver(fs* app):
    m_app(app)
  {
    m_last_w_time = bfs::last_write_time(fragment_shader);
  }

  void operator() ()
  {
    std::chrono::milliseconds interval(50);
    while(true & !m_app->is_shutdown())
    {
      std::this_thread::sleep_for(interval);
      try {
        // throws if file is being written?
        auto t = bfs::last_write_time(fragment_shader);
        if(t != m_last_w_time)
        {
          m_last_w_time = t;
          std::cout << "shader updated to " << std::ctime(&m_last_w_time) << std::endl;
          m_app->update_shader(GL_TRUE);
        }
      }catch(std::exception& e) {
        std::cout << e.what() << std::endl;
      }
    }
  }

};
}


int main(int argc, char *argv[]) {

  if(argc < 2)
  {
    std::cout << "miss fragment file" << std::endl;
  }

  zxd::fragment_shader = argv[1];

  zxd::fs app;

  std::thread observer_thread((zxd::ShaderObserver(&app)));

  app.run();
  observer_thread.join();
}
