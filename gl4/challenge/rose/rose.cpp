// https://en.wikipedia.org/wiki/Rose_(mathematics)
#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include <sstream>
#include <numeric>
#include "rose.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

lightless_program prg;
kcip kci_n;
kcip kci_d;
kcip kci_slice;
kcip kci_start;
kcip kci_end;
kcip kci_line_width;
kcip kci_offset;

class app_name : public app {
protected:
  bitmap_text m_text;
  rose m_rose;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    prg.fix2d_camera(-1, 1, -1, 1);

    auto callback = std::bind(std::mem_fn(&app_name::update_rose), this, std::placeholders::_1);
    kci_n = m_control.add_control(GLFW_KEY_Q, 1, 1, 1000, 1, callback);
    kci_d = m_control.add_control(GLFW_KEY_W, 1, 1, 1000, 1, callback);
    kci_slice = m_control.add_control(GLFW_KEY_E, 1024, 1, 1000, 1, callback);
    kci_line_width = m_control.add_control<GLfloat>(GLFW_KEY_R, 1, 1, 50, 1, callback);
    kci_start = m_control.add_control<GLfloat>(GLFW_KEY_U, 0, 0, 1, 0.01, callback);
    kci_end = m_control.add_control<GLfloat>(GLFW_KEY_I, 1, 0, 1, 0.01, callback);
    kci_offset = m_control.add_control<GLfloat>(GLFW_KEY_O, 0, -999, 999, 0.1, callback);

    update_rose(0);
  }

  void update_rose(const kci* kci)
  {
    glLineWidth(kci_line_width->get_float());
    m_rose.radius(1);
    m_rose.slice(128);
    m_rose.n(kci_n->get_int());
    m_rose.d(kci_d->get_int());
    m_rose.start(kci_start->get_float());
    m_rose.end(kci_end->get_float());
    m_rose.slice(kci_slice->get_int());
    m_rose.offset(kci_offset->get_float());
    m_rose.build_mesh();

    //const auto& vertices = m_rose.vertices();
    //vec2 min_ext = vec2(-500);
    //vec2 max_ext = vec2(500);
    //for (int i = 0; i < vertices.size(); ++i) {
      //const vec2& v = vertices[i];
      //if(min_ext.x > v.x) min_ext.x = v.x;
      //if(min_ext.y > v.y) min_ext.y = v.y;
      //if(max_ext.x < v.x) max_ext.x = v.x;
      //if(max_ext.y < v.y) max_ext.y = v.y;
    //}
    //min_ext *= 1.05;
    //max_ext *= 1.05;

    //prg.fix2d_camera(min_ext.x, max_ext.x, min_ext.y, max_ext.y);
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat));
    glUniform4f(prg.ul_color, 1,1,1,1);
    m_rose.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : n : " << kci_n->get_int() << std::endl;
    ss << "w : d : " << kci_d->get_int() << std::endl;
    ss << "e : slice : " << kci_slice->get_int() << std::endl;
    ss << "r : line width : " << kci_line_width->get_float() << std::endl;
    ss << "u : start : " << kci_start->get_float() << std::endl;
    ss << "i : end : " << kci_end->get_float() << std::endl;
    ss << "o : offset : " << kci_offset->get_float() << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
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
