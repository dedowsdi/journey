#include "nurbsurface.h"
#include "nurb.h"

namespace zxd {

//--------------------------------------------------------------------
void nurb_surface::build_vertex() {
  vec4_array& vertices = *(new vec4_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve((m_upartition + 1) * (m_vpartition + 1));

  vec4_vector2 q2 = u_interim2();

  // create triangle strip row by row
  // GLfloat ustep = (m_uend - m_ubegin) / m_upartition;
  GLfloat vstep = (m_vend - m_vbegin) / m_vpartition;

  for (GLuint i = 0; i < m_upartition; ++i) {
    const vec4_vector& q0 = q2[i];
    const vec4_vector& q1 = q2[i + 1];

    for (int j = 0; j <= m_vpartition; ++j) {
      GLfloat v = m_vbegin + vstep * j;
      vec4 v0 = nurb::get(
        q0.begin(), q0.end(), m_vknots.begin(), m_vknots.end(), m_vdegree, v);
      vec4 v1 = nurb::get(
        q1.begin(), q1.end(), m_vknots.begin(), m_vknots.end(), m_vdegree, v);
      vertices.push_back(v1);
      vertices.push_back(v0);
    }
  }
}

//--------------------------------------------------------------------
void nurb_surface::build_normal() {
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(num_vertices());

  // create triangle strip row by row
  GLfloat ustep = (m_uend - m_ubegin) / m_upartition;
  GLfloat vstep = (m_vend - m_vbegin) / m_vpartition;

  vec4_vector2 uq2 = u_interim2();
  vec4_vector2 vq2 = v_interim2();

  for (GLuint i = 0; i < m_upartition; ++i) {
    GLfloat u0 = m_ubegin + ustep * i;
    GLfloat u1 = u0 + ustep;
    const vec4_vector& uq0 = uq2[i];
    const vec4_vector& uq1 = uq2[i + 1];

    for (int j = 0; j <= m_vpartition; ++j) {
      GLfloat v = m_vbegin + vstep * j;
      const vec4_vector& vq = vq2[j];

      vec3 right0 = nurb::tangent(
        uq0.begin(), uq0.end(), m_vknots.begin(), m_vknots.end(), m_vdegree, v);
      vec3 right1 = nurb::tangent(
        uq1.begin(), uq1.end(), m_vknots.begin(), m_vknots.end(), m_vdegree, v);
      vec3 front0 = nurb::tangent(
        vq.begin(), vq.end(), m_uknots.begin(), m_uknots.end(), m_udegree, u0);
      vec3 front1 = nurb::tangent(
        vq.begin(), vq.end(), m_uknots.begin(), m_uknots.end(), m_udegree, u1);

      normals.push_back(normalize(cross(right1, front1)));
      normals.push_back(normalize(cross(right0, front0)));
    }
  }
}

//--------------------------------------------------------------------
void nurb_surface::build_texcoord() {
  // QUES : even distribute ?.
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());

  for (GLuint i = 0; i < m_upartition; ++i) {
    GLfloat y0 = static_cast<GLfloat>(i) / m_upartition;
    GLfloat y1 = static_cast<GLfloat>(i + 1) / m_upartition;
    for (int j = 0; j <= m_vpartition; ++j) {
      GLfloat x = static_cast<GLfloat>(j) / m_vpartition;
      texcoords.push_back(vec2(x, y1));
      texcoords.push_back(vec2(x, y0));
    }
  }
}

//--------------------------------------------------------------------
void nurb_surface::draw(GLuint primcount /* = 1*/) {
  bind_vao();
  GLuint strip_size = (m_vpartition + 1) * 2;
  for (GLuint i = 0; i < m_upartition; ++i) {
    draw_arrays(GL_TRIANGLE_STRIP, strip_size * i, strip_size, primcount);
  }
}

//--------------------------------------------------------------------
vec4 nurb_surface::get(GLfloat u, GLfloat v) {
  vec4_vector q = u_interim(u);
  return nurb::get(
    q.begin(), q.end(), m_vknots.begin(), m_vknots.end(), m_vdegree, v);
}

//--------------------------------------------------------------------
vec4_vector nurb_surface::u_interim(GLfloat u) {
  vec4_vector q;

  for (int i = 0; i < vorder(); ++i) {
    const vec4_vector& ctrl_points = col(i);
    q.push_back(nurb::get(ctrl_points.begin(), ctrl_points.end(),
      m_uknots.begin(), m_uknots.end(), m_udegree, u));
  }

  return q;
}

//--------------------------------------------------------------------
vec4_vector nurb_surface::v_interim(GLfloat v) {
  vec4_vector q;

  for (int i = 0; i < uorder(); ++i) {
    vec4_vector ctrl_points = row(i);
    q.push_back(nurb::get(ctrl_points.begin(), ctrl_points.end(),
      m_vknots.begin(), m_vknots.end(), m_vdegree, v));
  }

  return q;
}

//--------------------------------------------------------------------
vec4_vector2 nurb_surface::u_interim2() {
  vec4_vector2 vv;
  GLfloat ustep = (m_uend - m_ubegin) / m_upartition;
  for (int i = 0; i <= m_upartition; ++i) {
    GLfloat u = m_ubegin + ustep * i;
    vv.push_back(u_interim(u));
  }
  return vv;
}

//--------------------------------------------------------------------
vec4_vector2 nurb_surface::v_interim2() {
  vec4_vector2 vv;
  GLfloat vstep = (m_vend - m_vbegin) / m_vpartition;
  for (int i = 0; i <= m_vpartition; ++i) {
    GLfloat v = m_vbegin + vstep * i;
    vv.push_back(v_interim(v));
  }
  return vv;
}

//--------------------------------------------------------------------
const vec4_vector& nurb_surface::col(GLuint i) { return m_ctrl_points[i]; }

//--------------------------------------------------------------------
vec4_vector nurb_surface::row(GLuint i) {
  vec4_vector v;
  for (int j = 0; j < m_ctrl_points.size(); ++j) {
    v.push_back(m_ctrl_points[j][i]);
  }
  return v;
}

//--------------------------------------------------------------------
void nurb_surface::uniform_knots() {
  uniform_uknots();
  uniform_vknots();
}

//--------------------------------------------------------------------
void nurb_surface::uniform_uknots() {
  GLuint m = un() + up() + 1;

  m_uknots.reserve(m + 1);
  GLuint s = up() + 1;
  GLint c = m + 1 - 2 * s;

  for (GLuint i = 0; i < s; ++i) m_uknots.push_back(0);
  for (GLuint i = 0; i < c; ++i)
    m_uknots.push_back(static_cast<GLfloat>((i + 1)) / (c + 1));
  for (GLuint i = 0; i < s; ++i) m_uknots.push_back(1);
}

//--------------------------------------------------------------------
void nurb_surface::uniform_vknots() {
  GLuint m = vn() + vp() + 1;

  m_vknots.reserve(m + 1);
  GLuint s = vp() + 1;
  GLint c = m + 1 - 2 * s;

  for (GLuint i = 0; i < s; ++i) m_vknots.push_back(0);
  for (GLuint i = 0; i < c; ++i)
    m_vknots.push_back(static_cast<GLfloat>((i + 1)) / (c + 1));
  for (GLuint i = 0; i < s; ++i) m_vknots.push_back(1);
}
}
