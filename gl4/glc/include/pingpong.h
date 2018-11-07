#ifndef PINGPONG_H
#define PINGPONG_H
#include "glm.h"

class pingpong
{
protected:
  GLuint m_ping;
  GLuint m_pong;

public:
  pingpong(){}
  pingpong(GLuint r0, GLuint r1);

  GLuint ping() const { return m_ping; }
  void ping(GLuint v){ m_ping = v; }

  GLuint pong() const { return m_pong; }
  void pong(GLuint v){ m_pong = v; }

  void set(GLuint r0, GLuint r1);

  void swap();
};


#endif /* PINGPONG_H */
