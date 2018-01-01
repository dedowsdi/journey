#ifndef BITMAP_TEXT_H
#define BITMAP_TEXT_H

#include "glad/glad.h"
#include <string>
#include "glm.h"
#include "program.h"
#include <memory>
#include <map>

namespace zxd {

struct BitmapTextProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint loc_textColor;
  GLint loc_fontMap;
  BitmapTextProgram() {}
  void reshape(GLuint wndWidth, GLuint wndHeight) {
    modelViewProjMatrix = glm::ortho(
      0.0f, (GLfloat)wndWidth, 0.0f, (GLfloat)wndHeight, -1.0f, 1.0f);
  }
  void updateUniforms(const glm::vec4& textColor) {
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
    glUniform4fv(loc_textColor, 1, value_ptr(textColor));
    glUniform1i(loc_fontMap, 0);
  }
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/bitmap_text.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/bitmap_text.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_textColor, "textColor");
    setUniformLocation(&loc_fontMap, "fontMap");
  }
  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
  }
};

// only works with grayscale bitmap
class BitmapText {
public:
  struct Glyph {
    GLfloat xMin;  // the same as bearying X in freetype glyph
    GLfloat yMin;  // bearyingY - height
    GLfloat xMax;  // xMin + width
    GLfloat yMax;  // the same as bearying Y
    GLfloat sMin;  // texcoord
    GLfloat tMin;  //
    GLfloat sMax;  //
    GLfloat tMax;  //
    GLfloat xOrigin;
    GLfloat yOrigin;
    GLuint advance;  // in pixel
  };

protected:
  std::string mFace;
  std::string mFmtFile;
  std::shared_ptr<BitmapTextProgram> mProgram;
  GLuint mVao;
  GLuint mVbo;
  GLuint mTextureSize;
  GLuint mTexture;
  GLuint mNumCharacters;
  GLuint mHeight;
  GLuint mLinespace;
  GLuint mMaxAdvance;

  std::map<GLchar, Glyph> mGlyphDict;

public:
  BitmapText(const std::string& fmtfile = "data/font/DejaVuSansMono_15_9.fmt");
  ~BitmapText();
  void init();
  // must be called at least once
  void reshape(GLuint wndWidth, GLuint wndHeight) {
    mProgram->reshape(wndWidth, wndHeight);
  }

  void print(const std::string& text, GLuint x, GLuint y,
    const glm::vec4& color = vec4(1.0f), GLfloat scale = 1.0);
};
}

#endif /* BITMAP_TEXT_H */
