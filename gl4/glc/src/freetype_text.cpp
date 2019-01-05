#include "freetype_text.h"

#include <sstream>
#include <algorithm>

#include "common.h"
#include "texutil.h"
#include "glc_utf8.h"
#include "stream_util.h"
#include "guard.h"
#include "bitmap_text.h"

namespace zxd {

//--------------------------------------------------------------------
raii_ft_library::raii_ft_library(const std::string& font_name):
  m_ft(nullptr)
{
  auto error = FT_Init_FreeType(&m_ft);
  if (error)
    throw std::runtime_error("failed to init freetype library");

  std::string font(stream_util::get_resource(font_name));
  error = FT_New_Face(m_ft, font.c_str(), 0, &m_face);
  if (error == FT_Err_Unknown_File_Format)
    throw std::runtime_error("ERROR:FREETYEP: unsupported face");
  else if (error == FT_Err_Cannot_Open_Resource)
    throw std::runtime_error("can not open font " + font_name);
  else if (error)
  {
    throw std::runtime_error("ERROR::FREETYPE: failed to load font" + font_name );
  }
}

//--------------------------------------------------------------------
raii_ft_library::~raii_ft_library()
{
  if(m_face)
    FT_Done_Face(m_face);
  if(m_ft)
    FT_Done_FreeType(m_ft);
}

//--------------------------------------------------------------------
freetype_text::freetype_text(const std::string& font)
: m_face(font),
  m_program(new freetype_text_program()),
  m_height(15) {}

//--------------------------------------------------------------------
freetype_text::~freetype_text() {}

//--------------------------------------------------------------------
void freetype_text::init(const std::string& text, bool legacy) {
  m_program->legacy = legacy;
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
void freetype_text::reset_content_to_all()
{
  raii_ft_library ftr(m_face);
  auto face = ftr.face();
  FT_ULong  charcode;
  FT_UInt   gindex;

  std::vector<uint32_t> content32;
  charcode = FT_Get_First_Char( face, &gindex );
  while ( gindex != 0 )
  {
    content32.push_back(charcode);
    charcode = FT_Get_Next_Char( face, charcode, &gindex );
  }
  std::string content8;
  utf8::utf32to8(content32.begin(), content32.end(), std::back_inserter(content8));
  reset_content(content8);
  std::cout << "reset content to " << content8.size() << " characters"  << std::endl;
}

//--------------------------------------------------------------------
void freetype_text::reset_content(const std::string& text) {

  m_glyph_dict.clear();
  // always add default glyph and space
  auto content = text + u8"\x01 ";

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

  // build free type glyph dict
  raii_ft_library ftr(m_face);
  auto face = ftr.face();

  auto error = FT_Set_Pixel_Sizes(face, 0, m_height);
  if (error)
    throw std::runtime_error("failed to set pixel size to " + std::to_string(m_height));

  m_linespace = face->size->metrics.height;
  m_linespace = m_linespace >> 6;
  m_max_advance = face->size->metrics.max_advance;
  m_max_advance = m_max_advance >> 6;

  // create 8 bit gray scale bitmap image
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  std::vector<uint32_t> content32; // freetype use unicode-32 as charcode
  utf8::utf8to32(content.begin(), content.end(), std::back_inserter(content32));
  auto beg = make_utf8_beg(content.begin(), content.end());

  for (auto charcode : content32) {
    // load glyph
    if (FT_Load_Char(face, charcode, FT_LOAD_RENDER))
      std::cout << "faield to load char " << charcode << std::endl;

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
    m_glyph_dict.insert(std::make_pair(*beg++, glyph));
  }
}

//--------------------------------------------------------------------
void freetype_text::clear()
{
  m_glyph_dict.clear();
}

//--------------------------------------------------------------------
void freetype_text::print(const std::string& text, GLuint x, GLuint y,
    const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /*= 1.0*/) const {
  GLuint nextY = y;
  GLuint nextX = x;

  auto beg = make_utf8_beg(text.begin(), text.end());
  auto end = make_utf8_end(text.begin(), text.end());
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
    const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /* = 1.0*/) const {
  auto iter = m_glyph_dict.find(c);
  if(iter == m_glyph_dict.end())
  {
    raii_io_pflags obj(std::cout);
    std::cout << "unknown code point " << std::hex << c << std::endl;
    iter = m_glyph_dict.find(1);
  }
  if(iter != m_glyph_dict.end())
    throw std::runtime_error("default glyph not found");

  print(iter->second, x, y, color, scale);
}

//--------------------------------------------------------------------
void freetype_text::print(const Glyph& glyph, GLuint x, GLuint y,
    const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /* = 1.0*/) const {
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

//--------------------------------------------------------------------
std::vector<uint32_t> freetype_text::codepoints()
{
  std::vector<uint32_t> res;
  res.reserve(m_glyph_dict.size());
  for (const auto& p : m_glyph_dict) {
    res.push_back(p.first);
  }
  return res;
}

//--------------------------------------------------------------------
std::unique_ptr<bitmap_text> freetype_text::create_bitmap_text(GLuint width, GLuint height)
{
  auto tex_data = render_to_texture_2d(width, height);
  auto bt = std::make_unique<bitmap_text>("");
  bt->texture(tex_data.first);
  bt->glyph_dict(std::move(tex_data.second));
  bt->max_advance(get_max_advance());
  bt->linespace(get_linespace());
  bt->height(get_height());
  bt->texture_width(width);
  bt->texture_height(height);

  return bt;
}

//--------------------------------------------------------------------
std::pair<GLuint, bitmap_text::glyphs> freetype_text::render_to_texture_2d(GLuint width, GLuint height)
{
  reshape(width, height);
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, 0);
  auto res = std::make_pair(tex, bitmap_text::glyphs{});

  GLuint fbo;
  glGenFramebuffers(1, &fbo);
  fbo_guard fg(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    throw std::runtime_error("incomplete frame buffer");

  viewport_guard obj(0, 0, width, height);

  GLint x = 0;
  GLint y = height - get_linespace();
  glClear(GL_COLOR_BUFFER_BIT);
  glClearColor(0, 0, 0, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  for (const auto& p : m_glyph_dict) {
    auto character = p.first;
    const zxd::freetype_text::Glyph& glyph = getGlyph(character);

    if ((x + glyph.advance) > width) {
      x = 0;
      y -= get_linespace();
      if(y < 0)
      {
        std::cout << "texture not bigger enough to accommodate characters" << std::endl;
        break;
      }
    }

    print(glyph, x, y);

    zxd::bitmap_text::glyph bmg;
    bmg.x_min = glyph.x_min;
    bmg.x_max = glyph.x_max;
    bmg.y_min = glyph.y_min;
    bmg.y_max = glyph.y_max;
    bmg.s_min = (x + glyph.x_min) / width;
    bmg.s_max = (x + glyph.x_max) / width ;
    bmg.t_min = (y + glyph.y_min) / height;
    bmg.t_max = (y + glyph.y_max) / height;
    bmg.advance = glyph.advance;
    res.second.insert(std::make_pair(character, bmg));

    x += glyph.advance;
  }
  glDisable(GL_BLEND);
  
  glDeleteFramebuffers(1, &fbo);
  return res;
}

}
