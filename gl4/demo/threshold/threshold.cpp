#include "app.h"
#include "bitmaptext.h"
#include "quad.h"
#include "program.h"
#include "texutil.h"
#include <sstream>

namespace zxd {

struct threshold_program : public program{
  GLint ul_quad_map;
  GLint ul_threshold;

  threshold_program() {}

  void attach_shaders(){
    attach(GL_VERTEX_SHADER, "shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/threshold.fs.glsl");
  }

  void update_uniforms(GLuint tex_index = 0){
    glUniform1i(ul_quad_map, tex_index);
  }


  virtual void bind_uniform_locations(){
    uniform_location(&ul_quad_map, "quad_map");
    uniform_location(&ul_threshold, "threshold");
  }

  virtual void bind_attrib_locations(){
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }
} prg;


class threshold : public app {
protected:
  bitmap_text m_text;
  quad m_quad;
  GLuint m_texture;
  vec3 m_threshold;

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

    m_quad.include_texcoord(true);
    m_quad.build_mesh();
    m_quad.bind_vao();

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    fipImage image = fipLoadResource("texture/bricks2.jpg");
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(),
        0, GL_BGR, GL_UNSIGNED_BYTE, image.accessPixels());

    m_threshold = vec3(0.15, 0.15, 0.15);
  }

  virtual void update() {
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    prg.use();

    glUniform1i(prg.ul_quad_map, 0);
    glUniform3fv(prg.ul_threshold, 1, glm::value_ptr(m_threshold));

    m_quad.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : red : " << m_threshold.x << std::endl;
    ss << "wW : green : "    << m_threshold.y << std::endl;
    ss << "eE : blue : "    << m_threshold.z << std::endl;
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
          m_threshold.x += 0.02 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_W:
          m_threshold.y += 0.02 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        case GLFW_KEY_E:
          m_threshold.z += 0.02 * (mods & GLFW_MOD_SHIFT ? -1 : 1);
          break;

        default:
          break;
      }
    }
  }
};
}

int main(int argc, char *argv[]) {
  zxd::threshold app;
  app.run();
}
