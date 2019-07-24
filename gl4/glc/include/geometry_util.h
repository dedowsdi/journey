#ifndef GL_GLC_GEOMETRY_UTIL_H
#define GL_GLC_GEOMETRY_UTIL_H

#include <vector>
#include <iterator>

#include <glm.h>
#include "gl.h"

namespace zxd
{
  class geometry_base;
  struct itriangle;
  using itriangle_list = std::list<itriangle>;
}

namespace geometry_util
{
  using namespace glm;

  using uint_vector = std::vector<GLuint>;
  using vec2_vector = std::vector<vec2>;
  using vec3_vector = std::vector<vec3>;
  using vec4_vector = std::vector<vec4>;
  using uvec3_vector = std::vector<uvec3>;

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
  vec4_vector vec3_vector_to_vec4_vector(const vec3_vector& vertices, GLfloat w = 1);

  using vv3_cit = std::vector<vec3>::const_iterator;
  std::pair<vec3, vec3> bounding_box(vv3_cit beg, vv3_cit end);
  
  using vv2_cit = std::vector<vec2>::iterator;
  std::pair<vec2, vec2> bounding_box(vv2_cit beg, vv2_cit end);

  void subdivide(uint_vector& triangle_indices, vec3_vector& vertices, vec2_vector* texcoords = nullptr);

  template <typename T>
  struct normalize_inplace
  {
    void operator()(T& v)
    {
      v /= glm::length(v);
    }
  };

}


#endif /* GL_GLC_GEOMETRY_UTIL_H */
