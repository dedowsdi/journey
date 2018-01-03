#ifndef TEXT_H
#define TEXT_H

#include "glad/glad.h"
#include <string>
#include "glm.h"
#include "program.h"
#include <memory>
#include <map>

namespace zxd {

struct FreetypeTextProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint loc_textColor;
  GLint loc_fontMap;
  FreetypeTextProgram() {
  }
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
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/freetype_text.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/freetype_text.fs.glsl");
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
class FreetypeText {
public:
  struct Glyph {
    GLfloat xMin;  // the same as bearying X
    GLfloat yMin;  // bearyingY - height
    GLfloat xMax;  // xMin + width
    GLfloat yMax;  // the same as bearying Y
    GLfloat bearingX;
    GLfloat bearingY;
    GLfloat width;   // width of bounding box
    GLfloat height;  // height of bounding box, bitmap.rows
    GLfloat origin;  // not used
    GLuint advance;  // in pixel
    GLuint texture;
  };

protected:
  GLuint mNumCharacters;
  GLuint mVao;
  GLuint mVbo;
  GLuint mTexture;
  GLuint mHeight;     // width will be deduced from height by freetype
  GLuint mLinespace;  // max height, in pixel
  GLuint mMaxAdvance; // in pixel

  std::string mFace;
  std::shared_ptr<FreetypeTextProgram> mProgram;
  std::map<GLchar, Glyph> mGlyphDict;

public:
  FreetypeText(const std::string& font);
  ~FreetypeText();
  void init();

  void updateGlyphDict();

  // must be called at least once
  void reshape(GLuint wndWidth, GLuint wndHeight) {
    mProgram->reshape(wndWidth, wndHeight);
  }

  // linespace = height of 'I' + 2 if it's 0
  void print(const std::string& text, GLuint x, GLuint y,
    const glm::vec4& color = vec4(1.0f), GLfloat scale = 1.0);

  void print(char c, GLuint x, GLuint y, const glm::vec4& color = vec4(1.0f),
    GLfloat scale = 1.0);

  void print(const Glyph& glyph, GLuint x, GLuint y,
    const glm::vec4& color = vec4(1.0f), GLfloat scale = 1.0);

  GLuint getNumCharacters() const { return mNumCharacters; }
  void setNumCharacters(GLuint v) { mNumCharacters = v; }

  GLuint getHeight() const { return mHeight; }
  void setHeight(GLuint v) { mHeight = v; }

  const std::string& getFace() const { return mFace; }
  void setFace( const std::string& v){mFace = v;}

  GLuint getLinespace() const { return mLinespace; }
  void setLinespace(GLuint v) { mLinespace = v; }

  GLuint getMaxAdvance() const { return mMaxAdvance; }
  void setMaxAdvance(GLuint v) { mMaxAdvance = v; }

  const std::map<GLchar, Glyph>& getGlyphDict() { return mGlyphDict; }
  const Glyph& getGlyph(char c) { return mGlyphDict[c]; }
};
}

#endif /* TEXT_H */
