// https://web.archive.org/web/20160116150939/http://freespace.virgin.net/hugo.elias/graphics/x_water.htm
#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "common_program.h"
#include "quad.h"
#include "pingpong.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

quad q;

// need to access 2 buffers and right to the other one
npingpong tex;
GLuint fbo;
GLuint current = 2; // current rtt

key_control_item* kci_dampen;
key_control_item* kci_ripple_start;
key_control_item* kci_ripple_frame;

GLuint createTexture()
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  // need float texture, as ripple start might be greater than 1
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  return tex;
}

class ripple_program : public program
{
public:
  GLint ul_buffer0;
  GLint ul_buffer1;
  GLint ul_dampen;

  void attach_shaders() override
  {
    attach(GL_VERTEX_SHADER, "shader/ripple.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/ripple.fs.glsl");
  }

  void bind_uniform_locations() override
  {
    uniform_location(&ul_buffer0, "buffer0");
    uniform_location(&ul_buffer1, "buffer1");
    uniform_location(&ul_dampen, "dampen");
  }

  void bind_attrib_locations() override
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

}prg;

class ripple_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "ripple_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    q.include_texcoord(true);
    q.build_mesh();

    prg.init();
    prg.v_mat = prg.p_mat = prg.vp_mat = mat4(1);


    tex.add_resource(createTexture());
    tex.add_resource(createTexture());
    tex.add_resource(createTexture());

    glGenFramebuffers(1, &fbo);

    //glBindTexture(GL_TEXTURE_2D, tex[0]);
    //glTexSubImage2D(GL_TEXTURE_2D, 0, 400, 400, 1, 1, GL_RGB, GL_FLOAT, glm::value_ptr(p));

    current = 1;

    kci_dampen = m_control.add_control(GLFW_KEY_Q, 0.99, 0, 10, 0.01);
    kci_ripple_start = m_control.add_control(GLFW_KEY_W, 1, 0, 500, 1);
    kci_ripple_frame = m_control.add_control(GLFW_KEY_E, 2, 1, 60, 1);
  }

  virtual void update() {
    GLint f = kci_ripple_frame->value;
    if((m_frame_number % f) == 0)
      tex.shift();

    createRipple(glm::linearRand(ivec2(0, 0), ivec2(WIDTH - 1, HEIGHT - 1)));
  }

  void createRipple(ivec2(pos))
  {
    vec3 p(kci_ripple_start->value);
    glBindTexture(GL_TEXTURE_2D, tex.first_ping());
    glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, 1, 1, GL_RGB, GL_FLOAT, glm::value_ptr(p));
  }

  virtual void display() {

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.pong(), 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.first_ping());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex.last_ping());
    prg.use();
    glUniform1i(prg.ul_buffer0, 0);
    glUniform1i(prg.ul_buffer1, 1);
    glUniform1f(prg.ul_dampen, kci_dampen->value);
    q.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //glClear(GL_COLOR_BUFFER_BIT);
    
    draw_quad(tex.pong(), 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << " fps : " << m_fps << std::endl;
    ss << " qQ : dampen : " << kci_dampen->value << std::endl;
    ss << " wW : ripple start : " << kci_ripple_start->value << std::endl;
    ss << " eE : ripple frame : " << kci_ripple_frame->value << std::endl;
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

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y)
  {
    app::glfw_mouse_move(wnd, x, y);
    createRipple(glfw_to_gl(vec2(x, y)));
  }
};
}

int main(int argc, char *argv[]) {
  zxd::ripple_app app;
  app.run();
}
