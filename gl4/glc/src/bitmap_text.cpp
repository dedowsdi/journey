#include <bitmap_text.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include <ft2build.h>
#include FT_FREETYPE_H
#include <glm/gtc/matrix_transform.hpp>
#include "utf8.h"
#include <texutil.h>
#include <common.h>
#include <stream_util.h>
#include <glc_utf8.h>

namespace zxd
{

void bitmap_text_program::reshape(GLuint wnd_width, GLuint wnd_height)
{
  mvp_mat = glm::ortho(
      0.0f, (GLfloat)wnd_width, 0.0f, (GLfloat)wnd_height, -1.0f, 1.0f);
}

void bitmap_text_program::update_uniforms(const glm::vec4& text_color)
{
  glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  glUniform4fv(ul_text_color, 1, value_ptr(text_color));
  glUniform1i(ul_font_map, 0);
}


//--------------------------------------------------------------------
bitmap_text::bitmap_text(
  const std::string& fmtfile /* = "DejaVuSansMono_15_9.fmt"*/)
    : m_program(new bitmap_text_program()), m_fmt_file(fmtfile) {}

//--------------------------------------------------------------------
bitmap_text::~bitmap_text() {}

//--------------------------------------------------------------------
void bitmap_text::init(bool legacy)
{
  m_program->legacy = legacy;
  m_program->init();
  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

  if(m_fmt_file.empty())
    return;

  // load font format information
  std::string fmtfile = m_fmt_file;
  std::ifstream is(stream_util::get_resource(fmtfile));

  if (!is)
  {
    std::stringstream ss;
    ss << "faield to load file " << fmtfile << std::endl;
    throw std::runtime_error(ss.str());
  }

  load_format(is);

}

//--------------------------------------------------------------------
void bitmap_text::load_format(std::istream& is)
{
  // read texture size
  std::string line;

  // 0 read file, 1 read metrics, 2 read characters
  int stage = 0;

  std::string texfile;
  while (getline(is, line))
  {
    if (line.empty() || line[0] == '#' || line[0] == '-')
    {
      continue;
    }

    std::stringstream ss(line);

    if (stage == 0)
    {
      ss >> texfile >> m_face;
      texfile = "font/" + texfile;
      ++stage;
    } else if (stage == 1)
    {
      ss >> m_num_characters >> m_texture_width >> m_texture_height >> m_height >> m_linespace >>
        m_max_advance;
      ++stage;
    } else if (stage == 2)
    {
      glyph glyph;
      uint32_t c;
      ss << std::hex;
      ss >> c;
      ss.unsetf(std::ios_base::hex);
      ss >> glyph.x_min >> glyph.x_max >> glyph.y_min >> glyph.y_max;
      ss >> glyph.s_min >> glyph.s_max >> glyph.t_min >> glyph.t_max;
      ss >> glyph.advance;

      if (ss.fail())
      {
        std::cout << "find illegal data : " << line << std::endl;
      }

      glyph.s_min /= m_texture_width;
      glyph.t_min /= m_texture_height;
      glyph.s_max /= m_texture_width;
      glyph.t_max /= m_texture_height;

      m_glyph_dict.insert(std::make_pair(c, glyph));
    }
  }

  if (m_glyph_dict.size() != m_num_characters)
  {
    std::cout << "load only " << m_glyph_dict.size()
       << " character info, there should be " << m_num_characters << std::endl;
  }

  if(glIsTexture(m_texture)) // you can set it manually
    return;

  fipImage fip = zxd::fipLoadResource(texfile);
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
  const glm::vec4& color /*= vec4(1.0f)*/, GLfloat scale /*= 1.0f*/)
{
  if (m_glyph_dict.empty())
  {
    throw std::runtime_error("empty glyphs, you must forgot to init it");
  }

  zxd::vec4_vector vertices;  // {x, y, s, t}

  GLuint nexty = y;
  GLuint nextx = x;

  auto beg = make_utf8_beg(text.begin(), text.end());
  auto end = make_utf8_end(text.begin(), text.end());
  // collect character quads
  while(beg != end)
  {
    auto character = *beg++;

    if (character == '\n')
    {
      nextx = x;
      nexty -= m_linespace * scale;
      continue;
    }

    auto iter = m_glyph_dict.find(character);
    if (iter == m_glyph_dict.end())
    {
      std::cout << "unknown character " << std::hex << character << std::endl;
      std::cout.unsetf(std::ios_base::hex);

      iter = m_glyph_dict.find(1);
      if(iter == m_glyph_dict.end())
        throw std::runtime_error("failed to find default character");
    }
    const glyph& glyph = iter->second;

    glm::vec2 vertex0(nextx + glyph.x_min * scale, nexty + glyph.y_min * scale);
    glm::vec2 texcoord0(glyph.s_min, glyph.t_min);

    glm::vec2 vertex1(nextx + glyph.x_max * scale, nexty + glyph.y_min * scale);
    glm::vec2 texcoord1(glyph.s_max, glyph.t_min);

    glm::vec2 vertex2(nextx + glyph.x_max * scale, nexty + glyph.y_max * scale);
    glm::vec2 texcoord2(glyph.s_max, glyph.t_max);

    glm::vec2 vertex3(nextx + glyph.x_min * scale, nexty + glyph.y_max * scale);
    glm::vec2 texcoord3(glyph.s_min, glyph.t_max);

    vertices.push_back(vec4(vertex0, texcoord0));
    vertices.push_back(vec4(vertex1, texcoord1));
    vertices.push_back(vec4(vertex2, texcoord2));

    vertices.push_back(vec4(vertex0, texcoord0));
    vertices.push_back(vec4(vertex2, texcoord2));
    vertices.push_back(vec4(vertex3, texcoord3));

    nextx += glyph.advance * scale;
  }

  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec4),
    value_ptr(vertices[0]), GL_STREAM_DRAW);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, m_texture);

  glUseProgram(*m_program);
  m_program->update_uniforms(color);

  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

//--------------------------------------------------------------------
const bitmap_text::glyph& bitmap_text::get_glyph(uint32_t codepoint)
{
  auto iter = m_glyph_dict.find(codepoint);
  if(iter == m_glyph_dict.end())
    throw std::runtime_error("codepoint not found");
  return iter->second;
}

//--------------------------------------------------------------------
std::vector<uint32_t> bitmap_text::code_points() const
{
  std::vector<uint32_t> res;
  res.reserve(m_glyph_dict.size());
  for (const auto& p : m_glyph_dict)
  {
    res.push_back(p.first);
  }
  return res;
}
}
