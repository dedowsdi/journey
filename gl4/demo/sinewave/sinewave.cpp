#include "app.h"
#include "bitmaptext.h"
#include "quad.h"
#include "program.h"
#include "texutil.h"
#include <sstream>

namespace zxd {

struct sine_wave_program : public program{
  GLint ul_quad_map;
  GLint ul_time;
  GLint ul_amplitude;
  GLint ul_period;
  GLint ul_wave_period;
  GLint ul_mode;

  GLint al_vertex;
  GLint al_texcoord;

  sine_wave_program() {}

  void attach_shaders(){
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/sinewave.fs.glsl");
  }

  void update_uniforms(GLuint tex_index = 0){
    glUniform1i(ul_quad_map, tex_index);
  }


  virtual void bind_uniform_locations(){
    uniform_location(&ul_quad_map, "quad_map");
    uniform_location(&ul_time, "time");
    uniform_location(&ul_amplitude,  "amplitude");
    uniform_location(&ul_wave_period   ,  "wave_period");
    uniform_location(&ul_period   ,  "period");
    uniform_location(&ul_mode    ,  "mode");
  }

  virtual void bind_attrib_locations(){
    al_vertex = attrib_location("vertex");
    al_texcoord = attrib_location("texcoord");
  }
} prg;


class sine_wave : public app {
protected:
  bitmap_text m_text;
  quad m_quad;
  GLuint m_texture;
  GLfloat m_time;
  GLfloat m_amplitude;
  GLfloat m_period;
  GLfloat m_wave_period;
  GLint m_mode;
  GLboolean m_pause;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "sine wave";
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    prg.init();

    m_quad.build_mesh();
    //m_quad.bind_vntt(prg.al_vertex, -1, prg.al_texcoord, -1);
    m_quad.bind_vao();
    m_quad.bind(0,  prg.al_vertex);
    m_quad.bind(2,  prg.al_texcoord);

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    fipImage image = fipLoadImage("data/texture/bricks2.jpg");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(),
        0, GL_BGR, GL_UNSIGNED_BYTE, image.accessPixels());

    m_time = 0;
    m_amplitude = 0.2;
    m_period = 0.25;
    m_wave_period = 1.5;
    m_mode = 0;
    m_pause = GL_FALSE;
  }

  virtual void update() {
    if(!m_pause)
      m_time += m_delta_time;
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    prg.use();

    glUniform1i(prg.ul_quad_map, 0);
    glUniform1f(prg.ul_time, m_time * 0.25);
    glUniform1f(prg.ul_amplitude, m_amplitude);
    glUniform1f(prg.ul_period, m_period);
    glUniform1f(prg.ul_wave_period, m_wave_period);
    glUniform1i(prg.ul_mode, m_mode);

    m_quad.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : amplitude : " << m_amplitude << std::endl;
    ss << "wW : period : "    << m_period << std::endl;
    ss << "eE : wave_period : "    << m_wave_period << std::endl;
    ss << "r :  mode : "      << m_mode << std::endl;
    ss << "p :  pause : "     << (m_pause == GL_TRUE ? 1 : 0) << std::endl;
    m_text.print(ss.str(), 10, 492);
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

        case GLFW_KEY_Q:
          m_amplitude += 0.02 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_W:
          m_period += 0.02 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_E:
          m_wave_period += 0.02 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_R:
          m_mode ^= 1;
          break;

        case GLFW_KEY_P:
          m_pause = !m_pause;
          break;

        default:
          break;
      }
    }
  }
};
}

int main(int argc, char *argv[]) {
  zxd::sine_wave app;
  app.run();
}
