#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include "super_shape_2d.h"
#include <sstream>

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

twod_program0 prg;
super_shape_2d shape;

key_control_item* n1;
key_control_item* n2;
key_control_item* n3;
key_control_item* a;
key_control_item* b;
key_control_item* m;
key_control_item* slice;

class supershape_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "supershape_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    prg.init();
    prg.fix2d_camera(0, m_info.wnd_width, 0, m_info.wnd_height);

    auto callback = std::bind(std::mem_fn(&supershape_app::update_shape), this, std::placeholders::_1);
    n1 = m_control.add_control(GLFW_KEY_Q, 1, -10000, 10000, 0.1, callback);
    n2 = m_control.add_control(GLFW_KEY_W, 1, -10000, 10000, 0.1, callback);
    n3 = m_control.add_control(GLFW_KEY_E, 1, -10000, 10000, 0.1, callback);
    m = m_control.add_control(GLFW_KEY_R, 0, -10000, 10000, 0.1, callback);
    a = m_control.add_control(GLFW_KEY_U, 1, -10000, 10000, 0.1, callback);
    b = m_control.add_control(GLFW_KEY_I, 1, -10000, 10000, 0.1, callback);
    slice = m_control.add_control(GLFW_KEY_O, 64, -10000, 10000, 1, callback);
    update_shape(0);
  }

  virtual void update() {}

  void update_shape(const key_control_item* item)
  {
    shape.n1(n1->value);
    shape.n2(n2->value);
    shape.n3(n3->value);
    shape.a(a->value);
    shape.b(b->value);
    shape.m(m->value);
    shape.radius(100);
    shape.num_slice(slice->value);
    shape.build_mesh();
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    prg.use();

    mat4 m_mat = glm::translate(vec3(WIDTH * 0.5, HEIGHT * 0.5, 0));
    mat4 mvp_mat = prg.vp_mat * m_mat;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4f(prg.ul_color, 1, 1, 1, 1);
    shape.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : n1 : " << n1->value << std::endl;
    ss << "wW : n2 : " << n2->value << std::endl;
    ss << "eE : n3 : " << n3->value << std::endl;
    ss << "rR : m : " << m->value << std::endl;
    ss << "uU : a : " << a->value << std::endl;
    ss << "iI : b : " << b->value << std::endl;
    ss << "oO : slice : " << slice->value << std::endl;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

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
  zxd::supershape_app app;
  app.run();
}
