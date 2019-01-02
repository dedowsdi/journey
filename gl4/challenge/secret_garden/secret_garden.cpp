#include "app.h"

#include <sstream>

#include "bitmap_text.h"
#include "common_program.h"
#include "bug.h"

#define WIDTH 800
#define HEIGHT 800

#define BUFFER_OFFSET(x) ((GLubyte*)NULL + (x))

namespace zxd {

lightless_program prg;


class secret_garden_app : public app {
private:
  bitmap_text m_text;
  bug m_bug;

public:
  virtual void init_info();
  virtual void create_scene();

  virtual void update();

  virtual void display();

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h);

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods);
};

void secret_garden_app::init_info() {
  app::init_info();
  m_info.title = "secret_garden_app";
  m_info.wnd_width = WIDTH;
  m_info.wnd_height = HEIGHT;
}

void secret_garden_app::create_scene() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  m_text.init();
  m_text.reshape(wnd_width(), wnd_height());

  prg.with_color = true;
  prg.init();
  prg.p_mat = zxd::rect_ortho(30, 30, wnd_aspect());
}

void secret_garden_app::update() {
  m_bug.update();
}

void secret_garden_app::display() {
  glClear(GL_COLOR_BUFFER_BIT);

  prg.use();

  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.p_mat));
  m_bug.draw();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "";
  m_text.print(ss.str(), 10, wnd_height()- 20);
  glDisable(GL_BLEND);
}

void secret_garden_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
  m_text.reshape(wnd_width(), wnd_height());
}

void secret_garden_app::glfw_key(
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

}

int main(int argc, char *argv[]) {
  zxd::secret_garden_app app;
  app.run();
}
