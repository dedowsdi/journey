#include "app.h"
#include "bitmap_text.h"

namespace zxd {
class HelloWorld : public App {
protected:
  BitmapText mBitmapText;

public:
  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "hello world";
  }
  virtual void createScene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mBitmapText.print("Hello World", 10, 492);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfwResize(GLFWwindow *wnd, int w, int h) {
    App::glfwResize(wnd, w, h);
    mBitmapText.reshape(w, h);
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
