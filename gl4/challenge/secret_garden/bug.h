#ifndef SECRET_GARDEN_BUG_H
#define SECRET_GARDEN_BUG_H value

#include "gl.h"
#include "glm.h"

namespace zxd
{

class bug
{

public:
  bug(GLuint wing_count = 50, GLuint vertex_count = 20);

  void update() ;
  void draw() ;

private:

  void update_buffer(const vec3_vector& vertices, const vec3_vector& color) ;

  GLint m_wing_count;
  GLint m_vertex_count; // per wing
  GLfloat m_size = 10;;

  GLuint m_vao;
  GLuint m_vbo;
  GLuint m_ebo;
};

}

#endif /* ifndef SECRET_GARDEN_BUG_H */
