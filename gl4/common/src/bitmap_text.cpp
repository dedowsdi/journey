#include "bitmap_text.h"
#include "texutil.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include "common.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

namespace zxd {
//--------------------------------------------------------------------
BitmapText::BitmapText(
  const std::string& fmtfile /* = "DejaVuSansMono_15_9.fmt"*/)
    : mProgram(new BitmapTextProgram()), mFmtFile(fmtfile) {}

//--------------------------------------------------------------------
BitmapText::~BitmapText() {}

//--------------------------------------------------------------------
void BitmapText::init() {
  mProgram->init();
  glGenVertexArrays(1, &mVao);
  glGenBuffers(1, &mVbo);

  // load font format information
  std::string fmtfile = mFmtFile;
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
      ss >> texfile >> mFace;
      ++stage;
    } else if (stage == 1) {
      ss >> mNumCharacters >> mTextureSize >> mHeight >> mLinespace >>
        mMaxAdvance;
      ++stage;
    } else if (stage == 2) {
      Glyph glyph;
      GLint c;
      ss >> c;
      ss >> glyph.xMin >> glyph.xMax >> glyph.yMin >> glyph.yMax;
      ss >> glyph.sMin >> glyph.sMax >> glyph.tMin >> glyph.tMax;
      ss >> glyph.advance;

      if (ss.fail()) {
        std::stringstream es;
        es << "find illegal data : " << line << std::endl;
        throw std::runtime_error(es.str());
      }

      glyph.sMin /= mTextureSize;
      glyph.tMin /= mTextureSize;
      glyph.sMax /= mTextureSize;
      glyph.tMax /= mTextureSize;

      mGlyphDict.insert(std::make_pair(c, glyph));
    }
  }

  if (mGlyphDict.size() != mNumCharacters) {
    std::stringstream ss;
    ss << "load only " << mGlyphDict.size()
       << " character info, there should be " << mNumCharacters << std::endl;
    throw std::runtime_error(ss.str());
  }

  fipImage fip = zxd::fipLoadImage(texfile);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &mTexture);
  glBindTexture(GL_TEXTURE_2D, mTexture);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fip.getWidth(), fip.getHeight(), 0,
    GL_RED, GL_UNSIGNED_BYTE, fip.accessPixels());
}

//--------------------------------------------------------------------
void BitmapText::print(const std::string& text, GLuint x, GLuint y,
  const glm::vec4& color /*= vec4(1.0f)*/, GLfloat scale /*= 1.0f*/) {
  zxd::Vec4Vector vertices;  // {x, y, s, t}

  GLuint nextY = y;
  GLuint nextX = x;

  // collect character quads
  for (int i = 0; i < text.size(); ++i) {
    char character = text[i];

    if (character == '\n') {
      nextX = x;
      nextY -= mLinespace * scale;
      continue;
    }

    const Glyph& glyph = mGlyphDict[character];

    vec2 vertex0(nextX + glyph.xMin * scale, nextY + glyph.yMin * scale);
    vec2 texcoord0(glyph.sMin, glyph.tMin);

    vec2 vertex1(nextX + glyph.xMax * scale, nextY + glyph.yMin * scale);
    vec2 texcoord1(glyph.sMax, glyph.tMin);

    vec2 vertex2(nextX + glyph.xMax * scale, nextY + glyph.yMax * scale);
    vec2 texcoord2(glyph.sMax, glyph.tMax);

    vec2 vertex3(nextX + glyph.xMin * scale, nextY + glyph.yMax * scale);
    vec2 texcoord3(glyph.sMin, glyph.tMax);

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

    nextX += glyph.advance * scale;
  }

  glBindVertexArray(mVao);
  glBindBuffer(GL_ARRAY_BUFFER, mVbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices),
    value_ptr(vertices[0]), GL_STREAM_DRAW);

  glVertexAttribPointer(mProgram->attrib_vertex, 4, GL_FLOAT, GL_FALSE,
    0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(mProgram->attrib_vertex);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, mTexture);

  glUseProgram(*mProgram);
  mProgram->updateUniforms(color);

  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}
}
