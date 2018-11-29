#ifndef GL_GLC_PINGPONG_H
#define GL_GLC_PINGPONG_H
#include "glm.h"

namespace zxd
{

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

  GLuint* pointer();
};

// n ping + 1 opong
class npingpong
{
protected:
  std::vector<GLuint> m_resources;
  GLuint m_current;

public:

  GLuint pong();
  GLuint last_ping();
  GLuint first_ping();
  GLuint get(GLuint index);
  GLuint size();
  void clear();
  void add_resource(GLuint resource);
  void shift();

};

}


#endif /* GL_GLC_PINGPONG_H */
