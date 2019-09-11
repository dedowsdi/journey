#include <beziersurface.h>

#include <glm/gtx/norm.hpp>
#include <bezier.h>

namespace zxd
{

//--------------------------------------------------------------------
bezier_surface::bezier_surface(GLfloat ubegin, GLfloat uend, GLfloat vbegin,
  GLfloat vend, GLuint upartition, GLuint vpartition)
    : m_ubegin(ubegin), m_uend(uend), m_vbegin(vbegin), m_vend(vend),
      m_upartition(upartition), m_vpartition(vpartition){};

//--------------------------------------------------------------------
common_geometry::vertex_build bezier_surface::build_vertices()
{
  auto vertices = std::make_unique<vec3_array>();

  vec3_vector2 q2 = u_interim2();

  // create triangle strip row by row
  // GLfloat ustep = (m_uend - m_ubegin) / m_upartition;
  GLfloat vstep = (m_vend - m_vbegin) / m_vpartition;

  for (GLuint i = 0; i < m_upartition; ++i)
  {
    const vec3_vector& q0 = q2[i];
    const vec3_vector& q1 = q2[i + 1];

    for (int j = 0; j <= m_vpartition; ++j)
    {
      GLfloat v = m_vbegin + vstep * j;
      glm::vec3 v0 = bezier::get(q0.begin(), q0.end(), v);
      glm::vec3 v1 = bezier::get(q1.begin(), q1.end(), v);
      vertices->push_back(v1);
      vertices->push_back(v0);
    }
  }

  clear_primitive_sets();
  GLuint strip_size = (m_vpartition + 1) * 2;
  for (GLuint i = 0; i < m_upartition; ++i)
  {
    add_primitive_set(std::make_shared<draw_arrays>(GL_TRIANGLE_STRIP, strip_size * i, strip_size));
  }

  return vertex_build{std::move(vertices)};
}

//--------------------------------------------------------------------
array_uptr bezier_surface::build_normals(const array& vertices)
{
  auto normals = std::make_unique<vec3_array>();

  // create triangle strip row by row
  GLfloat ustep = (m_uend - m_ubegin) / m_upartition;
  GLfloat vstep = (m_vend - m_vbegin) / m_vpartition;

  vec3_vector2 uq2 = u_interim2();
  vec3_vector2 vq2 = v_interim2();

  for (GLuint i = 0; i < m_upartition; ++i)
  {
    GLfloat u0 = m_ubegin + ustep * i;
    GLfloat u1 = u0 + ustep;
    const vec3_vector& uq0 = uq2[i];
    const vec3_vector& uq1 = uq2[i + 1];

    for (int j = 0; j <= m_vpartition; ++j)
    {
      GLfloat v = m_vbegin + vstep * j;
      const vec3_vector& vq = vq2[j];

      glm::vec3 right0 = bezier::tangent(uq0.begin(), uq0.end(), v);
      glm::vec3 right1 = bezier::tangent(uq1.begin(), uq1.end(), v);
      glm::vec3 front0 = bezier::tangent(vq.begin(), vq.end(), u0);
      glm::vec3 front1 = bezier::tangent(vq.begin(), vq.end(), u1);

      normals->push_back(normalize(cross(right1, front1)));
      normals->push_back(normalize(cross(right0, front0)));
    }
  }
  return normals;
}

//--------------------------------------------------------------------
array_uptr bezier_surface::build_texcoords(const array& vertices)
{
  auto texcoords = std::make_unique<vec2_array>();

  for (GLuint i = 0; i < m_upartition; ++i)
  {
    GLfloat y0 = static_cast<GLfloat>(i) / m_upartition;
    GLfloat y1 = static_cast<GLfloat>(i + 1) / m_upartition;
    for (int j = 0; j <= m_vpartition; ++j)
    {
      GLfloat x = static_cast<GLfloat>(j) / m_vpartition;
      texcoords->push_back(vec2(x, y1));
      texcoords->push_back(vec2(x, y0));
    }
  }
  return texcoords;
}

//--------------------------------------------------------------------
glm::vec3 bezier_surface::get(GLfloat u, GLfloat v)
{
  vec3_vector q = u_interim(u);
  return bezier::get(q.begin(), q.end(), v);
}

//--------------------------------------------------------------------
vec3_vector bezier_surface::u_interim(GLfloat u)
{
  vec3_vector q;

  for (int i = 0; i < vorder(); ++i)
  {
    const vec3_vector& ctrl_points = col(i);
    q.push_back(bezier::get(ctrl_points.begin(), ctrl_points.end(), u));
  }

  return q;
}

//--------------------------------------------------------------------
vec3_vector bezier_surface::v_interim(GLfloat v)
{
  vec3_vector q;

  for (int i = 0; i < uorder(); ++i)
  {
    vec3_vector ctrl_points = row(i);
    q.push_back(bezier::get(ctrl_points.begin(), ctrl_points.end(), v));
  }

  return q;
}

//--------------------------------------------------------------------
vec3_vector2 bezier_surface::u_interim2()
{
  vec3_vector2 vv;
  GLfloat ustep = (m_uend - m_ubegin) / m_upartition;
  for (int i = 0; i <= m_upartition; ++i)
  {
    GLfloat u = m_ubegin + ustep * i;
    vv.push_back(u_interim(u));
  }
  return vv;
}

//--------------------------------------------------------------------
vec3_vector2 bezier_surface::v_interim2()
{
  vec3_vector2 vv;
  GLfloat vstep = (m_vend - m_vbegin) / m_vpartition;
  for (int i = 0; i <= m_vpartition; ++i)
  {
    GLfloat v = m_vbegin + vstep * i;
    vv.push_back(v_interim(v));
  }
  return vv;
}

//--------------------------------------------------------------------
const vec3_vector& bezier_surface::col(GLuint i) { return m_ctrl_points[i]; }

//--------------------------------------------------------------------
vec3_vector bezier_surface::row(GLuint i)
{
  vec3_vector v;
  for (int j = 0; j < m_ctrl_points.size(); ++j)
  {
    v.push_back(m_ctrl_points[j][i]);
  }
  return v;
}

//--------------------------------------------------------------------
GLuint bezier_surface::uorder() { return m_ctrl_points.size(); }

//--------------------------------------------------------------------
GLuint bezier_surface::vorder()
{
  return m_ctrl_points.empty() ?: m_ctrl_points.front().size();
}
}
