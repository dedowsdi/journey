// http://www.songho.ca/opengl/gl_sphere.html
#include "icosahedron.h"

#include <algorithm>

#include "functor.h"
#include "geometry_util.h"

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
void icosahedron::build_vertex()
{
  if(m_mesh_type == mesh_type::MININUM)
    build_minimun_mesh();
  else
    build_paper_unwrapper_mesh();
}

//--------------------------------------------------------------------
void icosahedron::build_normal()
{
  auto normals = std::make_shared<vec3_array>(*attrib_vec3_array(0));
  for(auto& item : *normals)
  {
    item /= m_radius;
  }
  attrib_array(1, normals);
}

//--------------------------------------------------------------------
void icosahedron::build_texcoord()
{
  if(m_mesh_type == mesh_type::PAPER_UNWRAPPER)
    return;

  //TODO find a better way to generate texcoords
  auto texcoords = std::make_shared<vec2_array>();
  texcoords->reserve(num_vertices());
  attrib_array(2, texcoords);

  const auto& vertices = attrib_vec3_array(0);
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
}

//--------------------------------------------------------------------
void icosahedron::build_minimun_mesh()
{
  attrib_array(0, std::make_shared<vec3_array>());
  auto& vertices = *attrib_vec3_array(0);
  
  vertices.resize(12); // pole points repeated 5 times, +2 center points
  // ten points in center
  GLfloat latitude = atan(0.5f);
  GLfloat step_angle = fpi * 72.0f / 180.0f;
  GLfloat up_start_angle = fpi2;
  GLfloat down_start_angle = up_start_angle + fpi*36.0f/180.0f;
  GLuint up_index = 1;
  GLuint down_index = up_index + 5;
  GLuint south_pole_index = down_index + 5;

  // north, south, pole
  vertices[0] = vec3(0, 0, 1);
  vertices[south_pole_index] = vec3(0, 0, -1);

  // center
  for (int i = 0; i < 5; ++i) 
  {
    GLfloat z = sin(latitude);
    GLfloat proj = cos(latitude);
    GLfloat up_angle = up_start_angle + step_angle * i;
    GLfloat down_angle = down_start_angle + step_angle * i;
    vertices[up_index + i] = vec3(proj * cos(up_angle), proj * sin(up_angle) ,z);
    vertices[down_index + i] = vec3(proj * cos(down_angle), proj * sin(down_angle), -z);
  }

  auto elements = std::make_shared<uint_array>(GL_ELEMENT_ARRAY_BUFFER);
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

  for(auto& item : vertices)
    item *= m_radius;
  
  for (int i = 0; i < m_subdivisions; ++i)
  {
    auto old_size = vertices.size();
    geometry_util::subdivide(elements->get_vector(), vertices.get_vector());
    std::for_each(vertices.begin() + old_size, vertices.end(), 
        [this](vec3& v)->void
        {
          v = glm::normalize(v) * m_radius;
        });
  }

  std::cout << "icosahedron total vertices : " << vertices.size() << std::endl;

  m_elements = elements;
  auto ps = new draw_elements(GL_TRIANGLES, m_elements->num_elements(), GL_UNSIGNED_INT, 0);
  ps->indices(&elements->front());
  add_primitive_set(ps);
}

//--------------------------------------------------------------------
void icosahedron::build_paper_unwrapper_mesh()
{
  attrib_array(0, std::make_shared<vec3_array>());
  auto& vertices = *attrib_vec3_array(0);
  
  vertices.resize(22); // pole points repeated 5 times, +2 center points
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
    vertices[i] = vec3(0, 0, 1);
    vertices[south_pole_index + i] = vec3(0, 0, -1);
  }

  // center
  for (int i = 0; i < 6; ++i) 
  {
    GLfloat z = sin(latitude);
    GLfloat proj = cos(latitude);
    GLfloat up_angle = i == 5 ? up_start_angle : up_start_angle + step_angle * i;
    GLfloat down_angle = i == 5 ? down_start_angle : down_start_angle + step_angle * i;
    vertices[up_index + i] = vec3(proj * cos(up_angle), proj * sin(up_angle) ,z);
    vertices[down_index + i] = vec3(proj * cos(down_angle), proj * sin(down_angle), -z);
  }

  auto elements = std::make_shared<uint_array>(GL_ELEMENT_ARRAY_BUFFER);
  elements->reserve(60);

  // 5 triangles on top
  for (int i = 0; i < 5; ++i)
  {
    auto i1 = up_index + i;
    auto i2 = i1 + 1;
    elements->push_back(i);
    elements->push_back(i1);
    elements->push_back(i2);
  }
  
  // 10 triangles on center
  for (int i = 0; i < 5; ++i)
  {
    {
      auto i0 = up_index + i;
      auto i2 = i0 + 1;
      auto i1 = down_index + i;
      elements->push_back(i0);
      elements->push_back(i1);
      elements->push_back(i2);
    }
    {
      auto i0 = down_index + i;
      auto i2 = i0 + 1;
      auto i1 = up_index + 1 + i;
      elements->push_back(i0);
      elements->push_back(i2);
      elements->push_back(i1);
    }
  }

  // 5 triangles on bottom
  for (int i = 0; i < 5; ++i)
  {
    auto i1 = down_index + i;
    auto i2 = i1 + 1;
    elements->push_back(south_pole_index + i);
    elements->push_back(i2);
    elements->push_back(i1);
  }

  if(this->include_texcoord())
  {
    auto texcoords = std::make_shared<vec2_array>();
    texcoords->reserve(num_vertices());
    attrib_array(2, texcoords);
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

  for(auto& item : vertices)
    item *= m_radius;

  for (int i = 0; i < m_subdivisions; ++i)
  {
    GLuint old_size = vertices.num_elements();
    geometry_util::subdivide(elements->get_vector(), vertices.get_vector(),
        include_texcoord() ? &attrib_vec2_array(2)->get_vector() : nullptr);

    std::for_each(vertices.begin() + old_size, vertices.end(), 
        [this](vec3& v)->void
        {
          v = glm::normalize(v) * m_radius;
        });
  }

  m_elements = elements;
  add_primitive_set(new draw_elements(GL_TRIANGLES, m_elements->num_elements(), GL_UNSIGNED_INT, 0));
}

}
