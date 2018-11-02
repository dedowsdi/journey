#include "texpack.h"

namespace zxd
{

//--------------------------------------------------------------------
texpack::texpack(GLuint tex, GLuint rows, GLuint cols):
  m_tex(tex), 
  m_rows(rows),
  m_cols(cols)
{

}

//--------------------------------------------------------------------
vec2 texpack::tile_size()
{
  return vec2(1.0/m_cols, 1.0/m_rows);
}

//--------------------------------------------------------------------
vec2 texpack::get_tile_at(GLuint row, GLuint col)
{
  return tile_size() * vec2(col, row);
}

//--------------------------------------------------------------------
vec2 texpack::get_tile_at(GLuint index)
{
  return get_tile_at(index/m_cols, index%m_cols);
}

//--------------------------------------------------------------------
vec2 texpack::get_random_tile()
{
  vec2 rc = glm::linearRand(ivec2(0), ivec2(m_rows-1, m_cols-1));
  return get_tile_at(rc.x, rc.y);
}
}
