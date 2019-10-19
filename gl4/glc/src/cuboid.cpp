#include <cuboid.h>

#include <algorithm>
#include <stream_util.h>
#include <common.h>

namespace zxd
{

cuboid::cuboid(const glm::vec3& half_diag, type _type)
    : m_type(_type), m_half_diag(half_diag)
{
}

cuboid::cuboid(GLfloat size, type _type) : cuboid(vec3(size * 0.5f), _type) {}

void add_quad(vec3_array& vertices, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
  vertices.push_back(vertices[v0]);
  vertices.push_back(vertices[v1]);
  vertices.push_back(vertices[v2]);
  vertices.push_back(vertices[v3]);
}

void add_quad(uint_array &elements, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
  elements.push_back(v0);
  elements.push_back(v1);
  elements.push_back(v2);

  elements.push_back(v0);
  elements.push_back(v2);
  elements.push_back(v3);
}


//--------------------------------------------------------------------
common_geometry::vertex_build cuboid::build_vertices()
{

  //        1         0
  //        ----------
  //  2     |   3    |
  //  ------|---     |
  //  |     |  |     |
  //  |     |  |     |
  //  |     |  |     |
  //  |    6---|------7
  //  |        |
  // 5----------4
  //
  // 0, 1, 2, 3  --- top
  // 4, 5, 6, 7  --- bottom
  // 2, 5, 4, 3  --- front
  // 0, 7, 6, 1  --- back
  // 4, 7, 0, 3  --- right
  // 2, 1, 6, 5  --- left
  //
  // there is a tetrahedron between 0 2 4 6, it's 6 edges is also the shared
  // edge in each of the 6 faces

  auto vertices = std::make_unique<vec3_array>();
  auto num_vertices = m_type == type::CT_8 ? 8 : 24;

  vertices->reserve(num_vertices);

  GLfloat x = abs(m_half_diag.x);
  GLfloat y = abs(m_half_diag.y);
  GLfloat z = abs(m_half_diag.z);

  // top (ccw)
  vertices->push_back( vec3( x,  y,  z) );
  vertices->push_back( vec3(-x,  y,  z) );
  vertices->push_back( vec3(-x, -y,  z) );
  vertices->push_back( vec3( x, -y,  z) );

  // bottom (cw)
  vertices->push_back( vec3( x, -y, -z) );
  vertices->push_back( vec3(-x, -y, -z) );
  vertices->push_back( vec3(-x,  y, -z) );
  vertices->push_back( vec3( x,  y, -z) );

  if (m_type == type::CT_24)
  {
    add_quad(*vertices, 2, 5, 4, 3); //  --- front
    add_quad(*vertices, 0, 7, 6, 1); //  --- back
    add_quad(*vertices, 4, 7, 0, 3); //  --- right
    add_quad(*vertices, 2, 1, 6, 5); //  --- left
  }

  auto& elements = make_element<uint_array>();
  elements.reserve(36);

  add_quad(elements, 0, 1, 2, 3);
  add_quad(elements, 4, 5, 6, 7);

  if (m_type == type::CT_24)
  {
    add_quad(elements, 8, 9, 10, 11);
    add_quad(elements, 12, 13, 14, 15);
    add_quad(elements, 16, 17, 18, 19);
    add_quad(elements, 20, 21, 22, 23);
  }
  else
  {
    add_quad(elements, 2, 5, 4, 3);
    add_quad(elements, 0, 7, 6, 1);
    add_quad(elements, 4, 7, 0, 3);
    add_quad(elements, 2, 1, 6, 5);
  }

  clear_primitive_sets();
  add_primitive_set(std::make_shared<draw_elements>(
    GL_TRIANGLES, elements.size(), GL_UNSIGNED_INT, 0));

  return vertex_build{std::move(vertices)};
  return vertex_build{std::move(vertices)};
}

//--------------------------------------------------------------------
array_uptr cuboid::build_normals(const array& vertices)
{
  auto normals = std::make_unique<vec3_array>();
  normals->reserve(vertices.size());
  auto& v = static_cast<const vec3_array&>(vertices);
  if (m_type == type::CT_8)
  {
    std::transform(v.begin(), v.end(), std::back_inserter(*normals),
      [](const auto& vertex) -> vec3 { return normalize(vertex); });
  }
  else
  {
    normals->insert(normals->end(), 4, vec3( 0,  0,  1));
    normals->insert(normals->end(), 4, vec3( 0,  0, -1));
    normals->insert(normals->end(), 4, vec3( 0, -1,  0));
    normals->insert(normals->end(), 4, vec3( 0,  1,  0));
    normals->insert(normals->end(), 4, vec3( 1,  0,  0));
    normals->insert(normals->end(), 4, vec3(-1,  0,  0));
  }
  return normals;
}

//--------------------------------------------------------------------
array_uptr cuboid::build_texcoords(const array& vertices)
{
  if (m_type != type::CT_24)
  {
    throw std::runtime_error(
      "CT_24 should be selected if you want to add texcoord to a cuboid");
  }

  auto texcoords = std::make_unique<vec2_array>();
  texcoords->reserve(vertices.size());

  // only care about front orientation
  for (auto i = 0u; i < 6; ++i)
  {
    texcoords->push_back(glm::vec2(0, 1));
    texcoords->push_back(glm::vec2(0, 0));
    texcoords->push_back(glm::vec2(1, 0));
    texcoords->push_back(glm::vec2(1, 1));
  }
  return texcoords;
  return texcoords;
}

}
