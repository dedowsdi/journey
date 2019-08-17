#include "geometry_util.h"

#include <set>
#include <numeric>
#include <limits>
#include <map>
#include <algorithm>
#include <cassert>

#include <glm/gtx/transform.hpp>
#include "glmath.h"
#include "geometry.h"
#include "triangle_functor.h"

namespace zxd
{

struct less_vec3_ptr
{
  bool operator() (const vec3* lhs,const vec3* rhs) const
  {
    //return *lhs < *rhs;
    const vec3& l = *lhs;
    const vec3& r = *rhs;
    if (l[0]<r[0]) return true;
    else if (l[0]>r[0]) return false;
    else if (l[1]<r[1]) return true;
    else if (l[1]>r[1]) return false;
    else return (l[2]<r[2]);
  }
};

struct smooth_triangle_functor
{

  vec3* m_coord_base;
  vec3* m_normal_base;

  // sorted vertices container
  typedef std::multiset<const vec3*,less_vec3_ptr> coordinate_set;

  coordinate_set m_coord_set;

  smooth_triangle_functor():
    m_coord_base(0),
    m_normal_base(0) {}

  void set(vec3 *cb, int noVertices, vec3 *nb)
  {
    m_coord_base=cb;
    m_normal_base=nb;

    vec3* vptr = cb;
    for(int i=0;i<noVertices;++i)
      m_coord_set.insert(vptr++);
  }

  void update_normal(const vec3& normal,const vec3* vptr)
  {
    // accumulate normal for vertex that has same position
    auto p = m_coord_set.equal_range(vptr);
    for(auto itr=p.first; itr!=p.second; ++itr)
    {
      vec3* nptr = m_normal_base + (*itr-m_coord_base);
      (*nptr) += normal;
    }
  }

  void operator() ( const vec3 &v1, const vec3 &v2, const vec3 &v3, bool treat_vertex_data_as_temporary )
  {
    if (!treat_vertex_data_as_temporary)
    {
      // calc orientation of triangle.
      vec3 normal = cross((v2-v1), (v3-v1));
      // normal.normalize();

      update_normal(normal,&v1);
      update_normal(normal,&v2);
      update_normal(normal,&v3);
    }
  }
};

//--------------------------------------------------------------------
void smooth(zxd::geometry_base& geometry, unsigned normal_attrib_index)
{
  // check if draw face exists
  bool found = false;
  for (int i = 0; i < geometry.get_num_primitive_set(); ++i) 
  {
    switch(geometry.get_primitive_set(i)->mode())
    {
      case(GL_TRIANGLES):
      case(GL_TRIANGLE_STRIP):
      case(GL_TRIANGLE_FAN):
      //case(GL_QUADS):
      //case(GL_QUAD_STRIP):
      //case(GL_POLYGON):
        found = true;
        break;
      default:
        break;
    }
    if(found)
      break;
  }

  if(!found)
    std::cout << "no face to smooth" << std::endl;

  auto vertices = geometry.attrib_vec3_array(0).get();
  if(!vertices || vertices->empty())
    return;

  // init normals to 0
  auto normals = new zxd::vec3_array();
  geometry.attrib_array(normal_attrib_index, zxd::vec3_array_ptr(normals));
  normals->reserve(vertices->size());
  for (int i = 0; i < normals->capacity(); ++i)
  {
    normals->push_back(vec3(0));
  }

  zxd::triangle_functor<smooth_triangle_functor> tf;
  tf.set(&vertices->front(), vertices->size(), &normals->front());
  geometry.accept(tf);

  for (int i = 0; i < normals->size(); ++i)
  {
    normals->at(i) = normalize(normals->at(i));
    //std::cout << normals->at(i);
  }
  geometry.bind_vao();
  normals->bind(normal_attrib_index);
  normals->update_buffer();

  //std::cout << "finish smooth" << std::endl;
}

//--------------------------------------------------------------------
vec3_vector create_circle(GLfloat radius, GLuint slices,
    const vec3& center, const vec3& normal)
{
  vec3_vector vertices;
  vertices.reserve(slices+1);
  GLfloat step_angle = zxd::f2pi / slices;
  for (int i = 0; i < slices; ++i)
  {
    GLfloat angle = step_angle * i;
    vertices.push_back(vec3(radius* cos(angle), radius * sin(angle), 0));
  }
  vertices.push_back(vertices.front());

  return transform(vertices, translate(center) * zxd::rotate_to_any(zxd::pza, normal));
}

//--------------------------------------------------------------------
vec3_vector extrude_along_line_strip(const vec3_vector& vertices, GLfloat radius,
    GLuint num_faces/* = 6*/, GLuint type/* = 0*/)
{

  if(vertices.size() < 2)
    throw std::runtime_error("you need at least 2 vertiices to extrude along");

  vec3_vector result;

  vec3_vector circle = create_circle(radius, num_faces);

  vec3_vector circle0 = transform(circle,
      translate(vertices.front()) * zxd::rotate_to_any(zxd::pza, vertices.at(1) - vertices.at(0)));

  if(type == 0)
  {
    result.reserve(num_faces * vertices.size() * 2);
    // triangle strip for each face
    std::vector<vec3_vector> strips;
    strips.resize(num_faces);
    for (int i = 0; i < num_faces; ++i)
      strips[i].reserve(vertices.size() * 2);

    for (int j = 0; j < circle0.size() - 1; ++j) 
    {
      vec3_vector& strip = strips[0];
      strip.push_back(circle0[j]);
      strip.push_back(circle0[j+1]);
    }

    for (int i = 1; i < vertices.size(); ++i)
    {
      vec3_vector circle1 = transform(circle, 
          translate(vertices.at(i)) * zxd::rotate_to_any(zxd::pza, vertices.at(i) - vertices.at(i-1)));

      for (int j = 0; j < circle0.size() - 1; ++j) 
      {
        vec3_vector& strip = strips[j];
        strip.push_back(circle1[j]);
        strip.push_back(circle1[j+1]);
      }
    }

    for (int i = 0; i < strips.size(); ++i)
    {
      result.insert(result.end(),
          std::make_move_iterator(strips[i].begin()), std::make_move_iterator(strips[i].end()));
    }

    assert(result.size() == (num_faces * vertices.size() * 2));
  }
  else if(type == 1)
  {
    result.reserve((num_faces + 1) * vertices.size());
    // triangles
    for (int i = 1; i < vertices.size(); ++i)
    {
      vec3_vector circle1 = transform(circle, 
          translate(vertices.at(i)) * zxd::rotate_to_any(zxd::pza, vertices.at(i) - vertices.at(i-1)));

      for (int j = 0; j < circle0.size() - 1; ++j)
      {
        result.push_back(circle0.at(j));
        result.push_back(circle0.at(j+1));
        result.push_back(circle1.at(j+1));

        result.push_back(circle0.at(j));
        result.push_back(circle1.at(j+1));
        result.push_back(circle1.at(j));
      }
      circle0 = std::move(circle1);
    }

    assert(result.size() == (num_faces + 1) * vertices.size());
  }

  std::cout << "finished extruding, generate " << result.size() << " vertices" << std::endl;
  return result;
}

//--------------------------------------------------------------------
vec3_vector transform(const vec3_vector& vertices, const mat4& m)
{
  vec3_vector result;
  result.reserve(vertices.size());

  for(auto& item : vertices)
  {
    vec4 v = m * vec4(item, 1);
    result.push_back(vec3(v));
  }
  return result;
}

//--------------------------------------------------------------------
vec3_vector vec2_vector_to_vec3_vector(const vec2_vector& vertices)
{
  vec3_vector res;
  res.reserve(vertices.size());
  //res.assign(vertices.begin(), vertices.end());
  for(auto& vertex : vertices)
  {
    res.push_back(vec3(vertex, 0));
  }
  
  return res;
}

//--------------------------------------------------------------------
vec4_vector vec3_vector_to_vec4_vector(const vec3_vector& vertices, GLfloat w)
{
  vec4_vector res;
  res.reserve(vertices.size());
  for(auto& item : vertices)
  {
    res.push_back(vec4(item, w));
  }
  return res;
}

//--------------------------------------------------------------------
std::pair<vec3, vec3> bounding_box(vv3_cit beg, vv3_cit end)
{
  auto min_value = vec3(std::numeric_limits<GLfloat>::max());
  auto max_value = -min_value;

  while(beg != end)
  {
    const auto& item = *beg;
    if(max_value.x < item.x)
      max_value.x = item.x;
    if(max_value.y < item.y)
      max_value.y = item.y;
    if(max_value.z < item.z)
      max_value.z = item.z;

    if(item.x < min_value.x)
      min_value.x = item.x;
    if(item.y < min_value.y)
      min_value.y = item.y;
    if(item.z < min_value.z)
      min_value.z = item.z;

    ++beg;
  }

  return std::make_pair(min_value, max_value);
}

//--------------------------------------------------------------------
std::pair<vec2, vec2> bounding_box(vv2_cit beg, vv2_cit end)
{
  auto min_value = vec2(std::numeric_limits<GLfloat>::max());
  auto max_value = -min_value;

  while(beg != end)
  {
    const auto& item = *beg;
    if(max_value.x < item.x)
      max_value.x = item.x;
    if(max_value.y < item.y)
      max_value.y = item.y;

    if(item.x < min_value.x)
      min_value.x = item.x;
    if(item.y < min_value.y)
      min_value.y = item.y;

    ++beg;
  }

  return std::make_pair(min_value, max_value);
}

inline void add_triangle_indices(uint_vector& indices, GLuint i0, GLuint i1, GLuint i2)
{
  indices.push_back(i0);
  indices.push_back(i1);
  indices.push_back(i2);
}

//--------------------------------------------------------------------
void subdivide(uint_vector& triangle_indices, vec3_vector& vertices, vec2_vector* texcoords)
{
  vertices.reserve(vertices.size() * 2);
  if(texcoords)
    texcoords->reserve(vertices.capacity());
  uint_vector res_indices;
  res_indices.reserve(triangle_indices.size() * 4);

  std::map<std::pair<GLuint, GLuint>, GLuint> lookup; // mid lookup

  for (int i = 0; i < triangle_indices.size(); i+= 3) 
  {
    std::array<GLuint, 3> mid;
    // get middle points
    for (int j=0; j<3; ++j)
    {
      GLuint idx0 = triangle_indices[i + j];
      GLuint idx1 = triangle_indices[i + (j+1)%3];
      auto p = std::minmax(idx0, idx1);
      auto iter = lookup.find(p);
      if(iter == lookup.end())
      {
        auto ir = lookup.insert(std::make_pair(p, vertices.size()));
        iter = ir.first;
        vertices.push_back(0.5f * (vertices[idx0] + vertices[idx1]));
        if(texcoords)
          texcoords->push_back(0.5f * ((*texcoords)[idx0] + (*texcoords)[idx1]));
      }
      mid[j] = iter->second;
    }
 
    // create indices for 4 inner triangles
    add_triangle_indices(res_indices, triangle_indices[i+0], mid[0], mid[2]);
    add_triangle_indices(res_indices, triangle_indices[i+1], mid[1], mid[0]);
    add_triangle_indices(res_indices, triangle_indices[i+2], mid[2], mid[1]);
    add_triangle_indices(res_indices, mid[0], mid[1], mid[2]);
  }

  triangle_indices.swap(res_indices);
}


}
