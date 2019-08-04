#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"
#include "lissajous.h"
#include "debugger.h"

#define WIDTH 720
#define HEIGHT 720

namespace zxd
{

glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 vp_mat;

lightless_program prg;
GLuint rows = 10;
GLuint cols = 10;
GLfloat radius = 1;

vec4 base_color = vec4(0.6, 0, 0, 1);
vec4 lissa_color = vec4(1, 0, 1, 1);
vec4 dot_color = vec4(0, 1, 0, 1);
vec4 line_color = vec4(1, 0, 0, 1);
GLfloat scale = 0.9f;

GLuint lissa_type = 1;
GLuint rose_n = 2;
GLuint rose_d = 1;
GLfloat rose_offset = 0.01f;

bool display_help = true;
kcip kci_lissa_type;
kcip kci_rose_n;
kcip kci_rose_d;
kcip kci_rose_offset;

vec2_vector dots;
vec2_vector lines;

std::vector<lissajous> lisas;

class lissajous_app : public app
{
protected:

public:

  virtual void init_info()
  {
    app::init_info();
    m_info.title = "lissajous_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 8;
    m_info.decorated = false;
  }

  virtual void create_scene()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);

    glfwSetWindowPos(m_wnd, 100, 100);


    prg.init();
    p_mat = zxd::rect_ortho((cols+1)*radius, (rows+1)*radius, wnd_aspect());

    auto callback = std::bind(std::mem_fn(&lissajous_app::reset_pattern), this, std::placeholders::_1);
    kci_lissa_type = m_control.add<GLint>(GLFW_KEY_Q, lissa_type, 0, 1, 1, callback);
    kci_rose_n = m_control.add<GLint>(GLFW_KEY_W, rose_n, 1, 100, 1, callback);
    kci_rose_d = m_control.add<GLint>(GLFW_KEY_E, rose_d, 1, 100, 1, callback);
    kci_rose_offset = m_control.add<GLfloat>(GLFW_KEY_R, rose_offset, 0, 100, 0.1, callback);
    reset_pattern(0);
  }

  virtual void update() 
  {
    dots.clear();
    lines.clear();
  }

  void reset_pattern(const kci* item)
  {
    lissa_type = kci_lissa_type->get_int();
    rose_n = kci_rose_n->get_int();
    rose_d = kci_rose_d->get_int();
    rose_offset = kci_rose_offset->get_float();

    lisas.clear();
    for (int y = 0; y < rows; ++y) 
    {
      for (int x = 0; x < cols; ++x) 
      {
        lissajous lisa;
        lisa.xscale(x+1);
        lisa.yscale(y+1);
        lisa.slices(256);
        lisa.type(static_cast<lissajous::LISSA_TYPE>(lissa_type));
        lisa.rose_n(rose_n);
        lisa.rose_d(rose_d);
        lisa.rose_offset(rose_offset);

        lisa.build_mesh();
        lisas.push_back(lisa);
      }
    }

  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(base_color));
    GLfloat t = m_current_time * 0.5f;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // draw 1st col and row
    vec2 lt = vec2(-radius * (cols+1), radius * (rows+1)) + vec2(radius, -radius);
    for (int i = 0; i <= cols; ++i)
    {
      vec2 pos = lt + vec2(i, 0) * radius * 2.0f;
      mat4 mvp_mat = glm::scale(glm::translate(p_mat, vec3(pos, 0)), vec3(scale,scale,1));
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
      lisas[0].draw();

      if(i == 0)
        continue;

      vec2 dot = lisas[0].get_at_angle(t*i) * scale + pos;
      dots.push_back(dot);
      lines.push_back(dot);
      lines.push_back(dot + vec2(0, -2 * radius * (rows + 1)));
    }

    for (int i = 0; i <= rows; ++i)
    {
      vec2 pos = lt + vec2(0, -i) * radius * 2.0f;
      mat4 mvp_mat = glm::scale(glm::translate(p_mat, vec3(pos, 0)), vec3(scale,scale,1));
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
      lisas[0].draw();

      if(i == 0)
        continue;

      vec2 dot = lisas[0].get_at_angle(t * i) * scale + pos;
      dots.push_back(dot);
      lines.push_back(dot);
      lines.push_back(dot + vec2(2 * radius * (rows + 1), 0));
    }

    // draw lissajous
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(lissa_color));
    lt += vec2(radius, -radius) * 2.0f;
    for (int y = 0; y < rows; ++y) 
    {
      for (int x = 0; x < cols; ++x) 
      {
        vec2 pos = lt + vec2(x, - y) * radius * 2.0f;
        mat4 mvp_mat = glm::scale(glm::translate(p_mat, vec3(pos, 0)), vec3(scale,scale,1));
        glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
      
        lissajous& lisa = lisas[y * cols + x];
        lisa.draw();
        dots.push_back(lisa.get_at_angle(t) * scale + pos);
      }
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    glBlendColor(1,1,1,0.6);
    debugger::draw_point(dots, p_mat, 5, dot_color);
    debugger::draw_line(GL_LINES, lines, p_mat, 1, line_color);
    glDisable(GL_BLEND);

    if(!display_help)
      return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q  : type : " << (lissa_type == 0 ? "circle" : "rose") << std::endl;;
    ss << "wW : rose_n : " << rose_n << std::endl;
    ss << "eE : rose_d : " << rose_d << std::endl;
    ss << "rR : rose_offset : " << rose_offset << std::endl;
    ss << "h : hide help : " << std::endl;
    ss << "fps : " << m_fps << std::endl;
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
        case GLFW_KEY_H:
          display_help ^= 1;
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
  zxd::lissajous_app app;
  app.run();
}
