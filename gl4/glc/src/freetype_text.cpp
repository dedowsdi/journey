#include "freetype_text.h"

#include <sstream>

#include <ft2build.h>
#include "common.h"
#include "texutil.h"
#include "utf8.h"
#include FT_FREETYPE_H
#include "stream_util.h"

namespace zxd {

//--------------------------------------------------------------------
freetype_text::freetype_text(const std::string& font)
: m_face(font),
  m_program(new freetype_text_program()),
  m_height(15) {}

//--------------------------------------------------------------------
freetype_text::~freetype_text() {}

//--------------------------------------------------------------------
void freetype_text::init(const std::string& text) {
  m_program->init();

  glGenVertexArrays(1, &m_vao);
  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  // sub data will be changed for every character
  glBufferData(GL_ARRAY_BUFFER, 64, 0, GL_DYNAMIC_DRAW);

  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

  reset_content(text + u8"\x1"); // 1 will be used as unknown character
}

//--------------------------------------------------------------------
void freetype_text::reset_content_as_ascii()
{
  std::stringstream ss;
  for (int i = 0; i < 128; ++i) {
    ss << static_cast<unsigned char>(i);
  }
  reset_content(ss.str());
}

//--------------------------------------------------------------------
void freetype_text::reset_content(const std::string& text) {

  m_glyph_dict.clear();
  // always add default glyph and space
  auto content = text + u8"\x1 ";

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

  // build free type glyph dict
  FT_Library ft;
  GLint error;

  error = FT_Init_FreeType(&ft);
  if (error)
    std::cout << "ERROR::FREETYPE: could not init FreeType library"
    << std::endl;

  FT_Face face;
  std::string font(stream_util::get_resource(m_face));
  error = FT_New_Face(ft, font.c_str(), 0, &face);
  if (error == FT_Err_Unknown_File_Format)
    std::cout << "ERROR:FREETYEP: unsupported face";
  else if (error == FT_Err_Cannot_Open_Resource)
    std::cout << "can not open font " << font << std::endl;
  else if (error)
    std::cout << "ERROR::FREETYPE: failed to load font" << std::endl;

  error = FT_Set_Pixel_Sizes(face, /* handle to face object */
      0,                             /* pixel_width           */
      m_height);                      /* pixel_height          */
  if (error) std::cout << "faield to set pixel size" << std::endl;

  m_linespace = face->size->metrics.height;
  m_linespace = m_linespace >> 6;
  m_max_advance = face->size->metrics.max_advance;
  m_max_advance = m_max_advance >> 6;

  // create 8 bit gray scale bitmap image
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  auto beg = utf8::iterator<std::string::const_iterator>(content.begin(), content.begin(), content.end());
  auto end = utf8::iterator<std::string::const_iterator>(content.end(), content.begin(), content.end());
  while(beg != end)
  {
    uint32_t c = *beg++;
    // load glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      std::cout << "faield to load char " << c << std::endl;

    FT_Bitmap& bitmap = face->glyph->bitmap;

    Glyph glyph;
    glyph.bearingX = face->glyph->bitmap_left;
    glyph.bearingY = face->glyph->bitmap_top;
    glyph.width = bitmap.width;
    glyph.height = bitmap.rows;  // bitmap.rows
    glyph.x_min = glyph.bearingX;
    glyph.y_min = glyph.bearingY - glyph.height;
    glyph.x_max = glyph.x_min + glyph.width;
    glyph.y_max = glyph.bearingY;
    // glyph.origin = face->glyph->origin;
    glyph.advance = face->glyph->advance.x;
    glyph.advance >>= 6;

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph.width, glyph.height, 0, GL_RED,
        GL_UNSIGNED_BYTE, bitmap.buffer);

    glyph.texture = tex;
    m_glyph_dict.insert(std::make_pair(c, glyph));
  }

  // clear up
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

//--------------------------------------------------------------------
void freetype_text::clear()
{
  m_glyph_dict.clear();
}

//--------------------------------------------------------------------
void freetype_text::print(const std::string& text, GLuint x, GLuint y,
    const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /*= 1.0*/) {
  GLuint nextY = y;
  GLuint nextX = x;

  auto beg = utf8::iterator<std::string::const_iterator>(text.begin(), text.begin(), text.end());
  auto end = utf8::iterator<std::string::const_iterator>(text.end(), text.begin(), text.end());
  while(beg != end)
  {
    auto character = *beg++;
    if (character == '\n') {
      nextX = x;
      nextY -= m_linespace * scale;
      continue;
    }

    auto iter = m_glyph_dict.find(character);
    if(iter == m_glyph_dict.end())
    {
      std::cerr << "failed to find glyph for " << std::hex << character << std::endl;
      return;
    }

    const Glyph& glyph = iter->second;

    if (character == ' ') {
      nextX += glyph.advance * scale;
      continue;
    }

    print(glyph, nextX, nextY, color, scale);

    nextX += glyph.advance * scale;
  }
}

//--------------------------------------------------------------------
void freetype_text::print(uint32_t c, GLuint x, GLuint y,
    const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /* = 1.0*/) {
  Glyph& glyph = m_glyph_dict[c];
  print(glyph, x, y, color, scale);
}

//--------------------------------------------------------------------
void freetype_text::print(const Glyph& glyph, GLuint x, GLuint y,
    const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /* = 1.0*/) {
  glUseProgram(*m_program);
  m_program->update_uniforms(color);

  // freetype generate texture from left to right, top to bottom, which means
  // we must flip y
  GLfloat vertices[4][4] = {
    {x + glyph.x_min * scale, y + glyph.y_min * scale, 0, 1},
    {x + glyph.x_max * scale, y + glyph.y_min * scale, 1, 1},
    {x + glyph.x_max * scale, y + glyph.y_max * scale, 1, 0},
    {x + glyph.x_min * scale, y + glyph.y_max * scale, 0, 0}
  };

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

  glBindTexture(GL_TEXTURE_2D, glyph.texture);

  glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

//--------------------------------------------------------------------
const freetype_text::Glyph& freetype_text::getGlyph(uint32_t c) const
{ 
  auto iter = m_glyph_dict.find(c);
  if(iter != m_glyph_dict.end())
    return iter->second;

  std::cout << "failed to find character " << std::hex << c << std::endl;
  std::cout.unsetf(std::ios_base::hex);
  iter = m_glyph_dict.find(1);
  if(iter != m_glyph_dict.end())
    return iter->second;

  throw std::runtime_error("failed to find default character glyph");
}

}
