#include <icosahedron.h>

#include <algorithm>
#include <iostream>

#include <glm/trigonometric.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <functor.h>
#include <geometry_util.h>
#include <exception.h>

namespace zxd
{

//--------------------------------------------------------------------
icosahedron::icosahedron(GLfloat radius/* = 1.0f*/, GLuint subdivisions/* = 0*/, mesh_type mt/* = mesh_type::MT_MININUM*/):
  m_radius(radius),
  m_subdivisions(subdivisions),
  m_mesh_type(mt)
{
}

//--------------------------------------------------------------------
common_geometry::vertex_build icosahedron::build_vertices()
{
  if(m_mesh_type == mesh_type::MININUM)
    return build_minimun_mesh();
  else
    return build_paper_unwrapper_mesh();
}

//--------------------------------------------------------------------
array_ptr icosahedron::build_normals(const array& vertices)
{
  auto normals =
    std::make_unique<vec3_array>(static_cast<const vec3_array&>(vertices));
  for(auto& item : *normals)
  {
    item /= m_radius;
  }
  return normals;
}

//--------------------------------------------------------------------
common_geometry::vertex_build icosahedron::build_minimun_mesh()
{
  auto vertices = std::make_unique<vec3_array>();

  vertices->resize(12); // pole points repeated 5 times, +2 center points
  // ten points in center
  GLfloat latitude = atan(0.5f);
  GLfloat step_angle = fpi * 72.0f / 180.0f;
  GLfloat up_start_angle = fpi2;
  GLfloat down_start_angle = up_start_angle + fpi*36.0f/180.0f;
  GLuint up_index = 1;
  GLuint down_index = up_index + 5;
  GLuint south_pole_index = down_index + 5;

  // north, south, pole
  vertices->at(0) = vec3(0, 0, 1);
  vertices->at(south_pole_index) = vec3(0, 0, -1);

  // center
  for (int i = 0; i < 5; ++i) 
  {
    GLfloat z = sin(latitude);
    GLfloat proj = cos(latitude);
    GLfloat up_angle = up_start_angle + step_angle * i;
    GLfloat down_angle = down_start_angle + step_angle * i;
    vertices->at(up_index + i) = vec3(proj * cos(up_angle), proj * sin(up_angle) ,z);
    vertices->at(down_index + i) = vec3(proj * cos(down_angle), proj * sin(down_angle), -z);
  }

  auto elements = make_element<uint_array>();
  elements->reserve(60);

  // 5 triangles on top
  for (int i = 0; i < 5; ++i)
  {
    auto i1 = up_index + i;
    auto i2 = i == 4 ? up_index : i1 + 1;
    elements->push_back(0);
    elements->push_back(i1);
    elements->push_back(i2);
  }
  
  // 10 triangles on center
  for (int i = 0; i < 5; ++i)
  {
    {
      auto i0 = up_index + i;
      auto i2 = i == 4 ? up_index : i0 + 1;
      auto i1 = down_index + i;
      elements->push_back(i0);
      elements->push_back(i1);
      elements->push_back(i2);
    }
    {
      auto i0 = down_index + i;
      auto i2 = i == 4 ? down_index : i0 + 1;
      auto i1 = i == 4 ? up_index : up_index + 1 + i;
      elements->push_back(i0);
      elements->push_back(i2);
      elements->push_back(i1);
    }
  }

  // 5 triangles on bottom
  for (int i = 0; i < 5; ++i)
  {
    auto i1 = down_index + i;
    auto i2 = i == 4 ? down_index : i1 + 1;
    elements->push_back(south_pole_index);
    elements->push_back(i2);
    elements->push_back(i1);
  }

  for(auto& item : *vertices)
    item *= m_radius;

  for (int i = 0; i < m_subdivisions; ++i)
  {
    auto old_size = vertices->size();
    subdivide(elements->get_vector(), vertices->get_vector());
    std::for_each(vertices->begin() + old_size, vertices->end(),
      [this](vec3& v) -> void { v = glm::normalize(v) * m_radius; });
  }

  //TODO find a better way to generate texcoords
  auto texcoords = std::make_unique<vec2_array>();
  texcoords->reserve(vertices->size());

  for(auto& item : *vertices)
  {
    auto v = glm::normalize(item);
    GLfloat s = atan2(-v.x, v.y);
    if(s < 0)
      s += f2pi;
    s /= f2pi;
    GLfloat t = (v.z + 1) * 0.5f;
    texcoords->push_back(vec2(s, t));
  }

  std::cout << "icosahedron total vertices : " << vertices->size() << std::endl;

  auto ps = std::make_unique<draw_elements>(
    GL_TRIANGLES, elements->size(), GL_UNSIGNED_INT, 0);
  ps->indices(&elements->front());
  add_primitive_set(std::move(ps));

  std::map<attrib_semantic, array_ptr> m;
  m.insert(std::make_pair(attrib_semantic::vertex, std::move(vertices)));
  if (has_normal())
    m.insert(std::make_pair(attrib_semantic::texcoord, std::move(texcoords)));
  return m;
}

//--------------------------------------------------------------------
common_geometry::vertex_build icosahedron::build_paper_unwrapper_mesh()
{
  auto vertices = std::make_unique<vec3_array>();
  
  vertices->resize(22); // pole points repeated 5 times, +2 center points
  // ten points in center
  GLfloat latitude = atan(0.5f);
  GLfloat step_angle = fpi * 72.0f / 180.0f;
  GLfloat up_start_angle = fpi2;
  GLfloat down_start_angle = up_start_angle + fpi*36.0f/180.0f;
  GLuint up_index = 5;
  GLuint down_index = up_index + 6;
  GLuint south_pole_index = down_index + 6;

  // north, south, pole
  for (int i = 0; i < 5; ++i)
  {
    vertices->at(i) = vec3(0, 0, 1);
    vertices->at(south_pole_index + i) = vec3(0, 0, -1);
  }

  // center
  for (int i = 0; i < 6; ++i) 
  {
    GLfloat z = sin(latitude);
    GLfloat proj = cos(latitude);
    GLfloat up_angle = i == 5 ? up_start_angle : up_start_angle + step_angle * i;
    GLfloat down_angle = i == 5 ? down_start_angle : down_start_angle + step_angle * i;
    vertices->at(up_index + i) = vec3(proj * cos(up_angle), proj * sin(up_angle) ,z);
    vertices->at(down_index + i) = vec3(proj * cos(down_angle), proj * sin(down_angle), -z);
  }

  auto elements = make_element<uint_array>();
  elements->reserve(60);

  // 5 triangles on top
  for (int i = 0; i < 5; ++i)
  {
    auto i1 = up_index + i;
    auto i2 = i1 + 1;
    elements->emplace_back(i);
    elements->emplace_back(i1);
    elements->emplace_back(i2);
  }
  
  // 10 triangles on center
  for (int i = 0; i < 5; ++i)
  {
    {
      auto i0 = up_index + i;
      auto i2 = i0 + 1;
      auto i1 = down_index + i;
      elements->emplace_back(i0);
      elements->emplace_back(i1);
      elements->emplace_back(i2);
    }
    {
      auto i0 = down_index + i;
      auto i2 = i0 + 1;
      auto i1 = up_index + 1 + i;
      elements->emplace_back(i0);
      elements->emplace_back(i2);
      elements->emplace_back(i1);
    }
  }

  // 5 triangles on bottom
  for (int i = 0; i < 5; ++i)
  {
    auto i1 = down_index + i;
    auto i2 = i1 + 1;
    elements->emplace_back(south_pole_index + i);
    elements->emplace_back(i2);
    elements->emplace_back(i1);
  }

  std::unique_ptr<vec2_array> texcoords;
  if(has_texcoord())
  {
    texcoords = std::make_unique<vec2_array>();
    texcoords->reserve(vertices->size());
    GLfloat step_s = 186.0f / 2048;
    GLfloat step_t = 322.0f / 1024;

    // t grow from bottom to up, different from the website
    for (int i = 0; i < 5; ++i)
      texcoords->push_back(vec2(step_s * (1 + i * 2), step_t * 3));
    for (int i = 0; i < 6; ++i)
      texcoords->push_back(vec2(step_s * i * 2, step_t * 2));
    for (int i = 0; i < 6; ++i)
      texcoords->push_back(vec2(step_s * (1 + i * 2), step_t));
    for (int i = 0; i < 5; ++i)
      texcoords->push_back(vec2(step_s * (2 + i * 2), 0));
  }

  for(auto& item : *vertices)
    item *= m_radius;

  for (int i = 0; i < m_subdivisions; ++i)
  {
    GLuint old_size = vertices->size();
    subdivide(elements->get_vector(), vertices->get_vector(),
      texcoords ? &texcoords->get_vector() : nullptr);

    std::for_each(vertices->begin() + old_size, vertices->end(), 
        [this](vec3& v)->void
        {
          v = glm::normalize(v) * m_radius;
        });
  }

  add_primitive_set(std::make_shared<draw_elements>(
    GL_TRIANGLES, elements->size(), GL_UNSIGNED_INT, 0));

  std::map<attrib_semantic, array_ptr> m;
  m.insert(std::make_pair(attrib_semantic::vertex, std::move(vertices)));
  if (has_normal())
    m.insert(std::make_pair(attrib_semantic::texcoord, std::move(texcoords)));
  return m;
}

}
