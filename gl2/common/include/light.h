#include "glad/glad.h"
#include "glm.h"

namespace zxd {

struct LightSource {
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  glm::vec4 position;  // local
  glm::vec3 spotDirection;
  float spotExponent;
  float spotCutoff;
  float spotCosCutoff;
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;

  GLint loc_ambient;
  GLint loc_diffuse;
  GLint loc_specular;
  GLint loc_position;
  GLint loc_spotDirection;
  GLint loc_spotExponent;
  GLint loc_spotCutoff;
  GLint loc_spotCosCutoff;
  GLint loc_constantAttenuation;
  GLint loc_linearAttenuation;
  GLint loc_quadraticAttenuation;

  LightSource();

  void bindUniformLocations(GLuint program, const std::string& name);

  void updateUniforms( const glm::mat4& transform);

  void pipeline(GLuint index, const glm::mat4& viewMatrix);
};

struct LightModel {
  glm::vec4 ambient;
  GLboolean localViewer;

  GLint loc_ambient;
  GLint loc_localViewer;

  LightModel() : ambient(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)), localViewer(0) {}

  void bindUniformLocations(GLint program, const std::string& name);
  void updateUniforms();
  void pipeline();
};

struct Material {
  vec4 emission;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;

  GLint loc_emission;
  GLint loc_ambient;
  GLint loc_diffuse;
  GLint loc_specular;
  GLint loc_shininess;

  Material();
  void bindUniformLocations(GLuint program, const std::string& name);
  void updateUniforms();
  void pipeline();
};
}
