#include "cuboid.h"
#include "common.h"

namespace zxd {

//--------------------------------------------------------------------
void cuboid::build_vertex() {
  vec3_array& vertices = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve(24);

  GLfloat x = m_half_diag.x;
  GLfloat y = m_half_diag.y;
  GLfloat z = m_half_diag.z;
  GLfloat x2 = x * 2;
  GLfloat y2 = y * 2;
  GLfloat z2 = z * 2;

  // make sure that if  rotate around z, origin of every face is at low left
  // corner.
  glm::vec3 origins[6] = {
    vec3(-x, -y, -z),  // front
    vec3(x, y, -z),    // back
    vec3(x, -y, -z),   // right
    vec3(-x, y, -z),   // left
    vec3(-x, -y, z),   // top
    vec3(-x, y, -z)    // bottom
  };

  glm::vec3 xsteps[6] = {
    vec3(x2, 0, 0), vec3(-x2, 0, 0),  // front, back
    vec3(0, y2, 0), vec3(0, -y2, 0),  // right, left
    vec3(x2, 0, 0), vec3(x2, 0, 0)    // top, bottom
  };

  glm::vec3 ysteps[6] = {
    vec3(0, 0, z2), vec3(0, 0, z2),  // front, back
    vec3(0, 0, z2), vec3(0, 0, z2),  // right, left
    vec3(0, y2, 0), vec3(0, -y2, 0)  // top, bottom
  };

  for (int i = 0; i < 6; ++i) {
    const vec3& origin = origins[i];
    const vec3& xstep = xsteps[i];
    const vec3& ystep = ysteps[i];

    vertices.push_back(origin);
    vertices.push_back(origin + xstep);
    vertices.push_back(origin + xstep + ystep);

    vertices.push_back(origin);
    vertices.push_back(origin + xstep + ystep);
    vertices.push_back(origin + ystep);
  }

  m_primitive_sets.clear();
  add_primitive_set(new draw_arrays(GL_TRIANGLES, 0, num_vertices()));
}

//--------------------------------------------------------------------
void cuboid::build_normal() {
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(num_vertices());
  for (int i = 0; i < 6; ++i) normals.push_back(vec3(0, -1, 0));
  for (int i = 0; i < 6; ++i) normals.push_back(vec3(0, 1, 0));
  for (int i = 0; i < 6; ++i) normals.push_back(vec3(1, 0, 0));
  for (int i = 0; i < 6; ++i) normals.push_back(vec3(-1, 0, 0));
  for (int i = 0; i < 6; ++i) normals.push_back(vec3(0, 0, 1));
  for (int i = 0; i < 6; ++i) normals.push_back(vec3(0, 0, -1));
}

//--------------------------------------------------------------------
void cuboid::build_texcoord() {
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());

  for (int i = 0; i < 6; ++i) {
    texcoords.push_back(glm::vec2(0, 0));
    texcoords.push_back(glm::vec2(1, 0));
    texcoords.push_back(glm::vec2(1, 1));
    texcoords.push_back(glm::vec2(0, 0));
    texcoords.push_back(glm::vec2(1, 1));
    texcoords.push_back(glm::vec2(0, 1));
  }
}

}
