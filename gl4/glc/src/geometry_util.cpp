#include "geometry_util.h"
#include "geometry.h"
#include <set>
#include "triangle_functor.h"

using namespace zxd;
namespace geometry_util
{

struct less_vec3_ptr
{
  inline bool operator() (const vec3* lhs,const vec3* rhs) const
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

  inline void update_normal(const vec3& normal,const vec3* vptr)
  {
    // accumulate normal for vertex that has same position
    auto p = m_coord_set.equal_range(vptr);
    for(auto itr=p.first; itr!=p.second; ++itr)
    {
      vec3* nptr = m_normal_base + (*itr-m_coord_base);
      (*nptr) += normal;
    }
  }

  inline void operator() ( const vec3 &v1, const vec3 &v2, const vec3 &v3, bool treat_vertex_data_as_temporary )
  {
    if (!treat_vertex_data_as_temporary)
    {
      // calc orientation of triangle.
      vec3 normal = glm::cross((v2-v1), (v3-v1));
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

  vec3_array* vertices = geometry.attrib_vec3_array(0).get();
  if(!vertices || vertices->empty())
    return;

  // init normals to 0
  vec3_array* normals = new vec3_array();
  geometry.attrib_array(normal_attrib_index, vec3_array_ptr(normals));
  normals->reserve(vertices->size());
  for (int i = 0; i < normals->capacity(); ++i) {
    normals->push_back(vec3(0));
  }

  triangle_functor<smooth_triangle_functor> tf;
  tf.set(&vertices->front(), vertices->size(), &normals->front());
  geometry.accept(tf);

  for (int i = 0; i < normals->size(); ++i) {
    normals->at(i) = glm::normalize(normals->at(i));
    //std::cout << normals->at(i);
  }
  geometry.bind_vao();
  normals->bind(normal_attrib_index);
  normals->update_array_buffer();

  std::cout << "finish smooth" << std::endl;
}
}
