#include "app.h"

namespace zxd {
class HelloWorld : public App {
public:
  virtual void createScene() { glClearColor(0.0f, 0.5f, 1.0f, 1.0f); }

  virtual void update() {}

  virtual void display() { glClear(GL_COLOR_BUFFER_BIT); }

  virtual void glfwKey(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(mWnd, GL_TRUE);
          break;
        default:
          break;
      }
    }
  }
};
}

int main(int argc, char *argv[]) {
  zxd::HelloWorld app;
  app.run();
}
