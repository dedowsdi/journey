#include "app.h"
#include "bitmap_text.h"
#include "quad.h"
#include "program.h"
#include "texutil.h"
#include <sstream>

namespace zxd
{

struct plot_program : public program
{
  GLint ul_resolution;
  GLint ul_mode;
  GLint ul_exponent;
  GLint ul_line_width; // in percent of screen height
  GLint ul_translation;
  GLint ul_stretch;

  plot_program() {}

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/plot.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/plot.fs.glsl");
  }

  void update_uniforms(GLuint tex_index = 0)
  {
  }


  virtual void bind_uniform_locations()
  {
    ul_resolution = get_uniform_location("resolution");
    ul_mode = get_uniform_location("mode");
    ul_exponent = get_uniform_location("exponent");
    ul_line_width = get_uniform_location("line_width");
    ul_translation = get_uniform_location("translation");
    ul_stretch = get_uniform_location("stretch");
  }

  virtual void bind_attrib_locations()
  {
  }
} prg;

class plot : public app
{
protected:
  GLuint m_vao;
  GLint m_mode;
  GLfloat m_exponent;
  GLfloat m_line_width;
  vec2 m_translation;
  vec2 m_stretch;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "sine wave";
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    glGenVertexArrays(1, &m_vao);

    prg.init();

    m_mode = 0;
    m_exponent = 0.5;
    m_line_width = 0.02;
    m_translation = vec2(0);
    m_stretch = vec2(1);
  }

  virtual void update()
  {
  }

  std::string getModeString()
  {
    switch(m_mode)
    {
      case 0:
        return "x";

      case 1:
        return "1 - pow(abs(x), exponent)";

      case 2:
        return "pow(cos(PI * x / 2.0), exponent)";

      case 3:
        return "1- pow(abs(sin(PI * x / 2.0)), exponent)";

      case 4:
        return "pow(min(cos(PI*x/2.0), 1-abs(x)), exponent)";

      case 5:
        return "1 - pow(max(0, abs(x) * 2.0 - 1.0), exponent)";

      default:
        return "";
    }
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);
    prg.use();
    glUniform2f(prg.ul_resolution, wnd_width(), wnd_height());
    glUniform1i(prg.ul_mode, m_mode);
    glUniform1f(prg.ul_exponent, m_exponent);
    glUniform1f(prg.ul_line_width, m_line_width);
    glUniform2fv(prg.ul_translation,1, glm::value_ptr(m_translation));
    glUniform2fv(prg.ul_stretch, 1, glm::value_ptr(m_stretch));

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : mode : " << getModeString() << std::endl;
    ss << "wW : exponent : " << m_exponent << std::endl;
    ss << "eE : m_line_width : " << m_line_width << std::endl;
    ss << "uU : m_translation.x : " << m_translation.x << std::endl;
    ss << "iI : m_translation.y : " << m_translation.y << std::endl;
    ss << "oO : m_stretch.x : " << m_stretch.x << std::endl;
    ss << "pP : m_stretch.y : " << m_stretch.y << std::endl;
    m_text.print(ss.str(), 10, 492, vec4(1, 0, 0, 1));
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

        case GLFW_KEY_Q:
            m_mode = (m_mode + (mods & GLFW_MOD_SHIFT ? -1 : 1) )%6;
            break;

        case GLFW_KEY_W:
          m_exponent += 0.5 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_E:
          m_line_width += 0.002 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_U:
          m_translation.x += 0.1 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_I:
          m_translation.y += 0.1 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_O:
          m_stretch.x += 0.1 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_P:
          m_stretch.y += 0.1 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        default:
          break;
      }
    }
  }
};
}

int main(int argc, char *argv[])
{
  zxd::plot app;
  app.run();
}
