#include "glad/glad.h"
#include "app.h"
#include "program.h"
#include "texutil.h"

namespace zxd {

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512

GLuint bitmap_tex;
vec4 text_color(1.0f);
GLuint vao;
GLuint vbo;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "bitmap_text";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGBA;
    m_info.wnd_width = WINDOWS_WIDTH;
    m_info.wnd_height = WINDOWS_HEIGHT;
    m_info.samples = 16;
  }

  struct bitmap_text_program : public zxd::program {
    GLint ul_text_color;
    GLint ul_font_map;
    bitmap_text_program() {
      mvp_mat = glm::ortho(0.0f, (GLfloat)WINDOWS_WIDTH, 0.0f,
        (GLfloat)WINDOWS_HEIGHT, -1.0f, 1.0f);
    }
    virtual void update_frame() {
      glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
      glUniform4fv(ul_text_color, 1, value_ptr(text_color));
      glUniform1i(ul_font_map, 0);
    }
    virtual void model(const glm::mat4& m_mat) {}
    virtual void attach_shaders() {
      attach(GL_VERTEX_SHADER, "shader2/bitmap_text.vs.glsl");
      attach(GL_FRAGMENT_SHADER, "shader2/bitmap_text.fs.glsl");
    }
    virtual void bind_uniform_locations() {
      uniform_location(&ul_mvp_mat, "mvp_mat");
      uniform_location(&ul_text_color, "text_color");
      uniform_location(&ul_font_map, "font_map");
    }
    virtual void bind_attrib_locations() {
      bind_attrib_location(0, "vertex");
    }
  } prg;

  // assume 16 * 16 characters in texture
  void printText2D(const std::string& text, GLuint x, GLuint y, GLuint size) {
    prg.update_frame();
    zxd::vec4_vector vertices;  // {x, y, s, t}

    float tex_step = 1 / 16.0f;
    GLuint nextY = y;
    GLuint nextX = x;

    // collect character quads
    for (int i = 0; i < text.size(); ++i) {
      char character = text[i];

      if (character == '\n') {
        nextX = x;
        nextY -= size;
        continue;
      }

      vec2 vertex0(nextX, nextY);
      vec2 texcoord0(
        (character & 0xf) * tex_step, (15 - character / 16) * tex_step);

      // clang-format off
    vertices.push_back(vec4(vertex0, texcoord0));
    vertices.push_back(
      vec4(vertex0 + vec2(size, 0),    texcoord0 + vec2(tex_step, 0)));
    vertices.push_back(
      vec4(vertex0 + vec2(size, size), texcoord0 + vec2(tex_step, tex_step)));
    vertices.push_back(
      vec4(vertex0 + vec2(0,    size), texcoord0 + vec2(0,       tex_step)));
      // clang-format on

      nextX += size;
    }

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices),
      value_ptr(vertices[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(glm::vec4), BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glBindTexture(GL_TEXTURE_2D, bitmap_tex);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    glDrawArrays(GL_QUADS, 0, vertices.size());
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    prg.update_frame();
    printText2D("aaaaa\nbADSF\ncsdfsdfs\nqADFDFS", 10, 400, 32);

    glutSwapBuffers();
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    prg.init();

    fipImage fip = zxd::fipLoadImage("font/Holstein.png");
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &bitmap_tex);
    glBindTexture(GL_TEXTURE_2D, bitmap_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fip.getWidth(), fip.getHeight(), 0,
      GL_RED, GL_UNSIGNED_BYTE, fip.accessPixels());

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  }

  void mouse(int button, int state, int x, int y) {
    switch (button) {
      default:
        break;
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
