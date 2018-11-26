#ifndef GL_GLC_DEBUGGER_H
#define GL_GLC_DEBUGGER_H
#include "gl.h"
#include "glm.h"
#include "common_program.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace debugger
{
  template<typename T>
  void draw_point(const T& point, const glm::mat4& mvp, 
      GLfloat size = 1,  const glm::vec4& color = glm::vec4(1));

  template<typename T>
  void draw_point(const std::vector<T>& vertices, const glm::mat4& mvp, 
      GLfloat size = 1, const glm::vec4& color = glm::vec4(1));

  template<typename It>
  void draw_point(It beg, It end, const glm::mat4& mvp, 
      GLfloat size = 1, const glm::vec4& color = glm::vec4(1));

  template<typename T>
  void draw_line(const T& p0, const T& p1, const glm::mat4& mvp,
      GLfloat width = 1, const glm::vec4& color = glm::vec4(1));

  template<typename T>
  void draw_line(GLenum mode, const std::vector<T>& vertices, const glm::mat4& mvp, 
      GLfloat width = 1, const glm::vec4& color = glm::vec4(1));

class debugger_program : public zxd::program
{
public:
  GLint ul_color;

  virtual void attach_shaders(){
    attach(GL_VERTEX_SHADER, "shader/debugger.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/debugger.fs.glsl");
  };

  virtual void bind_uniform_locations(){
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_color, "color");
  };

  virtual void bind_attrib_locations(){
    bind_attrib_location(0, "vertex");
  };
};


struct debugger_drawable
{
  GLuint vao = -1;
  GLuint vbo = -1;
  GLuint buffer_size = 1;

  void init_vao(GLuint components) ;
  void update_buffer(GLuint size, const GLvoid* data) ;

  void bind_vao();
};

//--------------------------------------------------------------------
template<typename T>
void draw_point(const T& point, const glm::mat4& mvp, GLfloat size/* = 1*/,  const glm::vec4& color/* = glm::vec4(1)*/)
{
  using namespace glm;
  static debugger_program prg;
  static debugger_drawable dd;

  if(!prg.is_inited())
    prg.init();

  dd.init_vao(T::components);
  dd.update_buffer(sizeof(T), glm::value_ptr(point));

  prg.use();
  glUniform4fv(prg.ul_color, 1, glm::value_ptr(color));
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp));

  dd.bind_vao();

  glPointSize(size);
  glDrawArrays(GL_POINTS, 0, 1);
}

//--------------------------------------------------------------------
template<typename T>
void draw_point(const std::vector<T>& vertices, const glm::mat4& mvp, GLfloat size/* = 1*/, const glm::vec4& color/* = glm::vec4(1)*/)
{
  draw_point(vertices.begin(), vertices.end(), mvp, size, color);
}

//--------------------------------------------------------------------
template<typename It>
void draw_point(It beg, It end, const glm::mat4& mvp, 
    GLfloat size/* = 1*/, const glm::vec4& color/* = glm::vec4(1)*/)
{
  using namespace glm;
  static debugger_program prg;
  static debugger_drawable dd;

  if(!prg.is_inited())
    prg.init();

  dd.init_vao(It::value_type::components);
  GLuint vertex_count = std::distance(beg, end);
  dd.update_buffer(sizeof(typename It::value_type) * vertex_count , &*beg);

  prg.use();
  glUniform4fv(prg.ul_color, 1, glm::value_ptr(color));
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp));

  dd.bind_vao();

  glPointSize(size);
  glDrawArrays(GL_POINTS, 0, vertex_count);
}

//--------------------------------------------------------------------
template<typename T>
void draw_line(const T& p0, const T& p1, const glm::mat4& mvp,
  GLfloat width/* = 1*/, const glm::vec4& color/* = glm::vec4(1)*/)
{

  using namespace glm;
  static debugger_program prg;
  static debugger_drawable dd;
  std::vector<T> vertices;
  vertices.push_back(p0);
  vertices.push_back(p1);

  if(!prg.is_inited())
    prg.init();

  dd.init_vao(T::components);
  dd.update_buffer(sizeof(T)*2, glm::value_ptr(vertices.front()));

  prg.use();
  glUniform4fv(prg.ul_color, 1, glm::value_ptr(color));
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp));

  glLineWidth(width);

  dd.bind_vao();
  glDrawArrays(GL_LINES, 0, 2);
}

//--------------------------------------------------------------------
template<typename T>
void draw_line(GLenum mode, const std::vector<T>& vertices, const glm::mat4& mvp, 
GLfloat width/* = 1*/, const glm::vec4& color/* = glm::vec4(1)*/)
{

  static debugger_program prg;
  static debugger_drawable dd;

  if(!prg.is_inited())
    prg.init();

  dd.init_vao(T::components);
  dd.update_buffer(sizeof(T) * vertices.size(), glm::value_ptr(vertices.front()));

  prg.use();
  glUniform4fv(prg.ul_color, 1, glm::value_ptr(color));
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp));

  dd.bind_vao();

  glLineWidth(width);
  glDrawArrays(mode, 0, vertices.size());
}


}


#endif /* GL_GLC_DEBUGGER_H */
