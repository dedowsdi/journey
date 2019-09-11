#include <guard.h>
#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace zxd
{

//--------------------------------------------------------------------
raii_io_pflags::raii_io_pflags(std::ios_base& io):
  m_io(io)
{
  m_flags = m_io.flags();
  m_precision = m_io.precision();
}

//--------------------------------------------------------------------
raii_io_pflags::~raii_io_pflags()
{
  m_io.flags(m_flags);
  m_io.precision(m_precision);
}

//--------------------------------------------------------------------
viewport_guard::viewport_guard(GLint x, GLint y, GLsizei width, GLsizei height)
{
  glGetIntegerv(GL_VIEWPORT, value_ptr(m_original));
  glViewport(x, y, width, height);
}

//--------------------------------------------------------------------
viewport_guard::~viewport_guard()
{
  glViewport(m_original.x, m_original.y, m_original.z, m_original.w);
}

//--------------------------------------------------------------------
fbo_guard::fbo_guard(GLenum target, GLuint fbo):
  m_original_read(-1),
  m_original_draw(-1)
{
  if(target == GL_FRAMEBUFFER || target == GL_DRAW_FRAMEBUFFER)
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &m_original_draw);
  if(target == GL_FRAMEBUFFER || target == GL_READ_FRAMEBUFFER)
    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &m_original_read);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
}

//--------------------------------------------------------------------
fbo_guard::~fbo_guard()
{
  if(m_original_read != -1)
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_original_draw);
  if(m_original_draw != -1)
    glBindFramebuffer(GL_READ_FRAMEBUFFER, m_original_read);
}

}
