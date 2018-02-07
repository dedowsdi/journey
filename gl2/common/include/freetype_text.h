#ifndef TEXT_H
#define TEXT_H

#include "glad/glad.h"
#include <string>
#include "glm.h"
#include "program.h"
#include <memory>
#include <map>

namespace zxd {

struct freetype_text_program : public zxd::program {
  GLint al_vertex;
  GLint ul_text_color;
  GLint ul_font_map;
  freetype_text_program() {
  }
  void reshape(GLuint wnd_width, GLuint wnd_height) {
    mvp_mat = glm::ortho(
      0.0f, (GLfloat)wnd_width, 0.0f, (GLfloat)wnd_height, -1.0f, 1.0f);
  }
  void update_uniforms(const glm::vec4& text_color) {
    glUniformMatrix4fv(
      ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
    glUniform4fv(ul_text_color, 1, value_ptr(text_color));
    glUniform1i(ul_font_map, 0);
  }
  virtual void attach_shaders() {
    attach_shader_file(GL_VERTEX_SHADER, "data/shader/freetype_text.vs.glsl");
    attach_shader_file(GL_FRAGMENT_SHADER, "data/shader/freetype_text.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_text_color, "text_color");
    uniform_location(&ul_font_map, "font_map");
  }
  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
  }
};

// only works with grayscale bitmap
class freetype_text {
public:
  struct Glyph {
    GLfloat x_min;  // the same as bearying X
    GLfloat y_min;  // bearyingY - height
    GLfloat x_max;  // x_min + width
    GLfloat y_max;  // the same as bearying Y
    GLfloat bearingX;
    GLfloat bearingY;
    GLfloat width;   // width of bounding box
    GLfloat height;  // height of bounding box, bitmap.rows
    GLfloat origin;  // not used
    GLuint advance;  // in pixel
    GLuint texture;
  };

protected:
  GLuint m_num_characters;
  GLuint m_vao;
  GLuint m_vbo;
  GLuint m_texture;
  GLuint m_height;     // width will be deduced from height by freetype
  GLuint m_linespace;  // max height, in pixel
  GLuint m_max_advance; // in pixel

  std::string m_face;
  std::shared_ptr<freetype_text_program> m_program;
  std::map<GLchar, Glyph> m_glyph_dict;

public:
  freetype_text(const std::string& font);
  ~freetype_text();
  void init();

  void update_glyph_dict();

  // must be called at least once
  void reshape(GLuint wnd_width, GLuint wnd_height) {
    m_program->reshape(wnd_width, wnd_height);
  }

  // linespace = height of 'I' + 2 if it's 0
  void print(const std::string& text, GLuint x, GLuint y,
    const glm::vec4& color = vec4(1.0f), GLfloat scale = 1.0);

  void print(char c, GLuint x, GLuint y, const glm::vec4& color = vec4(1.0f),
    GLfloat scale = 1.0);

  void print(const Glyph& glyph, GLuint x, GLuint y,
    const glm::vec4& color = vec4(1.0f), GLfloat scale = 1.0);

  GLuint get_num_characters() const { return m_num_characters; }
  void set_num_characters(GLuint v) { m_num_characters = v; }

  GLuint get_height() const { return m_height; }
  void set_height(GLuint v) { m_height = v; }

  const std::string& get_face() const { return m_face; }
  void set_face( const std::string& v){m_face = v;}

  GLuint get_linespace() const { return m_linespace; }
  void set_linespace(GLuint v) { m_linespace = v; }

  GLuint get_max_advance() const { return m_max_advance; }
  void set_max_advance(GLuint v) { m_max_advance = v; }

  const std::map<GLchar, Glyph>& get_glyph_dict() { return m_glyph_dict; }
  const Glyph& getGlyph(char c) { return m_glyph_dict[c]; }
};
}

#endif /* TEXT_H */
