#ifndef GL4_GLC_POINT_H
#define GL4_GLC_POINT_H

#include <glm.h>

namespace gm
{
  using namespace glm;

  class point_2d
  {
  public:
    GLfloat x;
    GLfloat y;

    point_2d(GLfloat x_, GLfloat y_);

    point_2d reflect() const;

    GLfloat length() const;

  };
}

#endif /* GL4_GLC_POINT_H */
