/*
 * Changed from superbible.
 */
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <string>

namespace zxd {

void glfwErrorCallback(int error, const char *description);
void glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar *message, const void *userParam);

class App {
public:
  struct AppInfo {
    std::string title;
    GLuint wndWidth;
    GLuint wndHeight;
    GLuint majorVersion;
    GLuint minorVersion;
    GLuint samples;
    GLboolean fullscreen;
    GLboolean vsync;
    GLboolean cursor;
    GLboolean stereo;
    GLboolean debug;
  };

  App(){}

  void init();
  virtual void run();

protected:
  virtual void initInfo();
  virtual void initGL();
  virtual void initWnd();
  virtual void createScene() {}
  virtual void update() {}
  virtual void display() {}

  virtual void loop();
  virtual void shutdown() {}

  virtual void glfwResize(GLFWwindow *wnd, int w, int h);
  virtual void glfwKey(
    GLFWwindow *wnd, int key, int scancode, int action, int mods);
  virtual void glfwMouseButton(
    GLFWwindow *wnd, int button, int action, int mods) {}
  virtual void glfwMouseMove(GLFWwindow *wnd, double x, double y){}
  virtual void glfwMouseWheel(GLFWwindow *wnd, double xoffset, double yoffset) {}

protected:
  bool mExit;
  AppInfo mInfo;
  GLFWwindow *mWnd;
};
}
