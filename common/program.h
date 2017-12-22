#include <GL/glew.h>
#include "glm.h"
#include <string>
#include <vector>

typedef std::vector<std::string> StringVector;
typedef std::vector<const char*> CStringVector;

#ifndef PROGRAM_H
#define PROGRAM_H

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

  // some popular location
  GLint loc_modelMatrix;
  GLint loc_viewMatrix;
  GLint loc_projMatrix;
  GLint loc_modelViewMatrix;
  GLint loc_viwProjMatrix;
  GLint loc_modelViewProjMatrix;

public:
  Program() {}
  virtual ~Program() {}

  operator GLuint() { return object; }

  // per model
  virtual void updateModel(const glm::mat4& model) {
    modelMatrix = model;
    doUpdateModel();
  }
  virtual void doUpdateModel() = 0;

  // per frame
  void updateFrame() {
    glUseProgram(object);
    doUpdateFrame();
  }
  virtual void doUpdateFrame() {}

  virtual void init() {
    object = glCreateProgram();
    attachShaders();
    glLinkProgram(object);
    GLint status;
    glGetProgramiv(object, GL_LINK_STATUS, &status);
    if (!status) {
      printf("failed to link program %d", object);
    }
    bindLocations();
  }

  virtual void attachShaders() = 0;
  virtual void bindLocations(){};

  // some wrapper
  void setUniformLocation(GLint* location, const std::string& name);
  void attachShaderFile(GLenum type, const std::string& file);
  void attachShaderSource(GLenum type, StringVector& sources);
  void attachShaderSourceAndFile(GLenum type, StringVector& sources, const std::string& file);
};
}

#endif /* PROGRAM_H */
