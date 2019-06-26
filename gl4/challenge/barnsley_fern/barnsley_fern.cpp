#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"
#include "debugger.h"

#define WIDTH 512
#define HEIGHT 512

namespace zxd
{

glm::mat4 m_mat;
glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 vp_mat;

GLuint num_points = 10000000;
lightless_program prg;
vec2_vector points;
GLuint draw_start = 0;
GLuint draw_count = num_points / 500;

class barnsley_fern_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "barnsley_fern_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    prg.init();
    //−2.1820 < x < 2.6558 and 0 ≤ y < 9.9983.
    p_mat = glm::ortho(-2.7, 2.7, 0.0, 10.5);
    v_mat = mat4(1);
    vp_mat = p_mat * v_mat;

    reset_fern();
  }

  void reset_fern()
  {
    points.clear();
    points.reserve(num_points);

    vec2 pos;
    points.push_back(pos);
    for (int i = 1; i < num_points; ++i) 
    {
      GLfloat r = zxd::random();
      if(r < 0.01)
      {
        pos.x = 0;
        pos.y *= 0.16;
      }
      else if(r < 0.86)
      {
        GLfloat x =  0.85*pos.x + 0.04 * pos.y;
        GLfloat y = -0.04*pos.x + 0.85 * pos.y + 1.6;
        pos = vec2(x,y);
      }
      else if(r < 0.93)
      {
        GLfloat x = 0.2*pos.x - 0.26*pos.y;
        GLfloat y = 0.23*pos.x + 0.22*pos.y + 1.6;
        pos = vec2(x,y);
      }
      else
      {
        GLfloat x = -0.15*pos.x + 0.28*pos.y;
        GLfloat y = 0.26*pos.x + 0.24*pos.y + 0.44;
        pos = vec2(x,y);
      }
      points.push_back(pos);
    }

    glClear(GL_COLOR_BUFFER_BIT);
  }

  virtual void update() 
  {

  }

  virtual void display()
  {

    //prg.use();
    if(draw_start != num_points)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_ONE, GL_ONE);
      debugger::draw_point(points.begin() + draw_start, points.begin() + draw_start + draw_count,
          vp_mat, 1, vec4(0, 0.01, 0, 1)); // weird, can't use any number samller than 0.003
      draw_start += draw_count;
    }


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
  zxd::barnsley_fern_app app;
  app.run();
}
