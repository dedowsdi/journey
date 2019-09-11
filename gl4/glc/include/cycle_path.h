#ifndef GLC_CYCLE_PATH_H
#define GLC_CYCLE_PATH_H
#include <gl.h>
#include <glm.h>

namespace zxd
{

class cycle_path
{
public:
  cycle_path(
    GLfloat a0, GLfloat b0, GLfloat c0, GLfloat a1, GLfloat b1, GLfloat c1);

  glm::vec2 get(GLfloat angle);
  glm::vec2 tangent(GLfloat angle);

private:
  GLfloat m_a0, m_b0, m_c0;
  GLfloat m_a1, m_b1, m_c1;
};

}
#endif /* ifndef GLC_CYCLE_PATH_H */
