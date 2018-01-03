#include "app.h"

namespace zxd {
class HelloWorld : public App {
public:
  virtual void createScene() { glClearColor(0.0f, 0.5f, 1.0f, 1.0f); }

  virtual void update() {}

  virtual void display() { glClear(GL_COLOR_BUFFER_BIT); }

  virtual void glfwResize(GLFWwindow *wnd, int w, int h) {
    (void)wnd;
    mInfo.wndWidth = w;
    mInfo.wndWidth = h;
  }

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

  virtual void glfwMouseButton(
    GLFWwindow *wnd, int button, int action, int mods) {}

  virtual void glfwMouseMove(GLFWwindow *wnd, double x, double y) {}

  virtual void glfwMouseWheel(GLFWwindow *wnd, double xoffset, double yoffset) {
  }
};
}

int main(int argc, char *argv[]) {
  zxd::HelloWorld app;
  app.run();
}
