#ifndef GL_GLC_GEOMETRY_UTIL_H
#define GL_GLC_GEOMETRY_UTIL_H

#include <vector>

#include <glm/fwd.hpp>
#include <glm/vec3.hpp>
#include "gl.h"

namespace zxd
{
  class geometry_base;
}

namespace geometry_util
{
  using namespace glm;

  using vec2_vector = std::vector<vec2>;
  using vec3_vector = std::vector<vec3>;
  using vec4_vector = std::vector<vec4>;

  // smooth_old of osg
  void smooth(zxd::geometry_base& gb, unsigned normal_attrib_index = 1);

  vec3_vector create_circle(GLfloat radius, GLuint slices,
      const vec3& center = vec3(0), const vec3& normal = vec3(0, 0, 1));
  // no branch
  // type:
  //  0 : triangle strip per face
  //  1 : triangles
  vec3_vector extrude_along_line_strip(const vec3_vector& vertices, GLfloat radius,
      GLuint num_faces = 6, GLuint type = 0);

  vec3_vector transform(const vec3_vector& vertices, const mat4& m);
  vec3_vector vec2_vector_to_vec3_vector(const vec2_vector& vertices);
}


#endif /* GL_GLC_GEOMETRY_UTIL_H */
