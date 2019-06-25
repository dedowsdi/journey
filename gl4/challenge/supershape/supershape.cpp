#include "app.h"
#include "bitmap_text.h"
#include "common_program.h"
#include "super_shape_2d.h"
#include <sstream>
#include "debugger.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

lightless_program prg;
super_shape_2d shape;

glm::mat4 v_mat;
glm::mat4 p_mat;

kcip n1;
kcip n2;
kcip n3;
kcip a;
kcip b;
kcip m;
kcip slice;

class supershape_app : public app
  {
protected:
  bitmap_text m_text;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "supershape_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());

    auto callback = std::bind(std::mem_fn(&supershape_app::update_shape), this, std::placeholders::_1);
    n1 = m_control.add_control<GLfloat>(GLFW_KEY_Q, 1, -10000, 10000, 0.1, callback);
    n2 = m_control.add_control<GLfloat>(GLFW_KEY_W, 1, -10000, 10000, 0.1, callback);
    n3 = m_control.add_control<GLfloat>(GLFW_KEY_E, 1, -10000, 10000, 0.1, callback);
    m = m_control.add_control<GLfloat>(GLFW_KEY_R, 0, -10000, 10000, 0.1, callback);
    a = m_control.add_control<GLfloat>(GLFW_KEY_U, 1, -10000, 10000, 0.1, callback);
    b = m_control.add_control<GLfloat>(GLFW_KEY_I, 1, -10000, 10000, 0.1, callback);
    slice = m_control.add_control<GLint>(GLFW_KEY_O, 64, -10000, 10000, 1, callback);
    update_shape(0);
  }

  virtual void update() {}

  void update_shape(const kci* item)
  {
    shape.n1(n1->get_float());
    shape.n2(n2->get_float());
    shape.n3(n3->get_float());
    shape.a(a->get_float());
    shape.b(b->get_float());
    shape.m(m->get_float());
    shape.radius(100);
    shape.num_slice(slice->get_int());
    shape.build_mesh();
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    prg.use();

    mat4 m_mat = glm::translate(vec3(WIDTH * 0.5, HEIGHT * 0.5, 0));
    mat4 mvp_mat = p_mat * m_mat;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4f(prg.ul_color, 1, 1, 1, 1);
    shape.draw();
    
    debugger::draw_point(shape.attrib_vec2_array(0)->get_vector(), mvp_mat, 5, vec4(0,0,0,1));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : n1 : " << n1->get_float() << std::endl;
    ss << "wW : n2 : " << n2->get_float() << std::endl;
    ss << "eE : n3 : " << n3->get_float() << std::endl;
    ss << "rR : m : " << m->get_float() << std::endl;
    ss << "uU : a : " << a->get_float() << std::endl;
    ss << "iI : b : " << b->get_float() << std::endl;
    ss << "oO : slice : " << slice->get_int() << std::endl;

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
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
  zxd::supershape_app app;
  app.run();
}
