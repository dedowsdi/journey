#ifndef GL_GLC_GEOMETRY_UTIL_H
#define GL_GLC_GEOMETRY_UTIL_H

#include <glm.h>

namespace zxd
{
  class geometry_base;
  // struct itriangle;
  // using itriangle_list = std::list<itriangle>;

  using uint_vector = std::vector<GLuint>;
  using vec2_vector = std::vector<vec2>;
  using vec3_vector = std::vector<vec3>;
  using vec4_vector = std::vector<vec4>;
  using uvec3_vector = std::vector<uvec3>;

  // smooth_old of osg
  void smooth(zxd::geometry_base& gb, unsigned normal_attrib_index = 1);
  vec3_vector get_smooth_normal(zxd::geometry_base& gb);

  vec3_vector create_circle(GLfloat radius, GLuint slices,
    const vec3& center = vec3(0), const vec3& normal = vec3(0, 0, 1));
  // no branch
  // type:
  //  0 : triangle strip per face
  //  1 : triangles
  vec3_vector extrude_along_line_strip(const vec3_vector& vertices,
    GLfloat radius, GLuint num_faces = 6, GLuint type = 0);

  vec3_vector transform(const vec3_vector& vertices, const mat4& m);
  vec3_vector vec2_vector_to_vec3_vector(const vec2_vector& vertices);
  vec4_vector vec3_vector_to_vec4_vector(
    const vec3_vector& vertices, GLfloat w = 1);

  using vv3_cit = std::vector<vec3>::const_iterator;
  std::pair<vec3, vec3> bounding_box(vv3_cit beg, vv3_cit end);

  using vv2_cit = std::vector<vec2>::iterator;
  std::pair<vec2, vec2> bounding_box(vv2_cit beg, vv2_cit end);

  void subdivide(uint_vector& triangle_indices, vec3_vector& vertices,
    vec2_vector* texcoords = nullptr);

  template <typename T>
  void build_strip_elements(T& elements, GLuint stacks, GLuint slices,
    GLuint offset = 0, bool reverse = false)
  {
    for (auto i = 0u; i < stacks; ++i)
    {
      auto stack0 = (slices + 1) * i + offset;
      auto stack1 = stack0 + slices + 1;

      for (int j = 0; j <= slices; j++)
      {
        if (reverse)
        {
          elements.push_back(stack1 + j);
          elements.push_back(stack0 + j);
        }
        else
        {
          elements.push_back(stack0 + j);
          elements.push_back(stack1 + j);
        }
      }
      elements.push_back(-1);
    }
  }

  template <typename T>
  void build_strip_texcoords(T& texcoords, GLuint stacks, GLuint slices,
    const vec2& texcoord0, const vec2& texcoord1);
}

#endif /* GL_GLC_GEOMETRY_UTIL_H */
