#include "bitmaptext.h"
#include "texutil.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "common.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace zxd {
//--------------------------------------------------------------------
bitmap_text::bitmap_text(
  const std::string& fmtfile /* = "DejaVuSansMono_15_9.fmt"*/)
    : m_program(new bitmap_text_program()), m_fmt_file(fmtfile) {}

//--------------------------------------------------------------------
bitmap_text::~bitmap_text() {}

//--------------------------------------------------------------------
void bitmap_text::init() {
  m_program->init();
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  // load font format information
  std::string fmtfile = m_fmt_file;
  std::string texfile;
  std::ifstream ifs(fmtfile);

  if (!ifs) {
    std::stringstream ss;
    ss << "faield to load file " << fmtfile << std::endl;
    throw std::runtime_error(ss.str());
  }

  // read texture size

  std::string line;

  // 0 read file, 1 read metrics, 2 read characters
  int stage = 0;

  while (getline(ifs, line)) {
    if (line.empty() || line[0] == '#' || line[0] == '-') {
      continue;
    }

    std::stringstream ss(line);

    if (stage == 0) {
      ss >> texfile >> m_face;
      ++stage;
    } else if (stage == 1) {
      ss >> m_num_characters >> m_texture_size >> m_height >> m_linespace >>
        m_max_advance;
      ++stage;
    } else if (stage == 2) {
      glyph glyph;
      GLint c;
      ss >> c;
      ss >> glyph.x_min >> glyph.x_max >> glyph.y_min >> glyph.y_max;
      ss >> glyph.s_min >> glyph.s_max >> glyph.t_min >> glyph.t_max;
      ss >> glyph.advance;

      if (ss.fail()) {
        std::stringstream es;
        es << "find illegal data : " << line << std::endl;
        throw std::runtime_error(es.str());
      }

      glyph.s_min /= m_texture_size;
      glyph.t_min /= m_texture_size;
      glyph.s_max /= m_texture_size;
      glyph.t_max /= m_texture_size;

      m_glyph_dict.insert(std::make_pair(c, glyph));
    }
  }

  if (m_glyph_dict.size() != m_num_characters) {
    std::stringstream ss;
    ss << "load only " << m_glyph_dict.size()
       << " character info, there should be " << m_num_characters << std::endl;
    throw std::runtime_error(ss.str());
  }

  fipImage fip = zxd::fipLoadImage(texfile);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &m_texture);
  glBindTexture(GL_TEXTURE_2D, m_texture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fip.getWidth(), fip.getHeight(), 0,
    GL_RED, GL_UNSIGNED_BYTE, fip.accessPixels());
}

//--------------------------------------------------------------------
void bitmap_text::print(const std::string& text, GLuint x, GLuint y,
  const glm::vec4& color /*= vec4(1.0f)*/, GLfloat scale /*= 1.0f*/) {
  zxd::vec4_vector vertices;  // {x, y, s, t}

  GLuint nexty = y;
  GLuint nextx = x;

  // collect character quads
  for (int i = 0; i < text.size(); ++i) {
    char character = text[i];

    if (character == '\n') {
      nextx = x;
      nexty -= m_linespace * scale;
      continue;
    }

    const glyph& glyph = m_glyph_dict[character];

    vec2 vertex0(nextx + glyph.x_min * scale, nexty + glyph.y_min * scale);
    vec2 texcoord0(glyph.s_min, glyph.t_min);

    vec2 vertex1(nextx + glyph.x_max * scale, nexty + glyph.y_min * scale);
    vec2 texcoord1(glyph.s_max, glyph.t_min);

    vec2 vertex2(nextx + glyph.x_max * scale, nexty + glyph.y_max * scale);
    vec2 texcoord2(glyph.s_max, glyph.t_max);

    vec2 vertex3(nextx + glyph.x_min * scale, nexty + glyph.y_max * scale);
    vec2 texcoord3(glyph.s_min, glyph.t_max);

    // std::cout << vertex0 << " : " << texcoord0 << std::endl;
    // std::cout << vertex1 << " : " << texcoord1 << std::endl;
    // std::cout << vertex2 << " : " << texcoord2 << std::endl;
    // std::cout << vertex3 << " : " << texcoord3 << std::endl << std::endl;

    // clang-format off
    vertices.push_back(vec4(vertex0, texcoord0));
    vertices.push_back(vec4(vertex1, texcoord1));
    vertices.push_back(vec4(vertex2, texcoord2));

    vertices.push_back(vec4(vertex0, texcoord0));
    vertices.push_back(vec4(vertex2, texcoord2));
    vertices.push_back(vec4(vertex3, texcoord3));

    nextx += glyph.advance * scale;
  }

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec4),
    value_ptr(vertices[0]), GL_STREAM_DRAW);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  glUseProgram(*m_program);
  m_program->update_uniforms(color);

  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
}
