#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "circle.h"
#include "common_program.h"
#include <memory>
#include "debugger.h"
#include "quad.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

circle circle_geometry;

lightless_program prg;

GLuint fbo;
GLuint fbo_tex;

key_control_item* kci_circles;
key_control_item* kci_k;
key_control_item* kci_resolution;
key_control_item* kci_type;

// translation only, no rotation
class circle_graph
{
protected:
  GLfloat m_angularSpeed = 0;
  GLfloat m_angle = fpi2; // current orbit angle around parent
  GLfloat m_radius;
  vec2 m_pos = vec2(0); // current position
  vec2 m_last_pos = vec2(0);

  std::shared_ptr<circle_graph> m_parent;
  std::shared_ptr<circle_graph> m_child;

public:

  GLfloat angularSpeed() const { return m_angularSpeed; }
  void angularSpeed(GLfloat v){ m_angularSpeed = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v;}

  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }

  const vec2& last_pos() const { return m_last_pos; }
  void last_pos(const vec2& v){ m_last_pos = v; }

  void update()
  {
    if(m_parent)
    {
      m_last_pos = m_pos;
      m_angle += m_angularSpeed;
      GLfloat r = m_radius * (kci_type->value == 0 ? 1 : -1) + m_parent->radius() ;
      pos(m_parent->pos() +  vec2(r * cos(m_angle), r * sin(m_angle)));
    }
  
    if(m_child)
      m_child->update();
  }
  
  circle_graph* get_pen()
  {
    if(!m_child)
      return this;

    return m_child->get_pen();
  }

  //const mat4& scale_mat() const { return m_scale_mat; }
  //void scale_mat(const mat4& v){ m_scale_mat = v; }
  vec3 scale() {return vec3(m_radius); }

  std::shared_ptr<circle_graph> parent() const { return m_parent; }
  void parent(std::shared_ptr<circle_graph> v){ m_parent = v; }

  std::shared_ptr<circle_graph> child() const { return m_child; }
  void child(std::shared_ptr<circle_graph> v){ m_child = v; }

};

class fractal_spirograph_app : public app {
protected:
  bitmap_text m_text;
  std::shared_ptr<circle_graph> m_graph;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "fractal_spirograph_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    circle_geometry.slice(64);
    circle_geometry.type(circle::LINE);
    circle_geometry.build_mesh();

    glGenTextures(1, &fbo_tex);
    glBindTexture(GL_TEXTURE_2D, fbo_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");

    auto callback = std::bind(std::mem_fn(&fractal_spirograph_app::reset_graph),
        this, std::placeholders::_1);
    kci_circles = m_control.add_control(GLFW_KEY_Q, 10, 2, 1000, 1, callback);
    kci_k = m_control.add_control(GLFW_KEY_W, -2, -1000, 1000, 1, callback);
    kci_resolution = m_control.add_control(GLFW_KEY_E, 10, 1, 10000, 1, callback);
    kci_type = m_control.add_control(GLFW_KEY_R, 0, 0, 1, 1, callback);

    reset_graph(0);
  }

  void reset_graph(const key_control_item* kci)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    m_graph = std::shared_ptr<circle_graph>(new circle_graph());
    m_graph->radius(180);
    m_graph->pos(vec2(WIDTH*0.5, HEIGHT*0.5));

    auto graph = m_graph;
    for (int i = 1; i < static_cast<int>(kci_circles->value); ++i) {
      auto child = std::shared_ptr<circle_graph>(new circle_graph);
      child->radius(graph->radius() / 3.0);
      GLfloat r = child->radius() * (kci_type->value == 0 ? 1 : -1) + graph->radius() ;
      child->pos(graph->pos() +  vec2(0, r));
      child->last_pos(child->pos());
      graph->child(child);
      child->parent(graph);
      child->angularSpeed(glm::radians(glm::pow(kci_k->value, i)) / kci_resolution->value);
      graph = child;
    }
  }

  virtual void update() {
    // draw trace in a different texture, no clear
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    auto pen = m_graph->get_pen();

    for (int i = 0; i < kci_resolution->value; ++i) {
      m_graph->update();
      debugger::draw_line(pen->last_pos(), pen->pos(), prg.vp_mat, 1, vec4(1,0,1,1));
    }
  }

  virtual void display() {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    // draw trace first
    glBindTexture(GL_TEXTURE_2D, fbo_tex);
    draw_quad(0);

    // draw circle
    prg.use();
    glUniform4f(prg.ul_color, 1,1,1,1);
    std::shared_ptr<circle_graph> graph = m_graph;
    while(graph)
    {
      //std::cout << graph->pos() << std::endl;
      mat4 mvp_mat = glm::scale(glm::translate(prg.vp_mat, vec3(graph->pos(), 0)), graph->scale());
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
      circle_geometry.draw();
      graph = graph->child();
    }
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : circles : " << kci_circles->value << std::endl;
    ss << "wW : k : " << kci_k->value << std::endl;
    ss << "eE : resolutions : " << kci_resolution->value << std::endl;
    ss << "rR : type : " << kci_type->value << std::endl;
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
  zxd::fractal_spirograph_app app;
  app.run();
}
