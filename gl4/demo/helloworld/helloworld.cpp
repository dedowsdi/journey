#include "app.h"
#include "bitmap_text.h"

namespace zxd
{
class hello_world : public app
{

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "hello world";
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
  }

  virtual void update() {}

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_text.print("hello world", 10, 492);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
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

int main(int argc, char *argv[])
{
  zxd::hello_world app;
  app.run();
}
