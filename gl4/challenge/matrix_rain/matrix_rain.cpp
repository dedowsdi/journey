#include <sstream>
#include <algorithm>
#include <iterator>
#include <memory>

#include <ft2build.h>
#include FT_FREETYPE_H
#include "glc_utf8.h"
#include "app.h"
#include "bitmap_text.h"
#include "freetype_text.h"
#include "stream_util.h"
#include "string_util.h"
#include "rain.h"
#include "bitmap_text.h"

namespace zxd
{

const GLint width = 720;
const GLint height = 720;
bool display_help = true;

freetype_text ft("font/DejaVuSansMono.ttf");
std::shared_ptr<bitmap_text> bt;

class matrix_rain_app : public app
{
private:
  std::unique_ptr<matrix_rain> m_matrix_rain;

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) override;
};

void matrix_rain_app::init_info()
{
  app::init_info();
  m_info.title = "matrix_rain_app";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

void matrix_rain_app::create_scene()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  ft.init();
  ft.set_height(18);
  ft.reset_content_to_all();

  bt = ft.create_bitmap_text(2048, 2048);

  glfw_resize(0, width, height);
}

void matrix_rain_app::update()
{

  if(m_frame_number % 4 != 0)
    return;

  m_matrix_rain->update();
}

void matrix_rain_app::display()
{
  glClear(GL_COLOR_BUFFER_BIT);

  m_matrix_rain->draw();

  if(!display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "fps : " << m_fps << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void matrix_rain_app::glfw_resize(GLFWwindow *wnd, int w, int h)
{
  app::glfw_resize(wnd, w, h);

  if(bt)
  {
    bt->reshape(wnd_width(), wnd_height());
    m_matrix_rain = std::make_unique<matrix_rain>(bt, wnd_width(), wnd_height(), 5);
  }
}

void matrix_rain_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (key)
    {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;
      case GLFW_KEY_H:
        display_help ^= 1;
        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

}

int main(int argc, char *argv[])
{
  zxd::matrix_rain_app app;
  app.run();
}
