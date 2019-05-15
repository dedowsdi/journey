/*
 * stars fly to user.
 */
#include "app.h"
#include "texutil.h"
#include "bitmap_text.h"
#include "glm.h"
#include <sstream>

#define WIDTH 800
#define HEIGHT 800
#define NUM_STARS 10000

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd
{

struct star_program : public zxd::program {
  GLint ul_diffuse_map;

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/starfield.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/starfield.fs.glsl");
  }

  virtual void bind_uniform_locations()
  {
    uniform_location(&ul_diffuse_map, "diffuse_map");
  }

  virtual void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
  }
} prg;


class Star
{
public:
  vec3 m_pos;
  GLfloat m_speed;
  Star()
  {
    reset();
  }

  void update(GLfloat dt)
  {
    m_pos.z -= dt * m_speed;
    if(m_pos.z <= 0)
    {
      reset();
    }
  }

  const vec3& pos() const { return m_pos; }
  void pos(const vec3& v){ m_pos = v; }

  void reset()
  {
    m_pos.x = glm::linearRand(-1.0, 1.0);
    m_pos.y = glm::linearRand(-1.0, 1.0);
    m_pos.z = 1;
    m_speed = linearRand(0.01f, 2.00f);
  }

};

using Stars = std::vector<Star>;

class Starfield : public app
{
protected:

  bitmap_text m_text;
  GLuint m_diffuse_map;
  GLuint m_vao;
  GLuint m_vbo;
  vec3_vector m_vertices;
  Stars m_stars;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "star field";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {

    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    glGenTextures(1, &m_diffuse_map);
    glBindTexture(GL_TEXTURE_2D, m_diffuse_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    fipImage img = zxd::fipLoadResource("texture/flare.png");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img.getWidth(), img.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, img.accessPixels());

    m_stars.resize(NUM_STARS);
    m_vertices.resize(NUM_STARS);

    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3),
      value_ptr(m_vertices[0]), GL_STREAM_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    prg.init();
  }

  virtual void update() {
    for (int i = 0; i < m_stars.size(); ++i) {
      m_stars[i].update(m_delta_time);
    }
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_diffuse_map);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

    prg.use();
    glEnable(GL_PROGRAM_POINT_SIZE);
    glUniform1i(prg.ul_diffuse_map, 0);

    // update vertex buffer
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    for (int i = 0; i < m_stars.size(); ++i)
      m_vertices[i] = m_stars[i].pos();

    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(glm::vec3),
      value_ptr(m_vertices[0]), GL_STREAM_DRAW);

    glDrawArrays(GL_POINTS, 0, m_vertices.size());

    glDisable(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << m_fps;
    m_text.print(ss.str(), 10, 780);
    glDisable(GL_BLEND);

  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(w, h);
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
  }

};

}

int main(int argc, char *argv[])
{
  zxd::Starfield app;
  app.run();
  return 0;
}
