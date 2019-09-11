#include <app.h>
#include <bitmap_text.h>
#include <sstream>
#include <common_program.h>
#include <circle.h>

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

lightless_program prg;
circle geometry;

glm::mat4 v_mat;
glm::mat4 p_mat;

class recursion_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "recursion_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());

    geometry.type(circle::LINE);
    geometry.slice(64);
    geometry.build_mesh();
  }

  virtual void update() {}

  void draw_circle(const vec2& pos, float size)
  {
    if(size < 30)
      return;

    mat4 m = glm::scale(glm::translate(vec3(pos, 0)), vec3(size,size, 1));
    mat4 mvp_mat = p_mat * m;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    geometry.draw();

    draw_circle(pos+vec2(size*0.5, 0), 0.5*size);
    draw_circle(pos+vec2(size, 0), 0.5*size);
    draw_circle(pos-vec2(size*0.5, 0), 0.5*size);
    draw_circle(pos-vec2(size, 0), 0.5*size);
    draw_circle(pos+vec2(0, size*0.5), 0.5*size);
    draw_circle(pos+vec2(0, size), 0.5*size);
    draw_circle(pos-vec2(0, size*0.5), 0.5*size);
    draw_circle(pos-vec2(0, size), 0.5*size);
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniform4f(prg.ul_color, 1, 1, 1, 1);

    draw_circle(vec2(0.5*WIDTH, 0.5*HEIGHT), 200);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
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
  zxd::recursion_app app;
  app.run();
}
