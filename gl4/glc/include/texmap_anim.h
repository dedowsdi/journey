#ifndef GL_GLC_TEXMAP_ANIM_H
#define GL_GLC_TEXMAP_ANIM_H

#include <gl.h>
#include <common_program.h>
#include <quad.h>

namespace zxd
{

class texmap_anim
{
protected:
  GLuint m_num_row = -1;
  GLuint m_num_col = -1;
  GLuint m_frame_rate = 24;
  GLuint m_texture = -1;
  GLuint m_tex_index = 0;
  GLfloat m_position = 0;
  GLfloat m_cool_down = 0;

  mat4 m_tex_mat;
  
  texture_animation_program* m_prg;
  quad m_quad;

public:

  texmap_anim(){}

  texmap_anim(GLuint num_row, GLuint num_col, GLuint frame_rate = 24):
    m_num_row(num_row),
    m_num_col(num_col),
    m_frame_rate(frame_rate)
  {
  }

  void init();

  void update(GLfloat dt); 

  void draw(const mat4& mvp, GLint tex_index = 0);

  texture_animation_program* prg() const { return m_prg; }
  void prg(texture_animation_program* v){ m_prg = v; }

  GLuint tex_index() const { return m_tex_index; }
  void tex_index(GLuint v){ m_tex_index = v; }

  GLuint frame_rate() const { return m_frame_rate; }
  void frame_rate(GLuint v){ m_frame_rate = v;}

  GLuint num_row() const { return m_num_row; }
  void num_row(GLuint v){ m_num_row = v; }

  GLuint num_col() const { return m_num_col; }
  void num_col(GLuint v){ m_num_col = v; }

  GLuint texture() const { return m_texture; }
  void texture(GLuint v){ m_texture = v; }

  GLfloat position() const { return m_position; }
  void position(GLfloat v){ m_position = v; }

  GLuint num_frames()
  {
    return m_num_row * m_num_col;
  }

  GLfloat duration()
  {
    return num_frames() / static_cast<GLfloat>(m_frame_rate);
  }

  GLboolean finished()
  {
    return m_position > duration();
  }

};

}

#endif /* GL_GLC_TEXMAP_ANIM_H */
