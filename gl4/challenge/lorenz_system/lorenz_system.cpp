#include "app.h"
#include "bitmaptext.h"
#include "dict_script.h"
#include "stream_util.h"
#include "common_program.h"

#include <sstream>

#define WIDTH 800
#define HEIGHT 800

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd {

vec3_vector vertices;
vec3_vector colors;
GLuint vao;
GLuint vbo[2]; // 0 vertex, 1 color

GLfloat time_step = 0.01;
GLuint count = 50000;
GLuint draw_count = 2;
GLuint draw_step = 3; // increment per frame
GLboolean dirty = GL_FALSE;

struct setting
{
  vec3 start = vec3(0.01, 0, 0);
  GLfloat sigma = 10;
  GLfloat rho =  28;
  GLfloat beta = 8.0/3;
};
setting s;

bool operator==(const setting& lhs, const setting& rhs)
{
  return lhs.start == rhs.start &&
         lhs.sigma == rhs.sigma &&
         lhs.rho == rhs.rho &&
         lhs.beta == rhs.beta;
}

bool operator!=(const setting& lhs, const setting& rhs)
{
  return !operator==(lhs, rhs);
}

lightless_program prg;

void update_buffer()
{
  draw_count = 2;
  if(vertices.size() != count)
  {
    vertices.resize(count);
    colors.resize(count);
  }

  vertices[0] = s.start;
  colors[0] = vec3(0);

  for (size_t i = 1; i < count; ++i) 
  {
    const vec3& p = vertices[i-1];
    vertices[i] = p;

    vertices[i].x += time_step * s.sigma*(p.y - p.x);
    vertices[i].y += time_step * (p.x * (s.rho - p.z) - p.y);
    vertices[i].z += time_step * (p.x * p.y - s.beta * p.z);

    colors[i] = vec3((i&0xff)/255.0, (i&0x7f)/127.0, (i&0x3f)/63.0);
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(vec3), glm::value_ptr(vertices.front()));

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, count * sizeof(vec3), glm::value_ptr(colors.front()));
}

void reset_buffer()
{
  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
  glBufferData(GL_ARRAY_BUFFER, count * sizeof(vec3), value_ptr(vertices[0]), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glVertexAttribPointer(1, 3, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
  glBufferData(GL_ARRAY_BUFFER, count * sizeof(vec3), value_ptr(vertices[0]), GL_STATIC_DRAW);
}

class lorenz_system : public app {

protected:
  bitmap_text m_text;
  dict_script* m_dict_script;

public:

  ~lorenz_system()
  {
    delete m_dict_script;
  }

  virtual void init_info() {
    app::init_info();
    m_info.title = "lorenz_system";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }

  void read_setting(bool force_update = false)
  {
    m_dict_script->update();
    setting ns;
    ns.start= m_dict_script->get("start", vec3(0.01, 0, 0));
    ns.sigma= m_dict_script->get("sigma", 10);
    ns.rho= m_dict_script->get("rho", 28);
    ns.beta = m_dict_script->get("beta ", 8.0/3);

    if(force_update || ns != s)
    {
      s = ns;
      update_buffer();
    }
  }

  virtual void create_scene() {

    glfwSetWindowPos(m_wnd, 1000, 10);
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.with_color = true;
    prg.init();
    prg.v_mat = glm::lookAt(vec3(0, -100, 100), vec3(0), pza);
    prg.p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 10000.0f);
    set_v_mat(&prg.v_mat);

    glGenVertexArrays(1, &vao);
    glGenBuffers(2, vbo);

    vertices.resize(count);
    colors.resize(count);

    m_dict_script = new dict_script(stream_util::get_resource("challenge/lorenz_system/lorenz_system.txt"));
    m_dict_script->track(0.1);

    reset_buffer();
    read_setting(true);
  }

  virtual void update() 
  {
    if(m_dict_script->changed())
    {
      read_setting();
    }
    else{
      draw_count += draw_step;
      draw_count = glm::min(count, draw_count);
    }
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();

    glBindVertexArray(vao);
    prg.mvp_mat = prg.p_mat * prg.v_mat;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.mvp_mat));

    glDrawArrays(GL_LINE_STRIP, 0, draw_count);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << " start " << s.start << std::endl;
    ss << " sigma " << s.sigma << std::endl;
    ss << " rho " << s.rho << std::endl;
    ss << " beta " << s.beta << std::endl;
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
  zxd::lorenz_system app;
  app.run();
}
