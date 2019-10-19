#include <light.h>

#include <iostream>

#include <glmath.h>
#include <glm/gtc/type_ptr.hpp>

namespace zxd
{

GLint get_uniform_location(GLuint program, const std::string& name)
{
  auto location = glGetUniformLocation(program, name.c_str());
  if (location == -1)
  {
    std::cout << program << " : " << name
              << " failed to get uniform location : \"" << name << "\""
              << std::endl;
  }
  return location;
}

//--------------------------------------------------------------------
light_source::light_source()
    : ambient(0, 0, 0, 1),
      diffuse(0, 0, 0, 1),
      specular(0, 0, 0, 1),
      position(0, 0, 0, 1),
      spot_direction(0, 0, -1),
      spot_exponent(0),
      spot_cutoff(180),
      spot_cos_cutoff(-1),
      constant_attenuation(1),
      linear_attenuation(0),
      quadratic_attenuation(0) {}

//--------------------------------------------------------------------
void light_source::bind_uniform_locations(
  GLuint program, const std::string& name)
{
  ul_ambient = get_uniform_location(program, name + ".ambient");
  ul_diffuse = get_uniform_location(program, name + ".diffuse");
  ul_specular = get_uniform_location(program, name + ".specular");
  ul_position = get_uniform_location(program, name + ".position");
  ul_spot_direction = get_uniform_location(program, name + ".spot_direction");
  ul_spot_exponent = get_uniform_location(program, name + ".spot_exponent");
  ul_spot_cutoff = get_uniform_location(program, name + ".spot_cutoff");
  ul_spot_cos_cutoff = get_uniform_location(program, name + ".spot_cos_cutoff");
  ul_constant_attenuation =
    get_uniform_location(program, name + ".constant_attenuation");
  ul_linear_attenuation =
    get_uniform_location(program, name + ".linear_attenuation");
  ul_quadratic_attenuation =
    get_uniform_location(program, name + ".quadratic_attenuation");
}

//--------------------------------------------------------------------
void light_source::update_uniforms(const glm::mat4& transform)
{
  glm::vec4 position1 = transform * position; // works for both directional and point
  glm::vec3 spot_direction1 = glm::mat3(transform) * spot_direction;
  spot_cos_cutoff = cos(spot_cutoff * fpi / 180.0f);

  if (ul_ambient != -1) glUniform4fv(ul_ambient, 1, value_ptr(ambient));
  if (ul_diffuse != -1) glUniform4fv(ul_diffuse, 1, value_ptr(diffuse));
  if (ul_specular != -1) glUniform4fv(ul_specular, 1, value_ptr(specular));
  if (ul_position != -1) glUniform4fv(ul_position, 1, value_ptr(position1));
  if (ul_spot_direction != -1)
    glUniform3fv(ul_spot_direction, 1, value_ptr(spot_direction1));
  if (ul_spot_exponent != -1) glUniform1f(ul_spot_exponent, spot_exponent);
  if (ul_spot_cutoff != -1) glUniform1f(ul_spot_cutoff, spot_cutoff);
  if (ul_spot_cos_cutoff != -1)
    glUniform1f(ul_spot_cos_cutoff, spot_cos_cutoff);
  if (ul_constant_attenuation != -1)
    glUniform1f(ul_constant_attenuation, constant_attenuation);
  if (ul_linear_attenuation != -1)
    glUniform1f(ul_linear_attenuation, linear_attenuation);
  if (ul_quadratic_attenuation != -1)
    glUniform1f(ul_quadratic_attenuation, quadratic_attenuation);
}

//--------------------------------------------------------------------
GLfloat light_source::radius(GLfloat epsilon)
{
  if (position[3] == 0) return -1;

  if (constant_attenuation == 0 && linear_attenuation == 0 &&
      quadratic_attenuation == 0)
  {
    return 0;
  }

  GLfloat mdc = max_abs_component(vec3(diffuse));

  return -linear_attenuation +
         std::sqrt(
           linear_attenuation * linear_attenuation -
           4 * quadratic_attenuation * (constant_attenuation - mdc * epsilon)) /
           (2 * quadratic_attenuation);
}

#ifndef GL_VERSION_3_0
//--------------------------------------------------------------------
void light_source::pipeline(GLuint index, const glm::mat4& v_mat)
{
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadMatrixf(value_ptr(v_mat));

  GLuint light_index = GL_LIGHT0 + index;
  glLightfv(light_index, GL_AMBIENT, value_ptr(ambient));
  glLightfv(light_index, GL_DIFFUSE, value_ptr(diffuse));
  glLightfv(light_index, GL_SPECULAR, value_ptr(specular));
  glLightfv(light_index, GL_POSITION, value_ptr(position));
  glLightfv(light_index, GL_SPOT_DIRECTION, value_ptr(spot_direction));
  glLightf(light_index, GL_SPOT_EXPONENT, spot_exponent);
  glLightf(light_index, GL_SPOT_CUTOFF, spot_cutoff);
  glLightf(light_index, GL_CONSTANT_ATTENUATION, constant_attenuation);
  glLightf(light_index, GL_LINEAR_ATTENUATION, linear_attenuation);
  glLightf(light_index, GL_QUADRATIC_ATTENUATION, quadratic_attenuation);

  glPopMatrix();

  glEnable(GL_LIGHTING);
  glEnable(light_index);
}
#endif

//--------------------------------------------------------------------
void light_model::bind_uniform_locations(GLint program, const std::string& name)
{
  ul_ambient = get_uniform_location(program, name + ".ambient");
  ul_local_viewer = get_uniform_location(program, name + ".local_viewer");
}

//--------------------------------------------------------------------
void light_model::update_uniforms()
{
  if (ul_ambient != -1) glUniform4fv(ul_ambient, 1, value_ptr(ambient));
  if (ul_local_viewer != -1) glUniform1i(ul_local_viewer, local_viewer);
}

#ifndef GL_VERSION_3_0
//--------------------------------------------------------------------
void light_model::pipeline()
{
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, value_ptr(ambient));
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, local_viewer);
}
#endif

//--------------------------------------------------------------------
material::material()
    : emission(0.0, 0.0, 0.0, 1.0),
      ambient(0.2, 0.2, 0.2, 1.0),
      diffuse(0.8, 0.8, 0.8, 1.0),
      specular(0.0, 0.0, 0.0, 1.0),
      shininess(0) {}

//--------------------------------------------------------------------
void material::bind_uniform_locations(GLuint program, const std::string& name)
{
  ul_emission = get_uniform_location(program, name + ".emission");
  ul_ambient = get_uniform_location(program, name + ".ambient");
  ul_diffuse = get_uniform_location(program, name + ".diffuse");
  ul_specular = get_uniform_location(program, name + ".specular");
  ul_shininess = get_uniform_location(program, name + ".shininess");
}

//--------------------------------------------------------------------
void material::update_uniforms()
{
  if (ul_emission != -1) glUniform4fv(ul_emission, 1, value_ptr(emission));
  if (ul_ambient != -1) glUniform4fv(ul_ambient, 1, value_ptr(ambient));
  if (ul_diffuse != -1) glUniform4fv(ul_diffuse, 1, value_ptr(diffuse));
  if (ul_specular != -1) glUniform4fv(ul_specular, 1, value_ptr(specular));
  if (ul_shininess != -1) glUniform1f(ul_shininess, shininess);
}

#ifndef GL_VERSION_3_0
//--------------------------------------------------------------------
void material::pipeline()
{
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, value_ptr(emission));
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, value_ptr(ambient));
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, value_ptr(diffuse));
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, value_ptr(specular));
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
}
#endif
}
