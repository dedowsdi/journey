#include <sstream>
#include <bitset>

#include <app.h>
#include <jsoncfg.h>

namespace zxd {

const auto width = 800;
const auto height = 800;
json_cfg cfg;
auto pattern = 0xffffu;
auto scale = 1.0f;

void reload_cfg()
{
  cfg.load("line_stipple.json");
  glLineWidth(cfg.get_float("line_width"));
  if (cfg.get_bool("line_smooth"))
  {
    glEnable(GL_LINE_SMOOTH);
  }
  else
  {
    glDisable(GL_LINE_SMOOTH);
  }
  pattern = std::stoi(cfg.get_string("pattern"), 0, 2);
  scale = cfg.get_float("scale");
}

struct vertex
{
  vec2 pos;
  vec4 color;
};

struct drawable
{
  GLuint vao;
  GLuint vbo;

  void draw()
  {
    glBindVertexArray(vao);
    glDrawArrays(GL_LINES, 0, 2);
  }
} line;

auto vsize = sizeof(vertex);
auto origin = vertex{vec2(0), vec4(1)};
auto point = vertex{vec2(width, height), vec4(1)};
auto mvp_mat = ortho<GLfloat>(0, width, 0, height);

class stipple_program : public program
{
public:
  GLint ul_mvp_mat = 0;
  GLint ul_vp = 1;
  GLint ul_pattern = 2;
  GLint ul_scale = 3;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/stipple.vs.glsl");
    attach(GL_GEOMETRY_SHADER, "shader4/stipple.gs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/stipple.fs.glsl");
  }

  void bind_uniform_locations()
  {

  }

  void bind_attrib_locations()
  {

  }

} prg;

class app_tt : public app
{

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(GLFWwindow *wnd, int key, int scancode, int action,
                int mods) override;

  void glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                         int mods) override;

  void glfw_mouse_move(GLFWwindow *wnd, double x, double y) override;
};

void app_tt::init_info()
{
  app::init_info();
  m_info.title = "app_tt :";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

void app_tt::create_scene()
{
  glGenVertexArrays(1, &line.vao);
  glGenBuffers(1, &line.vbo);

  glBindVertexArray(line.vao);
  glBindBuffer(GL_ARRAY_BUFFER, line.vbo);
  glBufferData(GL_ARRAY_BUFFER, vsize * 2, 0, GL_STATIC_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, value_ptr(origin.pos));
  glBufferSubData(GL_ARRAY_BUFFER, vsize, vsize, value_ptr(point.pos));

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vsize, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vsize,
                        BUFFER_OFFSET(sizeof(vec2)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  prg.init();
  reload_cfg();
}

void app_tt::update()
{
  if (cfg.modified_outside())
  {
    reload_cfg();
  }
}

void app_tt::display()
{
  glClear(GL_COLOR_BUFFER_BIT);

  prg.use();
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  glUniform1ui(prg.ul_pattern, pattern);
  glUniform1f(prg.ul_scale, scale);

  ivec4 vp;
  glGetIntegerv(GL_VIEWPORT, value_ptr(vp));
  glUniform4iv(prg.ul_vp, 1, value_ptr(vp));

  line.draw();

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  GLfloat line_width;
  glGetFloatv(GL_LINE_WIDTH, &line_width);
  ss << "lmc : " << origin.pos << std::endl;
  ss << "mm  : " << point.pos << std::endl;
  ss << "GL_LINE_WIDTH : " << line_width << std::endl;
  ss << "GL_LINE_SMOOTH : " << static_cast<GLint>(glIsEnabled(GL_LINE_SMOOTH))
     << std::endl;
  ss << "pattern : " << std::bitset<16>(pattern) << std::endl;
  ss << "scale : " << scale << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);

  glDisable(GL_BLEND);
}

void app_tt::glfw_resize(GLFWwindow *wnd, int w, int h)
{
  app::glfw_resize(wnd, w, h);
  m_text.reshape(m_info.wnd_width, m_info.wnd_height);
}

void app_tt::glfw_key(GLFWwindow *wnd, int key, int scancode, int action,
                      int mods)
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

void app_tt::glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                               int mods)
{
  app::glfw_mouse_button(wnd, button, action, mods);
  if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
  {
    origin.pos = current_mouse_position();
    origin.color = linearRand(vec4(0), vec4(1));
    glBindBuffer(GL_ARRAY_BUFFER, line.vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vsize, value_ptr(origin.pos));
  }
}

void app_tt::glfw_mouse_move(GLFWwindow* wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
  point.pos = current_mouse_position();
  point.color = linearRand(vec4(0), vec4(1));
  glBindBuffer(GL_ARRAY_BUFFER, line.vbo);
  glBufferSubData(GL_ARRAY_BUFFER, vsize, vsize, value_ptr(point.pos));
}
}

int main(int argc, char *argv[])
{
  zxd::app_tt app;
  app.run();
}
