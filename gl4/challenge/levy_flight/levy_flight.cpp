#include "app.h"
#include "bitmaptext.h"
#include "debugger.h"
#include "common_program.h"

#include <sstream>

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

vec3 v0;
vec3 v1;
mat4 v_mat;
mat4 p_mat;

class app_name : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    zxd::v_mat = glm::lookAt(vec3(0, -100, 100), vec3(0), pza);
    p_mat = glm::perspective(fpi2, wnd_aspect(), 0.1f, 1000.0f);

    v0 = v1 = vec3(0);

    set_v_mat(&zxd::v_mat);
  }

  virtual void update() {
    v0 = v1;
    v1 = glm::sphericalRand(1.0f);
    if(glm::linearRand(0.0f, 1.0f) < 0.02f)
      v1 *= 30;
    v1 = v0 + v1;
  }

  virtual void display() {
    //glClear(GL_COLOR_BUFFER_BIT);

    mat4 mvp_mat = p_mat * zxd::v_mat;
    debugger::draw_line(v0, v1, mvp_mat);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
    //m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::app_name app;
  app.run();
}
