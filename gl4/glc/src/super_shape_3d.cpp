#include "super_shape_3d.h"
#include <memory>
#include "geometry_util.h"

namespace zxd
{

//--------------------------------------------------------------------
void super_shape_3d::build_vertex()
{
  if(m_type == ST_SPHERE)
    build_sphere_vertex();
  else
    build_torus_vertex();

  if(m_include_normal)
    geometry_util::smooth(*this, 1);
}

//--------------------------------------------------------------------
void super_shape_3d::build_sphere_vertex()
{
  vec3_array_ptr vertices(new vec3_array);
  attrib_array(0, vertices); 
  GLuint circle_size = m_slice + 1;
  GLuint strip_size = circle_size * 2;
  vertices->reserve(m_stack * strip_size);

  GLfloat phi_step = (m_phi_end - m_phi_start) / m_stack;
  GLfloat theta_step = (m_theta_end - m_theta_start) / m_slice;

  vec3_vector points;
  points.reserve((m_stack + 1) * (m_slice + 1));
  vec2_vector point_texcoords;

  vec2_array_ptr texcoords(new vec2_array);
  if(m_include_texcoord)
  {
    attrib_array(2, texcoords);
    point_texcoords.reserve(points.size());
    texcoords->reserve(vertices->size());
  }

  // generate spherical points latitude by latitude
  // generate s along latitude line, t along longitude line
  GLfloat s_step = 1.0 / m_slice;
  GLfloat t_step = 1.0 / m_stack;
  for (int i = 0; i <= m_stack; ++i) {
    GLfloat phi = m_phi_start + phi_step * i; // latitude
    if(abs(phi - m_phi_start - f2pi) <= 0.00001)
      phi = m_phi_start;

    GLfloat t = t_step * i;

    GLfloat cos_phi = cos(phi);
    GLfloat sin_phi = sin(phi);
    GLfloat r2 = supershape(phi);
    GLfloat z = m_radius * r2 * sin_phi;

    for (int j = 0; j <= m_slice; ++j) {

      if(m_include_texcoord)
      {
        GLfloat s = j * s_step;
        point_texcoords.push_back(vec2(s, t));
      }

      if(abs(cos_phi) < 0.000001) // use exacty the same value for pole vertices
      {
        points.push_back(vec3(0, 0, z));
        continue;
      }

      GLfloat theta = m_theta_start + theta_step*j; // longitude
      if(abs(theta - m_theta_start - f2pi) <= 0.00001) // use exactly the same value for start and end
        theta = m_theta_start;

      GLfloat r1 = supershape(theta);

      GLfloat x = m_radius * r1 * cos(theta) * r2 * cos_phi;
      GLfloat y = m_radius * r1 * sin(theta) * r2 * cos_phi;

      points.push_back(vec3(x, y, z));
    }
  }

  m_primitive_sets.clear();

  // generate triangle strip phi by phi
  for (int i = 0; i < m_stack; ++i) {
    int stack_start0 = i * circle_size;
    int stack_start1 = stack_start0 + circle_size;

    for (int j = 0; j <= m_slice; ++j) {
      vertices->push_back(points[stack_start1 + j]);
      vertices->push_back(points[stack_start0 + j]);

      if(m_include_texcoord)
      {
        texcoords->push_back(point_texcoords[stack_start1 + j]);
        texcoords->push_back(point_texcoords[stack_start0 + j]);
      }
    }
    m_primitive_sets.push_back( std::make_shared<draw_arrays>(
          GL_TRIANGLE_STRIP, strip_size * i, strip_size));
  }

  std::cout << "create super sphere shape 3d in " << vertices->size() << " vertices" << std::endl;
}

//--------------------------------------------------------------------
void super_shape_3d::build_torus_vertex()
{
  vec3_array_ptr vertices(new vec3_array);
  attrib_array(0, vertices); 
  GLuint circle_size = side() + 1;
  GLuint strip_size = circle_size * 2;
  vertices->reserve(strip_size * ring());

  GLfloat phi_step = (m_phi_end - m_phi_start) / ring();
  GLfloat theta_step = (m_theta_end - m_theta_start) / side();

  vec3_vector points;
  points.reserve((ring() + 1) * (side() + 1));
  vec2_vector point_texcoords;

  vec2_array_ptr texcoords(new vec2_array);
  if(m_include_texcoord)
  {
    attrib_array(2, texcoords);
    point_texcoords.reserve(points.size());
    texcoords->reserve(vertices->size());
  }

  // create torus in the same manna as wikipedia:
  // ring by ring, use phi to rotate along z, use theta to rotate anlong
  // negative y.
  // http://paulbourke.net/geometry/supershape/#torus exchange phi and theta
  GLfloat s_step = 1.0 / side();
  GLfloat t_step = 1.0 / ring();
  for (int i = 0; i <= ring(); ++i) {
    GLfloat phi = m_phi_start + phi_step * i;
    if(abs(phi - m_phi_start - f2pi) <= 0.00001)
      phi = m_phi_start;
    GLfloat t = t_step * i;

    GLfloat cos_phi = cos(phi);
    GLfloat sin_phi = sin(phi);
    GLfloat r2 = supershape(phi);

    for (int j = 0; j <= side(); ++j) {

      if(m_include_texcoord)
      {
        GLfloat s = j * s_step;
        point_texcoords.push_back(vec2(s, t));
      }

      GLfloat theta = m_theta_start + theta_step*j;
      if(abs(theta - m_theta_start - f2pi) <= 0.00001)
        theta = m_theta_start;

      GLfloat r1 = supershape(theta);
      GLfloat sf0 = r1 + r2 * cos(theta);

      GLfloat x = m_radius * cos_phi * sf0;
      GLfloat y = m_radius * sin_phi * sf0;
      GLfloat z = m_radius * r2 * sin(theta);

      points.push_back(vec3(x, y, z));
      std::cout << points.back() << std::endl;
    }
  }

  m_primitive_sets.clear();

  // generate triangle strip phi by phi
  for (int i = 0; i < ring(); ++i) {
    int ring_start0 = circle_size * i;
    int ring_start1 = ring_start0 + circle_size;
    std::cout << ring_start0 << " : " << ring_start1 << std::endl;

    for (int j = 0; j <= side(); ++j) {
      // theata is along -y, so order must be changed
      vertices->push_back(points[ring_start0 + j]);
      vertices->push_back(points[ring_start1 + j]);

      if(m_include_texcoord)
      {
        texcoords->push_back(point_texcoords[ring_start0 + j]);
        texcoords->push_back(point_texcoords[ring_start1 + j]);
      }
    }
    m_primitive_sets.push_back( std::make_shared<draw_arrays>(GL_TRIANGLE_STRIP, 
          strip_size * i, strip_size));
  }


  std::cout << "create super torus shape 3d in " << vertices->size() << " vertices" << std::endl;
}

//--------------------------------------------------------------------
std::string super_shape_3d::shape_type_to_string(shape_type type)
{
  switch(type)
  {
    case ST_SPHERE:
      return "sphere";
    case ST_TORUS:
      return "torus";
  }
}

//--------------------------------------------------------------------
super_shape_3d::shape_type super_shape_3d::shape_type_from_string(const std::string& s)
{
  if(s == "sphere")
    return ST_SPHERE;
  else if(s == "torus")
    return ST_TORUS;

  throw std::runtime_error("unknown shape type " + s);
}

//--------------------------------------------------------------------
GLfloat super_shape_3d::supershape(GLfloat theta)
{
  if(m_a == 0 || m_b == 0)
    throw std::runtime_error("a or b can not be 0 in supershape3d");

  GLfloat cf0 = m_m * theta / 4;
  GLfloat p0 = pow(abs(1/m_a * cos(cf0)), m_n2);
  GLfloat p1 = pow(abs(1/m_b * sin(cf0)), m_n3);
  return pow(p0 + p1, -1/m_n1);
}

}
