#include <sstream>
#include <memory>

#include "app.h"
#include "bitmaptext.h"
#include "circle.h"
#include "common_program.h"
#include "debugger.h"
#include "quad.h"
#include "rose.h"
#include "lissajous.h"
#include "fractal_spiral.h"

namespace zxd {

lightless_program prg;

GLuint fbo;
GLuint fbo_tex;
GLuint tex;

GLfloat angular_scale = -2;
GLfloat radius_scale = 0.33;
GLfloat origin_scale = 1;

bool display_pen = true;
bool display_help = true;
kcip kci_circle_index;
kcip kci_angular_scale;
kcip kci_radius_scale;
kcip kci_origin_scale;
kcip kci_resolution;
kcip kci_rose_n;
kcip kci_rose_d;
kcip kci_rose_offset;
kcip kci_lissa_type;
kcip kci_xscale;
kcip kci_yscale;
kcip kci_pen_width;

int circle_index = 1;
int circle_type;

vec4 current_circle_color = vec4(1, 0, 0, 1);
vec4 circle_color = vec4(1);
GLfloat pen_width = 2;

// translation only, no rotation

class fractal_spirograph_app : public app {
protected:
  bitmap_text m_text;
  std::shared_ptr<spiral_seed> m_graph;
  spiral_seed* m_current_graph;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "fractal_spirograph_app";
    m_info.wnd_width = 720;
    m_info.wnd_height = 720;
    m_info.wm.x = 100;
    m_info.wm.y = 100;
    //m_info.fullscreen = true;
    m_info.samples = 16;
  }

  virtual void resize_buffer()
  {
    if(!glIsTexture(fbo_tex))
      glGenTextures(1, &fbo_tex);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, fbo_tex);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_info.samples, GL_RGB,
        m_info.wnd_width, m_info.wnd_height, GL_TRUE);

    if(!glIsTexture(tex))
      glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_info.wnd_width, m_info.wnd_height,
        0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    if(!glIsFramebuffer(fbo))
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, fbo_tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();

    auto callback = std::bind(std::mem_fn(&fractal_spirograph_app::reset_current_graph),
        this, std::placeholders::_1);
    auto index_callback = std::bind(std::mem_fn(&fractal_spirograph_app::rotate_index),
        this, std::placeholders::_1);

    kci_circle_index = m_control.add_control(GLFW_KEY_Q, circle_index, 0, 1000, 1, index_callback);
    kci_radius_scale = m_control.add_control<GLfloat>(GLFW_KEY_W, radius_scale, 0.01, 100, 0.1, callback);
    kci_origin_scale = m_control.add_control<GLfloat>(GLFW_KEY_E, origin_scale, -100, 100, 1, callback);
    kci_angular_scale = m_control.add_control<GLfloat>(GLFW_KEY_R, angular_scale, -1000, 1000, 1, callback);
    kci_resolution = m_control.add_control(GLFW_KEY_U, 10, 1, 10000, 1,
        [this](auto* kci){this->restart();});
    kci_rose_n = m_control.add_control(GLFW_KEY_J, 1, 1, 1000, 1, callback);
    kci_rose_d = m_control.add_control(GLFW_KEY_K, 1, 1, 1000, 1, callback);
    kci_rose_offset = m_control.add_control<GLfloat>(GLFW_KEY_L, 0, -1000, 1000, 0.1, callback);
    kci_lissa_type = m_control.add_control(GLFW_KEY_H, 0, 0, 1, 1, callback);
    kci_xscale = m_control.add_control<GLint>(GLFW_KEY_N, 1, 1, 100, 1, callback);
    kci_yscale = m_control.add_control<GLint>(GLFW_KEY_M, 1, 1, 100, 1, callback);

    resize_buffer();
    reset_graph();
  }

  void rotate_index(const kci* kci = 0)
  {
    int size = m_graph->size();
    select_graph(kci_circle_index->get_int() % size);
  }

  void select_graph(GLuint index)
  {
    circle_index = index;
    update_current_graph_value();
  }

  void update_current_graph_value()
  {
    m_current_graph = m_graph->get_child_at(circle_index);
    kci_radius_scale->set(m_current_graph->radius_scale());
    kci_origin_scale->set(m_current_graph->origin_scale());
    kci_angular_scale->set(m_current_graph->angular_scale());
    kci_rose_n->set<GLint>(m_current_graph->rose_n());
    kci_rose_d->set<GLint>(m_current_graph->rose_d());
    kci_rose_offset->set(m_current_graph->rose_offset());
    kci_lissa_type->set<GLint>(m_current_graph->type());
    kci_xscale->set<GLint>(m_current_graph->xscale());
    kci_yscale->set<GLint>(m_current_graph->yscale());
  }

  void remove_graph()
  {
    if(m_graph->size() > 2)
      m_graph->remove_pen();

    select_graph(m_graph->size() - 1);
    restart();
  }

  void add_graph()
  {
    auto pen = m_graph->get_pen();
    pen->add_child();

    select_graph(m_graph->size() - 1);

    restart();
  }

  void restart(const kci* kci = 0)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);
    m_graph->reset();

    auto hh = m_graph->approximate_height(180) * 1.05f;
    prg.p_mat = zxd::rect_ortho(hh, hh, wnd_aspect());
  }

  void reset_graph()
  {
    m_graph = std::make_shared<spiral_seed>();
    m_graph->angular_scale(-2);
    m_graph->origin_scale(1);
    m_graph->radius_scale(0.33);
    m_graph->angular_speed(0.1);
    m_graph->radius(100);
    m_graph->rose_n(3);
    m_graph->rose_d(1);
    m_graph->rose_offset(0);
    m_graph->type(lissajous::LT_ROSE);
    m_graph->xscale(1);
    m_graph->yscale(1);
    m_graph->add_child();

    select_graph(1);

    restart();
  }

  void reset_current_graph(const kci* kci)
  {
    m_current_graph->angular_scale(kci_angular_scale->get_float());
    m_current_graph->radius_scale(kci_radius_scale->get_float());
    m_current_graph->origin_scale(kci_origin_scale->get_float());
    m_current_graph->type(static_cast<lissajous::LISSA_TYPE>(kci_lissa_type->get_int()));
    m_current_graph->rose_n(kci_rose_n->get_int());
    m_current_graph->rose_d(kci_rose_d->get_int());
    m_current_graph->rose_offset(kci_rose_offset->get_float());
    m_current_graph->xscale(kci_xscale->get_int());
    m_current_graph->yscale(kci_yscale->get_int());

    restart();
  }

  virtual void update() {
    // draw trace in a different texture, no clear
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    auto pen = m_graph->get_pen();

    glEnable(GL_BLEND);
    glBlendColor(0, 0, 0, 0.5);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

    GLuint resolution = kci_resolution->get_int();
    vec2_vector lines;
    lines.reserve(resolution + 1);
    lines.push_back(pen->pos());
    for (int i = 0; i < resolution; ++i) {
      m_graph->update(kci_resolution->get_int());
      lines.push_back(pen->pos());
    }
    debugger::draw_line(GL_LINE_STRIP, lines, prg.p_mat, pen_width, vec4(1,0,1,1));
    glDisable(GL_BLEND);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, m_info.wnd_width, m_info.wnd_height, 0, 0, m_info.wnd_width, m_info.wnd_height, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  }

  virtual void display() {

    //glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glLineWidth(1);
    // draw circle
    prg.use();
    std::shared_ptr<spiral_seed> graph = m_graph;
    if(display_pen)
    {
      int i = 0;
      while(graph)
      {
        //std::cout << graph->pos() << std::endl;
        mat4 mvp_mat = glm::rotate(glm::translate(prg.p_mat, vec3(graph->pos(), 0)),
            graph->rotate_angle(), pza);
        glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
        const vec4& color = i++ == circle_index ? current_circle_color : circle_color;
        glUniform4fv(prg.ul_color, 1, glm::value_ptr(color));
        graph->debug_draw();
        if(graph->parent())
        {
          debugger::draw_point(graph->parent()->lisa().get_at_angle(graph->angle()),
              prg.p_mat, 10, vec4(0,1,0,1));
        }
        graph = graph->child();
      }
    }

    if(!display_help)
      return;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "num circles :  " << m_graph->size() << std::endl;
    ss << "qQ : circle index : " << circle_index << std::endl;
    ss << "wW : radius scale : " << m_current_graph->radius_scale() << std::endl;
    ss << "eE : origin_scale : " << m_current_graph->origin_scale() << std::endl;
    ss << "rR : angular_scale : " << m_current_graph->angular_scale() << std::endl;
    ss << "uU : resolutions : " << kci_resolution->get_int() << std::endl;
    ss << "iI : add remove circle : " << std::endl;
    ss << "jJ : rose_n : " <<kci_rose_n->get_int() << std::endl;
    ss << "kK : rose_d : " <<kci_rose_d->get_int() << std::endl;
    ss << "lL : rose_offset : " <<kci_rose_offset->get_float() << std::endl;
    ss << "hH: lissa type : " <<kci_lissa_type->get_int() << std::endl;
    ss << "nN : xscale : " <<kci_xscale->get_int() << std::endl;
    ss << "mM : yscale : " <<kci_yscale->get_int() << std::endl;

    ss << "p : reset : " << std::endl;
    ss << "z : toggle help : " << std::endl;
    ss << "x : restart : " << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    resize_buffer();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
  }

  virtual void glfw_key(
      GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;

        case GLFW_KEY_I:
          if(mods & GLFW_MOD_SHIFT)
            remove_graph();
          else
            add_graph();
          break;

        case GLFW_KEY_Z:
          display_pen ^= 1;
          display_help ^= 1;
          break;

        case GLFW_KEY_X:
          restart();
          break;

        case GLFW_KEY_P:
          reset_graph();
          break;

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
