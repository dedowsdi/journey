#include "freetype_text.h"
#include "common.h"
#include "texutil.h"
#include <ft2build.h>
#include FT_FREETYPE_H

namespace zxd {

//--------------------------------------------------------------------
FreetypeText::FreetypeText(const std::string& font)
    : mFace(font),
      mProgram(new FreetypeTextProgram()),
      mNumCharacters(256),
      mHeight(15) {}

//--------------------------------------------------------------------
FreetypeText::~FreetypeText() {}

//--------------------------------------------------------------------
void FreetypeText::init() {
  mProgram->init();

  glGenVertexArrays(1, &mVao);
  glGenBuffers(1, &mVbo);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  // sub data will be changed for every character
  glBufferData(GL_ARRAY_BUFFER, 64, 0, GL_DYNAMIC_DRAW);

  updateGlyphDict();
}

//--------------------------------------------------------------------
void FreetypeText::updateGlyphDict() {
  mGlyphDict.clear();

  glBindVertexArray(mVao);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);

  // build free type glyph dict
  FT_Library ft;
  GLint error;

  error = FT_Init_FreeType(&ft);
  if (error)
    std::cout << "ERROR::FREETYPE: Could not init FreeType Library"
              << std::endl;

  FT_Face face;
  std::string font(mFace);
  error = FT_New_Face(ft, font.c_str(), 0, &face);
  if (error == FT_Err_Unknown_File_Format)
    std::cout << "ERROR:FREETYEP: unsupported face";
  else if (error == FT_Err_Cannot_Open_Resource)
    std::cout << "can not open resource " << font << std::endl;
  else if (error)
    std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

  error = FT_Set_Pixel_Sizes(face, /* handle to face object */
    0,                             /* pixel_width           */
    mHeight);                      /* pixel_height          */
  if (error) std::cout << "faield to set pixel size" << std::endl;

  mLinespace = face->size->metrics.height;
  mLinespace = mLinespace >> 6;
  mMaxAdvance = face->size->metrics.max_advance;
  mMaxAdvance = mMaxAdvance >> 6;

  // create 8 bit gray scale bitmap image

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // load 128 ascii characters
  for (GLuint c = 0; c < mNumCharacters; c++) {
    // load glyph
    if (FT_Load_Char(face, c, FT_LOAD_RENDER))
      std::cout << "faield to load char " << c << std::endl;

    FT_Bitmap& bitmap = face->glyph->bitmap;

    Glyph glyph;
    glyph.bearingX = face->glyph->bitmap_left;
    glyph.bearingY = face->glyph->bitmap_top;
    glyph.width = bitmap.width;
    glyph.height = bitmap.rows;  // bitmap.rows
    glyph.xMin = glyph.bearingX;
    glyph.yMin = glyph.bearingY - glyph.height;
    glyph.xMax = glyph.xMin + glyph.width;
    glyph.yMax = glyph.bearingY;
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
    mGlyphDict.insert(std::make_pair(c, glyph));
  }

  // clear up
  FT_Done_Face(face);
  FT_Done_FreeType(ft);
}

//--------------------------------------------------------------------
void FreetypeText::print(const std::string& text, GLuint x, GLuint y,
  const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /*= 1.0*/) {
  GLuint nextY = y;
  GLuint nextX = x;

  for (int i = 0; i < text.size(); ++i) {
    char character = text[i];
    Glyph& glyph = mGlyphDict[character];

    if (character == '\n') {
      nextX = x;
      nextY -= mLinespace;
      continue;
    } else if (character == ' ') {
      nextX += glyph.advance * scale;
      continue;
    }
    print(glyph, nextX, nextY, color, scale);

    nextX += glyph.advance * scale;
  }
}

//--------------------------------------------------------------------
void FreetypeText::print(char c, GLuint x, GLuint y,
  const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /* = 1.0*/) {
  Glyph& glyph = mGlyphDict[c];
  print(glyph, x, y, color, scale);
}

//--------------------------------------------------------------------
void FreetypeText::print(const Glyph& glyph, GLuint x, GLuint y,
  const glm::vec4& color /* = vec4(1.0f)*/, GLfloat scale /* = 1.0*/) {
  glUseProgram(*mProgram);
  mProgram->updateUniforms(color);

  // clang-format off
    // freetype generate texture from left to right, top to bottom, which means
    // we must flip y
    GLfloat vertices[4][4] = {
      {x + glyph.xMin * scale, y + glyph.yMin * scale, 0, 1},
      {x + glyph.xMax * scale, y + glyph.yMin * scale, 1, 1},
      {x + glyph.xMax * scale, y + glyph.yMax * scale, 1, 0},
      {x + glyph.xMin * scale, y + glyph.yMax * scale, 0, 0}
    };
  // clang-format on

  glBindVertexArray(mVao);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

  glVertexAttribPointer(
    mProgram->attrib_vertex, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(mProgram->attrib_vertex);

  glBindTexture(GL_TEXTURE_2D, glyph.texture);

  glDrawArrays(GL_QUADS, 0, 4);
}
}
