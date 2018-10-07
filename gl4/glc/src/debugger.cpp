#include "debugger.h"
#include "program.h"

namespace debugger
{

class debugger_program : public zxd::program
{
public:
  GLint ul_color;

  virtual void attach_shaders(){
    attach(GL_VERTEX_SHADER, "data/shader/debugger_point.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/debugger_point.fs.glsl");
  };

  virtual void bind_uniform_locations(){
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_color, "color");
  };

  virtual void bind_attrib_locations(){
    bind_attrib_location(0, "vertex");
  };
} prg;

//--------------------------------------------------------------------
void draw_point(const glm::vec3& point, const glm::mat4& mvp, GLfloat size, const glm::vec4& color)
{
  using namespace glm;
  static GLuint vao = -1;
  static GLuint vbo = -1;

  if(vao == -1)
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3), 0, GL_DYNAMIC_DRAW);
    if(!prg.is_inited())
      prg.init();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  prg.use();
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp));

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3), value_ptr(point));

  GLfloat old_size;
  glGetFloatv(GL_POINT_SIZE, &old_size);
  glPointSize(size);

  glDrawArrays(GL_POINTS, 0, 1);

  glPointSize(old_size);
}

//--------------------------------------------------------------------
void draw_point(const zxd::vec3_vector& vertices, const glm::mat4& mvp, GLfloat size/* = 1*/, const glm::vec4& color/* = glm::vec4(1)*/)
{
  using namespace glm;
  static GLuint vao = -1;
  static GLuint vbo = -1;
  static GLuint buffer_size = sizeof(vec3);

  if(vao == -1)
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, 0, GL_DYNAMIC_DRAW);
    if(!prg.is_inited())
      prg.init();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  prg.use();
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp));

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  GLuint new_size = vertices.size() * sizeof(vec3);
  if(buffer_size == new_size)
    glBufferSubData(GL_ARRAY_BUFFER, 0, new_size, value_ptr(vertices[0]));
  else
    glBufferData(GL_ARRAY_BUFFER, new_size, value_ptr(vertices[0]), GL_DYNAMIC_DRAW);

  GLfloat old_size;
  glGetFloatv(GL_POINT_SIZE, &old_size);
  glPointSize(size);

  glDrawArrays(GL_POINTS, 0, vertices.size());

  glPointSize(old_size);
  buffer_size = new_size;
}

//--------------------------------------------------------------------
void draw_line(const glm::vec3& p0, const glm::vec3& p1, const glm::mat4& mvp, GLfloat width, const glm::vec4& color/* = glm::vec4(1)*/)
{
  using namespace glm;
  static GLuint vao = -1;
  static GLuint vbo = -1;

  if(vao == -1)
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vec3)*2, 0, GL_DYNAMIC_DRAW);
    if(!prg.is_inited())
      prg.init();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  prg.use();
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp));

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  vec3 vertices[2] = {p0, p1};

  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec3)*2, value_ptr(vertices[0]));

  GLfloat old_width;
  glGetFloatv(GL_LINE_WIDTH, &old_width);
  glLineWidth(width);

  glDrawArrays(GL_LINES, 0, 2);

  glLineWidth(old_width);
}

//--------------------------------------------------------------------
void draw_line(GLenum mode, const zxd::vec3_vector& vertices, const glm::mat4& mvp, GLfloat width/* = 1*/, const glm::vec4& color/* = glm::vec4(1)*/)
{
  using namespace glm;
  static GLuint vao = -1;
  static GLuint vbo = -1;
  static GLuint buffer_size = sizeof(vec3)* 2;

  if(vao == -1)
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer_size, 0, GL_DYNAMIC_DRAW);
    if(!prg.is_inited())
      prg.init();

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  prg.use();
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp));

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  GLuint new_size = vertices.size() * sizeof(vec3);
  if(buffer_size == new_size)
    glBufferSubData(GL_ARRAY_BUFFER, 0, new_size, value_ptr(vertices[0]));
  else
    glBufferData(GL_ARRAY_BUFFER, new_size, value_ptr(vertices[0]), GL_DYNAMIC_DRAW);

  GLfloat old_width;
  glGetFloatv(GL_LINE_WIDTH, &old_width);
  glLineWidth(width);

  glDrawArrays(mode, 0, vertices.size());

  glLineWidth(old_width);
  buffer_size = new_size;
}

}
