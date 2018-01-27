#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"

using namespace glm;

namespace zxd {
class HelloWorld : public App {
protected:
  BitmapText mBitmapText;

public:
  HelloWorld() {}

protected:
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << mFps << std::endl;
    mBitmapText.print(ss.str(), 10, mInfo.wndHeight - 25);
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
    App::glfwKey(wnd, key, scancode, action, mods);
  }

  virtual void glfwMouseButton(
    GLFWwindow *wnd, int button, int action, int mods) {
    App::glfwMouseButton(wnd, button, action, mods);
  }

  virtual void glfwMouseMove(GLFWwindow *wnd, double x, double y) {
    App::glfwMouseMove(wnd, x, y);
  }

  virtual void glfwMouseWheel(GLFWwindow *wnd, double xoffset, double yoffset) {
    App::glfwMouseWheel(wnd, xoffset, yoffset);
  }
  virtual void glfwChar(GLFWwindow *wnd, unsigned int codepoint){
    App::glfwChar(wnd, codepoint);
  }
  virtual void glfwCharmod(GLFWwindow *wnd, unsigned int codepoint, int mods){
    App::glfwCharmod(wnd, codepoint, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::HelloWorld app;
  app.run();
}
