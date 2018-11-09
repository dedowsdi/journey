#include "spritesheet.h"

namespace zxd
{

//--------------------------------------------------------------------
spritesheet::spritesheet(GLuint tex, GLuint rows, GLuint cols):
  m_tex(tex), 
  m_rows(rows),
  m_cols(cols)
{

}

//--------------------------------------------------------------------
vec2 spritesheet::sprite_size()
{
  return vec2(1.0/m_cols, 1.0/m_rows);
}

//--------------------------------------------------------------------
vec2 spritesheet::get_sprite(GLuint row, GLuint col)
{
  return sprite_size() * vec2(col, row);
}

//--------------------------------------------------------------------
vec2 spritesheet::get_sprite(GLuint index)
{
  return get_sprite(index/m_cols, index%m_cols);
}

//--------------------------------------------------------------------
vec2 spritesheet::get_random_sprite()
{
  vec2 rc = glm::linearRand(ivec2(0), ivec2(m_rows-1, m_cols-1));
  return get_sprite(rc.x, rc.y);
}
}
