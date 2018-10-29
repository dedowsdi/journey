// https://en.wikipedia.org/wiki/Rose_(mathematics)
#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include <sstream>
#include <numeric>

#define BUFFER_OFFSET(x) ((GLubyte*)NULL + (x))

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

lightless_program prg;
key_control_item* kci_n;
key_control_item* kci_d;
key_control_item* kci_slice;
key_control_item* kci_start;
key_control_item* kci_end;
key_control_item* kci_line_width;
key_control_item* kci_offset;

class rose
{
protected:
  GLuint m_vao = -1;
  GLuint m_vbo = -1;
  GLuint m_n = 1;
  GLuint m_d = 1;
  GLuint m_slice = 32;
  GLfloat m_radius = 1;
  GLfloat m_offset = 0;
  GLfloat m_start = 0; // normalized
  GLfloat m_end = 1; // normalized
  vec2_vector m_vertices;

public:

void build_mesh()
{
  if(m_vao == -1)
  {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
  }

  // simplify n and d
  GLuint g = zxd::gcd(m_n, m_d);
  GLuint n = m_n / g;
  GLuint d = m_d / g;

  GLfloat period = d&1 && n&1 && m_offset == 0 ? (d * fpi) : (d * f2pi);

  m_vertices.clear();
  m_vertices.reserve(m_slice + 1);

  GLfloat k = static_cast<GLfloat>(n) / d;

  GLfloat step_angle = period *(m_end - m_start) / m_slice;
  GLfloat start_angle = m_start * period;
  for (int i = 0; i <= m_slice; ++i) 
  {
    GLfloat theta = start_angle + step_angle * i;
    GLfloat ktheta = k*theta;
    GLfloat r = cos(ktheta) + m_offset;
    GLfloat x = r * cos(theta) * m_radius;
    GLfloat y = r * sin(theta) * m_radius;
    m_vertices.push_back(vec2(x,y));
    //std::cout << m_vertices.back() << std::endl;
  }

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vec2),
      value_ptr(m_vertices[0]), GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

}

void draw()
{
  glBindVertexArray(m_vao);
  glDrawArrays(GL_LINE_STRIP, 0, m_vertices.size());
}

GLuint n() const { return m_n; }
void n(GLuint v){ m_n = v; }

GLuint d() const { return m_d; }
void d(GLuint v){ m_d = v; }

GLuint slice() const { return m_slice; }
void slice(GLuint v){ m_slice = v; }

GLfloat radius() const { return m_radius; }
void radius(GLfloat v){ m_radius = v; }

GLfloat offset() const { return m_offset; }
void offset(GLfloat v){ m_offset = v; }

GLfloat start() const { return m_start; }
void start(GLfloat v){ m_start = v; }

GLfloat end() const { return m_end; }
void end(GLfloat v){ m_end = v; }

const vec2_vector& vertices() const { return m_vertices; }
void vertices(const vec2_vector& v){ m_vertices = v; }

};

class app_name : public app {
protected:
  bitmap_text m_text;
  rose m_rose;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    auto callback = std::bind(std::mem_fn(&app_name::update_rose), this, std::placeholders::_1);
    kci_n = m_control.add_control(GLFW_KEY_Q, 1, 1, 1000, 1, callback);
    kci_d = m_control.add_control(GLFW_KEY_W, 1, 1, 1000, 1, callback);
    kci_slice = m_control.add_control(GLFW_KEY_E, 1024, 1, 1000, 1, callback);
    kci_line_width = m_control.add_control(GLFW_KEY_R, 1, 1, 50, 1, callback);
    kci_start = m_control.add_control(GLFW_KEY_U, 0, 0, 1, 0.01, callback);
    kci_end = m_control.add_control(GLFW_KEY_I, 1, 0, 1, 0.01, callback);
    kci_offset = m_control.add_control(GLFW_KEY_O, 0, -999, 999, 0.1, callback);

    update_rose(0);
  }

  void update_rose(const key_control_item* kci)
  {
    glLineWidth(kci_line_width->value);
    m_rose.radius(300);
    m_rose.slice(128);
    m_rose.n(kci_n->value);
    m_rose.d(kci_d->value);
    m_rose.start(kci_start->value);
    m_rose.end(kci_end->value);
    m_rose.slice(kci_slice->value);
    m_rose.offset(kci_offset->value);
    m_rose.build_mesh();

    const auto& vertices = m_rose.vertices();
    vec2 min_ext = vec2(-500);
    vec2 max_ext = vec2(500);
    for (int i = 0; i < vertices.size(); ++i) {
      const vec2& v = vertices[i];
      if(min_ext.x > v.x) min_ext.x = v.x;
      if(min_ext.y > v.y) min_ext.y = v.y;
      if(max_ext.x < v.x) max_ext.x = v.x;
      if(max_ext.y < v.y) max_ext.y = v.y;
    }
    min_ext *= 1.05;
    max_ext *= 1.05;

    prg.fix2d_camera(min_ext.x, max_ext.x, min_ext.y, max_ext.y);
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat));
    glUniform4f(prg.ul_color, 1,1,1,1);
    m_rose.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : n : " << kci_n->value << std::endl;
    ss << "w : d : " << kci_d->value << std::endl;
    ss << "e : slice : " << kci_slice->value << std::endl;
    ss << "r : line width : " << kci_line_width->value << std::endl;
    ss << "u : start : " << kci_start->value << std::endl;
    ss << "i : end : " << kci_end->value << std::endl;
    ss << "o : offset : " << kci_offset->value << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
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
  zxd::app_name app;
  app.run();
}
