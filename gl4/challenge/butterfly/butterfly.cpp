#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

lightless_program prg;
kcip color;

glm::mat4 v_mat;
glm::mat4 p_mat;

class butterfly
{
protected:
  GLuint m_vao = -1;
  GLuint m_vbo = -1;
  GLuint m_slice = 32;
  GLfloat m_radius = 1;
  vec2_vector m_vertices;

public:

void build_mesh()
{
  if(m_slice & 1)
    m_slice += 1;

  if(m_vao == -1)
  {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, (m_slice + 2) * sizeof(vec2), 0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  update_mesh();
}

void update_mesh()
{
  static GLfloat yoff = 0;
  // simplify n and d

  m_vertices.clear();
  m_vertices.reserve(m_slice + 2);

  int half_slice = m_slice / 2;
  GLfloat step_angle = fpi / half_slice;
  // create 1st half, left, bottom
  GLfloat xoff = 0;
  for (int i = 0; i <= half_slice; ++i) 
  {
    GLfloat theta = fpi4 + step_angle * i;
    GLfloat ktheta = theta*2;
    //GLfloat r = cos(ktheta) * m_radius;
    GLfloat r = cos(ktheta) * glm::mix(m_radius * 0.5f, m_radius , glm::perlin(vec2(xoff, yoff)));
    GLfloat x = r * cos(theta);
    GLfloat y = r * sin(theta);
    m_vertices.push_back(vec2(x,y));
    xoff += 0.01;
  }

  // reflect along y=-x
  for (int i = 0; i <= half_slice; ++i) 
  {
    const vec2& vertex = m_vertices[i];
    m_vertices.push_back(-vec2(vertex.y, vertex.x));
  }
  yoff += 0.001;

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertices.size() * sizeof(vec2), glm::value_ptr(m_vertices.front()));
}

void draw()
{
  glBindVertexArray(m_vao);
  glDrawArrays(GL_LINE_STRIP, 0, m_vertices.size());
}


GLuint slice() const { return m_slice; }
void slice(GLuint v){ m_slice = v; }

GLfloat radius() const { return m_radius; }
void radius(GLfloat v){ m_radius = v; }

};

class butterfly_app : public app
{
protected:
  butterfly m_butterfly;
  mat4 m_mat;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "butterfly_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    prg.init();
    p_mat = glm::ortho<GLfloat>(0, WIDTH, 0, HEIGHT);

    m_butterfly.radius(300);
    m_butterfly.slice(1024);
    m_butterfly.build_mesh();

    color = m_control.add(GLFW_KEY_Q, vec4(1), vec4(0), vec4(1), vec4(0.1));
  }

  virtual void update()
  {
    m_butterfly.update_mesh();
    // fake slap
    m_mat = glm::scale(vec3(glm::cos(m_current_time), 1, 1));
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    //mat4 mvp_mat = prg.vp_mat * glm::translate(vec3(WIDTH * 0.5f, HEIGHT * 0.5f, 0));
    mat4 mvp_mat = p_mat * v_mat * glm::translate(vec3(WIDTH * 0.5f, HEIGHT * 0.5f, 0)) * m_mat
    * glm::rotate(-fpi4, pza);
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(color->get<vec4>()));

    m_butterfly.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : color : " << color->get<vec4>() << std::endl;
    ss << "index : " << m_control.index() << std::endl;
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
  zxd::butterfly_app app;
  app.run();
}
