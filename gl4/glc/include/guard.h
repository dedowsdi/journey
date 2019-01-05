#ifndef GL4_GLC_GUARD_H
#define GL4_GLC_GUARD_H

#include <iostream>
#include "gl.h"
#include <glm/vec4.hpp>

namespace zxd
{

using namespace glm;

// precision and fmtflag
class raii_io_pflags
{
public:
  raii_io_pflags(std::ios_base& io);
  ~raii_io_pflags();

  // no copy
  raii_io_pflags(const raii_io_pflags& rhs) = delete;
  raii_io_pflags& operator=(raii_io_pflags rhs) = delete;

private:
  std::ios_base& m_io;
  std::streamsize m_precision;
  std::ios_base::fmtflags m_flags;
};

class viewport_guard
{
public:
  viewport_guard(GLint x, GLint y, GLsizei width, GLsizei height);
  ~viewport_guard();

  // no copy
  viewport_guard(const viewport_guard& rhs) = delete;
  viewport_guard& operator=(viewport_guard rhs) = delete;
private:
  ivec4 m_original;
};

class fbo_guard
{
public:
  fbo_guard(GLenum target, GLuint fbo);
  ~fbo_guard();
  
  // no copy
  fbo_guard(const fbo_guard& rhs) = delete;
  fbo_guard& operator=(fbo_guard rhs) = delete;
private:
  GLint m_original_draw;
  GLint m_original_read;
};


}
#endif /* GL4_GLC_GUARD_H */
