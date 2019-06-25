#ifndef GL_GLC_LIGHT_H
#define GL_GLC_LIGHT_H

#include "gl.h"
#include "glm.h"

namespace zxd
{
struct light_source;
typedef std::vector<light_source> light_vector;

struct light_source
{
  glm::vec4 ambient;
  glm::vec4 diffuse;
  glm::vec4 specular;
  glm::vec4 position;  // local
  glm::vec3 spot_direction;
  float spot_exponent;
  float spot_cutoff;
  float spot_cos_cutoff;
  float constant_attenuation;
  float linear_attenuation;
  float quadratic_attenuation;

  GLint ul_ambient;
  GLint ul_diffuse;
  GLint ul_specular;
  GLint ul_position;
  GLint ul_spot_direction;
  GLint ul_spot_exponent;
  GLint ul_spot_cutoff;
  GLint ul_spot_cos_cutoff;
  GLint ul_constant_attenuation;
  GLint ul_linear_attenuation;
  GLint ul_quadratic_attenuation;

  light_source();

  void bind_uniform_locations(GLuint program, const std::string& name);
  void update_uniforms(const glm::mat4& transform);

  // get volume radius for point and spot, -1 for dir
  GLfloat radius(GLfloat epsilon);

#ifndef GL_VERSION_3_0
  void pipeline(GLuint index, const glm::mat4& v_mat);
#endif
};

struct light_model
{
  glm::vec4 ambient;
  GLboolean local_viewer;

  GLint ul_ambient;
  GLint ul_local_viewer;

  light_model() : ambient(glm::vec4(0.2f, 0.2f, 0.2f, 1.0f)), local_viewer(0) {}

  void bind_uniform_locations(GLint program, const std::string& name = "lm");
  void update_uniforms();
#ifndef GL_VERSION_3_0
  void pipeline();
#endif
};

struct material
{
  vec4 emission;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;

  GLint ul_emission;
  GLint ul_ambient;
  GLint ul_diffuse;
  GLint ul_specular;
  GLint ul_shininess;

  material();
  void bind_uniform_locations(GLuint program, const std::string& name = "mtl");
  void update_uniforms();
#ifndef GL_VERSION_3_0
  void pipeline();
#endif
};
}

#endif /* GL_GLC_LIGHT_H */
