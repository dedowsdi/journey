#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"
#include "debugger.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

lightless_program prg;

struct tentacle
{
  GLfloat angle = 0;
  GLfloat acm_angle = 0; // accumulated angle
  GLfloat len;
  GLfloat xoff;
  vec2 start;
  vec2 end;

  tentacle* child = 0;
  tentacle* parent = 0;

  tentacle()
  {
    xoff = glm::linearRand(0, 1);
  }

  ~tentacle()
  {
    if(child)
      delete child;
  }

  void update_acm_angle(GLfloat a)
  {
    acm_angle = a;
    end = start + vec2(cos(acm_angle), sin(acm_angle)) * len;
  }

  // update acc_angle and end
  void update()
  {
    GLfloat a  = angle;
    if(parent)
    {
      start = parent->end;
      a += parent->acm_angle;
    }
    update_acm_angle(a);

    if(child)
      child->update();
  }

  void set_child(tentacle* t)
  {
    child = t;
    t->start = end;
    t->parent = this;
    t->update();
  }

  void wiggle()
  {
    angle = glm::perlin(vec2(xoff, 0));
    xoff += 0.01;
  }

};

class forward_kinematics_app : public app {
protected:
  bitmap_text m_text;
  tentacle* m_root_tenacle;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "forward_kinematics_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    m_root_tenacle = new tentacle;
    m_root_tenacle->start = vec2(WIDTH*0.5f, 0);
    m_root_tenacle->angle = fpi2;
    m_root_tenacle->len = 30;
    m_root_tenacle->update();

    tentacle* parent = m_root_tenacle;
    for (int i = 0; i < 20; ++i) 
    {
      tentacle* current = new tentacle;
      current->len = parent->len;
      parent->set_child(current);
      parent = current;
    }
  }

  virtual void update() {
    m_root_tenacle->angle = fpi2 + sin(m_current_time) * 0.05;
    tentacle* current = m_root_tenacle->child;
    while(current)
    {
      current->wiggle();
      current = current->child;
    }
    m_root_tenacle->update();
  
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();

    tentacle* current = m_root_tenacle;
    while(current)
    {
      debugger::draw_line(current->start, current->end, prg.vp_mat);
      current = current->child;
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  virtual void glfw_key(
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
};
}

int main(int argc, char *argv[]) {
  zxd::forward_kinematics_app app;
  app.run();
}
