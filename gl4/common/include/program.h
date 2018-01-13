#ifndef PROGRAM_H
#define PROGRAM_H

#include "glad/glad.h"
#include "glm.h"
#include <string>
#include <vector>

typedef std::vector<std::string> StringVector;
typedef std::vector<const char*> CStringVector;

namespace zxd {

// just a place hold, barely has any function.
struct Program {
public:
  GLuint object;
  // some popular matrix
  glm::mat4 viewMatrix;
  glm::mat4 projMatrix;
  glm::mat4 modelMatrix;
  glm::mat4 modelViewMatrix;
  glm::mat4 viewProjMatrix;
  glm::mat4 modelViewProjMatrix;

  glm::mat4 viewMatrixInverse;
  glm::mat4 projMatrixInverse;
  glm::mat4 modelMatrixInverse;
  glm::mat4 modelViewMatrixInverse;
  glm::mat4 viewProjMatrixInverse;
  glm::mat4 modelViewProjMatrixInverse;

  glm::mat4 viewMatrixTranspose;
  glm::mat4 projMatrixTranspose;
  glm::mat4 modelMatrixTranspose;
  glm::mat4 modelViewMatrixTranspose;
  glm::mat4 viewProjMatrixTranspose;
  glm::mat4 modelViewProjMatrixTranspose;

  glm::mat4 viewMatrixInverseTranspose;
  glm::mat4 projMatrixInverseTranspose;
  glm::mat4 modelMatrixInverseTranspose;
  glm::mat4 modelViewMatrixInverseTranspose;
  glm::mat4 viewProjMatrixInverseTranspose;
  glm::mat4 modelViewProjMatrixInverseTranspose;

  // some popular location
  GLint loc_modelMatrix;
  GLint loc_viewMatrix;
  GLint loc_projMatrix;
  GLint loc_modelViewMatrix;
  GLint loc_viwProjMatrix;
  GLint loc_modelViewProjMatrix;

  GLint loc_modelMatrixInverse;
  GLint loc_viewMatrixInverse;
  GLint loc_projMatrixInverse;
  GLint loc_modelViewMatrixInverse;
  GLint loc_viwProjMatrixInverse;
  GLint loc_modelViewProjMatrixInverse;

  GLint loc_modelMatrixTranspose;
  GLint loc_viewMatrixTranspose;
  GLint loc_projMatrixTranspose;
  GLint loc_modelViewMatrixTranspose;
  GLint loc_viwProjMatrixTranspose;
  GLint loc_modelViewProjMatrixTranspose;

  GLint loc_modelMatrixInverseTranspose;
  GLint loc_viewMatrixInverseTranspose;
  GLint loc_projMatrixInverseTranspose;
  GLint loc_modelViewMatrixInverseTranspose;
  GLint loc_viwProjMatrixInverseTranspose;
  GLint loc_modelViewProjMatrixInverseTranspose;

public:
  Program() : object(-1) {}
  virtual ~Program() {}

  operator GLuint() { return object; }

  virtual void init() {
    object = glCreateProgram();
    attachShaders();
    link();
    bindUniformLocations();
    bindAttribLocations();
  }

  void link();

  virtual void attachShaders(){};
  virtual void bindUniformLocations(){};
  virtual void bindAttribLocations(){};

  GLint getAttribLocation(const std::string& name);

  // some wrapper
  void setUniformLocation(GLint* location, const std::string& name);
  void attachShaderFile(GLenum type, const std::string& file);
  bool attachShaderSource(GLenum type, const StringVector& source);
  void attachShaderSourceAndFile(
    GLenum type, const StringVector& source, const std::string& file);
};
}

#endif /* PROGRAM_H */
