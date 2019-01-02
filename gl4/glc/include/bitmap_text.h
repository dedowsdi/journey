#ifndef GL_GLC_BITMAP_TEXT_H
#define GL_GLC_BITMAP_TEXT_H

#include "gl.h"
#include <string>
#include "glm.h"
#include "program.h"
#include <memory>
#include <map>

namespace zxd {

struct bitmap_text_program : public zxd::program {
  GLint ul_text_color;
  GLint ul_font_map;
  bitmap_text_program() {}
  void reshape(GLuint wnd_width, GLuint wnd_height) {
    mvp_mat = glm::ortho(
      0.0f, (GLfloat)wnd_width, 0.0f, (GLfloat)wnd_height, -1.0f, 1.0f);
  }
  void update_uniforms(const glm::vec4& text_color) {
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
    glUniform4fv(ul_text_color, 1, value_ptr(text_color));
    glUniform1i(ul_font_map, 0);
  }
  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "shader4/bitmap_text.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/bitmap_text.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_text_color, "text_color");
    uniform_location(&ul_font_map, "font_map");
  }
  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
  }
};

// only works with grayscale bitmap
class bitmap_text {
public:
  struct glyph {
    GLfloat x_min;  // the same as bearying X in freetype glyph
    GLfloat y_min;  // bearyingY - height
    GLfloat x_max;  // x_min + width
    GLfloat y_max;  // the same as bearying Y
    GLfloat s_min;  // texcoord
    GLfloat t_min;  //
    GLfloat s_max;  //
    GLfloat t_max;  //
    GLfloat x_origin;
    GLfloat y_origin;
    GLuint advance;  // in pixel
  };

protected:
  std::string m_face;
  std::string m_fmt_file;
  std::shared_ptr<bitmap_text_program> m_program;
  GLuint m_vao;
  GLuint m_vbo;
  GLuint m_texture_size;
  GLuint m_texture;
  GLuint m_num_characters;
  GLuint m_height;
  GLuint m_linespace;
  GLuint m_max_advance;

  std::map<GLchar, glyph> m_glyph_dict;

public:
  bitmap_text(const std::string& fmtfile = "font/DejaVuSansMono_15.fmt");
  ~bitmap_text();
  void init();
  // must be called at least once
  void reshape(GLuint wnd_width, GLuint wnd_height) {
    m_program->reshape(wnd_width, wnd_height);
  }

  void print(const std::string& text, GLuint x, GLuint y,
    const glm::vec4& color = vec4(1.0f), GLfloat scale = 1.0);
};
}

#endif /* GL_GLC_BITMAP_TEXT_H */
