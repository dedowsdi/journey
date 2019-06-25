#include "primitive_set.h"
#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd
{

//--------------------------------------------------------------------
primitive_set::primitive_set(GLenum mode, GLuint num_instance):
  m_mode(mode),
  m_num_instance(num_instance)
{
}

//--------------------------------------------------------------------
draw_arrays::draw_arrays(GLenum mode, GLint first, GLsizei count, GLuint num_instance/* = 0*/):
  primitive_set(mode, num_instance),
  m_first(first),
  m_count(count)
{
}

//--------------------------------------------------------------------
void draw_arrays::draw()
{
  if (m_num_instance == 0)
  {
    glDrawArrays(m_mode, m_first, m_count);
  } else
  {
#ifdef GL_VERSION_3_0
    glDrawArraysInstanced(m_mode, m_first, m_count, m_num_instance);
#else
    glDrawArraysInstancedARB(m_mode, m_first, m_count, m_num_instance);
#endif
  }
}

//--------------------------------------------------------------------
void draw_arrays::accept(primitive_functor& functor) const
{
  functor.drawArrays(m_mode, m_first, m_count);
}

//--------------------------------------------------------------------
draw_elements::draw_elements(GLenum mode, GLint count, GLenum type, GLint offset, GLuint num_instance):
  primitive_set(mode, num_instance),
  m_count(count),
  m_type(type),
  m_offset(offset)
{
}

//--------------------------------------------------------------------
void draw_elements::draw()
{
  if (m_num_instance == 0)
  {
    glDrawElements(m_mode, m_count, m_type, BUFFER_OFFSET(m_offset));
  } else
  {
#ifdef GL_VERSION_3_0
    glDrawElementsInstanced(m_mode, m_count, m_type, BUFFER_OFFSET(m_offset), m_num_instance);
#else
    glDrawElementsInstancedARB(m_mode, m_count, m_type, BUFFER_OFFSET(m_offset), m_num_instance);
#endif
  }
}

//--------------------------------------------------------------------
void draw_elements::accept(primitive_functor& functor) const
{
  functor.drawElements(m_mode, m_count, m_indices);
}

}
