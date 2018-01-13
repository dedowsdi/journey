#include "light.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
LightSource::LightSource()
    : ambient(0, 0, 0, 1),
      diffuse(0, 0, 0, 1),
      specular(0, 0, 0, 1),
      position(0, 0, 0, 1),
      spotDirection(0, 0, -1),
      spotExponent(0),
      spotCutoff(180),
      spotCosCutoff(-1),
      constantAttenuation(1),
      linearAttenuation(0),
      quadraticAttenuation(0) {}

//--------------------------------------------------------------------
void LightSource::bindUniformLocations(
  GLuint program, const std::string& name) {
  setUniformLocation(&loc_ambient, program, name + ".ambient");
  setUniformLocation(&loc_diffuse, program, name + ".diffuse");
  setUniformLocation(&loc_specular, program, name + ".specular");
  setUniformLocation(&loc_position, program, name + ".position");
  setUniformLocation(&loc_spotDirection, program, name + ".spotDirection");
  setUniformLocation(&loc_spotExponent, program, name + ".spotExponent");
  setUniformLocation(&loc_spotCutoff, program, name + ".spotCutoff");
  setUniformLocation(&loc_spotCosCutoff, program, name + ".spotCosCutoff");
  setUniformLocation(
    &loc_constantAttenuation, program, name + ".constantAttenuation");
  setUniformLocation(
    &loc_linearAttenuation, program, name + ".linearAttenuation");
  setUniformLocation(
    &loc_quadraticAttenuation, program, name + ".quadraticAttenuation");
}

//--------------------------------------------------------------------
void LightSource::updateUniforms( const glm::mat4& transform) {
  glm::vec4 viewPosition = transform * position;
  glm::vec3 viewSpotDirection = glm::mat3(transform) * spotDirection;

  glUniform4fv(loc_ambient, 1, value_ptr(ambient));
  glUniform4fv(loc_diffuse, 1, value_ptr(diffuse));
  glUniform4fv(loc_specular, 1, value_ptr(specular));
  glUniform4fv(loc_position, 1, value_ptr(viewPosition));
  glUniform3fv(loc_spotDirection, 1, value_ptr(viewSpotDirection));
  glUniform1f(loc_spotExponent, spotExponent);
  glUniform1f(loc_spotCutoff, spotCutoff);
  glUniform1f(loc_spotCosCutoff, spotCosCutoff);
  glUniform1f(loc_constantAttenuation, constantAttenuation);
  glUniform1f(loc_linearAttenuation, linearAttenuation);
  glUniform1f(loc_quadraticAttenuation, quadraticAttenuation);
}

//--------------------------------------------------------------------
void LightSource::pipeline(GLuint index, const glm::mat4& viewMatrix) {
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadMatrixf(value_ptr(viewMatrix));

  GLuint glLightIndex = GL_LIGHT0 + index;
  glLightfv(glLightIndex, GL_AMBIENT, value_ptr(ambient));
  glLightfv(glLightIndex, GL_DIFFUSE, value_ptr(diffuse));
  glLightfv(glLightIndex, GL_SPECULAR, value_ptr(specular));
  glLightfv(glLightIndex, GL_POSITION, value_ptr(position));
  glLightfv(glLightIndex, GL_SPOT_DIRECTION, value_ptr(spotDirection));
  glLightf(glLightIndex, GL_SPOT_EXPONENT, spotExponent);
  glLightf(glLightIndex, GL_SPOT_CUTOFF, spotCutoff);
  glLightf(glLightIndex, GL_CONSTANT_ATTENUATION, constantAttenuation);
  glLightf(glLightIndex, GL_LINEAR_ATTENUATION, linearAttenuation);
  glLightf(glLightIndex, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);

  glPopMatrix();

  glEnable(GL_LIGHTING);
  glEnable(glLightIndex);
}

//--------------------------------------------------------------------
void LightModel::bindUniformLocations(GLint program, const std::string& name) {
  setUniformLocation(&loc_ambient, program, name + ".ambient");
  setUniformLocation(&loc_localViewer, program, name + ".localViewer");
}

//--------------------------------------------------------------------
void LightModel::updateUniforms() {
  glUniform4fv(loc_ambient, 1, value_ptr(ambient));
  glUniform1i(loc_localViewer, localViewer);
}

//--------------------------------------------------------------------
void LightModel::pipeline() {
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, value_ptr(ambient));
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);
}

//--------------------------------------------------------------------
Material::Material()
    : emission(0.0, 0.0, 0.0, 1.0),
      ambient(0.2, 0.2, 0.2, 1.0),
      diffuse(0.8, 0.8, 0.8, 1.0),
      specular(0.0, 0.0, 0.0, 1.0),
      shininess(0) {}

//--------------------------------------------------------------------
void Material::bindUniformLocations(GLuint program, const std::string& name) {
  setUniformLocation(&loc_emission, program, name + ".emission");
  setUniformLocation(&loc_ambient, program, name + ".ambient");
  setUniformLocation(&loc_diffuse, program, name + ".diffuse");
  setUniformLocation(&loc_specular, program, name + ".specular");
  setUniformLocation(&loc_shininess, program, name + ".shininess");
}

//--------------------------------------------------------------------
void Material::updateUniforms() {
  glUniform4fv(loc_emission, 1, value_ptr(emission));
  glUniform4fv(loc_ambient, 1, value_ptr(ambient));
  glUniform4fv(loc_diffuse, 1, value_ptr(diffuse));
  glUniform4fv(loc_specular, 1, value_ptr(specular));
  glUniform1f(loc_shininess, shininess);
}

//--------------------------------------------------------------------
void Material::pipeline() {
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, value_ptr(emission));
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, value_ptr(ambient));
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, value_ptr(diffuse));
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, value_ptr(specular));
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
}
