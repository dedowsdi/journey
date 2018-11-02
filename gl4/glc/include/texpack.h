#ifndef TEXPACK_H
#define TEXPACK_H

#include "glm.h"

namespace zxd
{

class texpack
{
protected:
  GLuint m_rows;
  GLuint m_cols;
  GLuint m_tex;

public:
  texpack(GLuint tex, GLuint rows, GLuint cols);

  vec2 tile_size();
  vec2 get_tile_at(GLuint row, GLuint col);
  vec2 get_tile_at(GLuint index);

  vec2 get_random_tile();

  GLuint rows() const { return m_rows; }
  void rows(GLuint v){ m_rows = v; }

  GLuint cols() const { return m_cols; }
  void cols(GLuint v){ m_cols = v; }

  GLuint tex() const { return m_tex; }
  void tex(GLuint v){ m_tex = v; }

};

}


#endif /* TEXPACK_H */
