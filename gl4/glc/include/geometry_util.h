#ifndef GEOMETRY_UTIL_H
#define GEOMETRY_UTIL_H
#include "glm.h"

namespace zxd
{
  class geometry_base;
}

namespace geometry_util
{
  // smooth_old of osg
  void smooth(zxd::geometry_base& gb, unsigned normal_attrib_index = 1);

  zxd::vec3_vector create_circle(GLfloat radius, GLuint slices,
      const glm::vec3& center = glm::vec3(0), const glm::vec3& normal = zxd::pza);
  // no branch
  // type:
  //  0 : triangle strip per face
  //  1 : triangles
  zxd::vec3_vector extrude_along_line_strip(const zxd::vec3_vector& vertices, GLfloat radius,
      GLuint num_faces = 6, GLuint type = 0);

  zxd::vec3_vector transform(const zxd::vec3_vector& vertices, const glm::mat4& m);
}


#endif /* GEOMETRY_UTIL_H */
