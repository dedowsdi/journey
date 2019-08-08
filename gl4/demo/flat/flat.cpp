#include <sstream>

#include <app.h>
#include <glenumstring.h>

namespace zxd {

const auto width = 800;
const auto height = 800;

std::shared_ptr<kci> provoke_mode;

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
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }
} triangle;

auto vsize = sizeof(vertex);
auto mvp_mat = mat4(1);

class program_name : public program
{
public:
  GLint ul_mvp_mat = 0;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/flat.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/flat.fs.glsl");
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
  // first red, last blue
  std::array<vertex, 3> vertices = {
    vertex{vec2(-1, -1), vec4(1, 0, 0, 1)},
    vertex{vec2( 1, -1), vec4(0, 1, 0, 1)},
    vertex{vec2( 1,  1), vec4(0, 0, 1, 1)},
  };

  glGenVertexArrays(1, &triangle.vao);
  glGenBuffers(1, &triangle.vbo);

  glBindVertexArray(triangle.vao);
  glBindBuffer(GL_ARRAY_BUFFER, triangle.vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertex) * vertices.size(),
               value_ptr(vertices.front().pos), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, vsize, BUFFER_OFFSET(0));
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, vsize,
                        BUFFER_OFFSET(sizeof(vec2)));
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  prg.init();
  provoke_mode = m_control.add_enum(
    'Q', {GL_FIRST_VERTEX_CONVENTION, GL_LAST_VERTEX_CONVENTION}, 1,
    [](auto* kci) -> void { glProvokingVertex(provoke_mode->get_int_enum()); });
}

void app_tt::update() {}

void app_tt::display()
{
  glClear(GL_COLOR_BUFFER_BIT);

  prg.use();
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  triangle.draw();

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  GLint layer;
  glGetIntegerv(GL_LAYER_PROVOKING_VERTEX, &layer);
  ss << "first red, second green, last blue" << std::endl;
  ss << "GL_LAYER_PROVOKING_VERTEX : " << (layer == GL_PROVOKING_VERTEX
                                             ? "GL_PROVOKING_VERTEX"
                                             : gl_provoke_mode_to_string(layer))
     << std::endl;
  ss << "Q : GL_PROVOKING_VERTEX : "
     << gl_provoke_mode_to_string(provoke_mode->get_int_enum());
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
}

void app_tt::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
}

}

int main(int argc, char *argv[])
{
  zxd::app_tt app;
  app.run();
}
