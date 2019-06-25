#include "xyplane.h"

namespace zxd
{

//--------------------------------------------------------------------
void xyplane::build_vertex()
{
  vec2_array& vertices = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&vertices));
  vertices.reserve((m_slice + 1) * (m_slice + 1));

  // build plane as triangle strip
  GLfloat xstep = m_width / m_slice;
  GLfloat ystep = m_height / m_slice;

  // build triangle strip as:
  //  0 2
  //  1 3
  for (int i = 0; i < m_slice; ++i) {  // row

    GLfloat y1 = m_bottom + ystep * i;
    GLfloat y0 = y1 + ystep;

    for (int j = 0; j <= m_slice; ++j) {  // col
      GLfloat x = m_left + xstep * j;

      vec2 v0(x, y0);
      vec2 v1(x, y1);
      vertices.push_back(v0);
      vertices.push_back(v1);
    }
  }

  m_primitive_sets.clear();
  GLuint row_size = (m_slice + 1) * 2;
  GLuint next = 0;
  for (int i = 0; i < m_slice; ++i, next += row_size)
  {
    add_primitive_set(new draw_arrays(GL_TRIANGLE_STRIP, next, row_size));
  }
}

//--------------------------------------------------------------------
void xyplane::build_normal()
{
  vec3_array& normals = *(new vec3_array());
  attrib_array(num_arrays(), array_ptr(&normals));
  normals.reserve(num_vertices());

  for (int i = 0; i < num_vertices(); ++i)
  {
    normals.push_back(vec3(0, 0, 1));
  }

  assert(normals.size() == num_vertices());
}

//--------------------------------------------------------------------
void xyplane::build_texcoord()
{
  vec2_array& texcoords = *(new vec2_array());
  attrib_array(num_arrays(), array_ptr(&texcoords));
  texcoords.reserve(num_vertices());

  for (int i = 0; i < m_slice; ++i) {  // row

    GLfloat t0 = static_cast<GLfloat>(i) / m_slice;
    GLfloat t1 = static_cast<GLfloat>(i + 1) / m_slice;

    for (int j = 0; j <= m_slice; ++j) {  // col
      GLfloat s = static_cast<GLfloat>(j) / m_slice;

      vec2 tex0(s, t0);
      vec2 tex1(s, t1);
      texcoords.push_back(tex0);
      texcoords.push_back(tex1);
    }
  }
  assert(texcoords.size() == num_vertices());
}

}
