#include "texmap_anim.h"
#include <glm/gtx/transform.hpp>

namespace zxd
{

//--------------------------------------------------------------------
void texmap_anim::init()
{
  m_quad.include_texcoord(GL_TRUE);
  m_quad.build_mesh();

  m_cool_down = 1.0 / m_frame_rate; 

  GLfloat sx = 1.0 / m_num_col;
  GLfloat sy = 1.0 / m_num_row;

  m_tex_mat = glm::scale(vec3(sx, sy, 1));
}

//--------------------------------------------------------------------
void texmap_anim::update(GLfloat dt)
{
  m_position += dt;
  m_cool_down -= dt;
  if(m_cool_down < 0)
  {
    m_cool_down += 1.0 / m_frame_rate;
    m_tex_index = ++m_tex_index % (m_num_row * m_num_col);

    GLfloat sx = 1.0 / m_num_col;
    GLfloat sy = 1.0 / m_num_row;

    GLint row =  m_tex_index / m_num_col;
    GLint col = m_tex_index % m_num_col;

    m_tex_mat = glm::translate(vec3(col * sx, row * sy, 0)) * glm::scale(vec3(sx, sy, 1));
  }
}

//--------------------------------------------------------------------
void texmap_anim::draw(const mat4& mvp, GLint tex_index)
{
  m_prg->use();

  if(m_texture != -1)
  {
    glBindTexture(GL_TEXTURE_2D, m_texture);
  }

  glUniformMatrix4fv(m_prg->ul_mvp_mat, 1, 0, glm::value_ptr(mvp));
  glUniformMatrix4fv(m_prg->ul_tex_mat, 1, 0, glm::value_ptr(m_tex_mat));
  glUniform1i(m_prg->ul_diffuse_map, tex_index);

  m_quad.draw();
}

}
