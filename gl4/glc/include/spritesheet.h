#ifndef GL_GLC_SPRITESHEET_H
#define GL_GLC_SPRITESHEET_H


#include <glm.h>

namespace zxd
{

class spritesheet
{
protected:
  GLuint m_rows = 0;
  GLuint m_cols = 0;
  GLuint m_tex = -1;

public:
  spritesheet(){}
  spritesheet(GLuint tex, GLuint rows, GLuint cols);

  glm::vec2 sprite_size();
  glm::vec2 get_sprite(GLuint row, GLuint col);
  glm::vec2 get_sprite(GLuint index);

  glm::vec2 get_random_sprite();

  GLuint rows() const { return m_rows; }
  void rows(GLuint v){ m_rows = v; }

  GLuint cols() const { return m_cols; }
  void cols(GLuint v){ m_cols = v; }

  GLuint tex() const { return m_tex; }
  void tex(GLuint v){ m_tex = v; }

};

}

#endif /* GL_GLC_SPRITESHEET_H */
