#include "pacman.h"
#include "common_program.h"

namespace zxd
{

//--------------------------------------------------------------------
void pacman::init(std::shared_ptr<lightless_program> prg)
{
  m_mouth_angle = m_mouth_max;
  m_body.type(circle::FILL);
  m_body.slice(128);
  m_body.radius(m_radius);

  m_outline.type(circle::PIE_LINE);
  m_outline.slice(512);
  m_outline.radius(m_radius);

  current_frame(0);

  m_eye.type(circle::FILL);
  m_eye.slice(128);
  m_eye.radius(m_radius * 0.15f);
  m_eye.build_mesh();

  m_program = prg;
}

//--------------------------------------------------------------------
void pacman::update()
{
  current_frame(m_current_frame + 1);
}

//--------------------------------------------------------------------
void pacman::draw(const mat4& m_mat)
{
  m_program->use();
  mat4 mvp_mat = m_program->p_mat * m_program->v_mat * m_mat;
  
  glUniformMatrix4fv(m_program->ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
  glUniform4fv(m_program->ul_color, 1, glm::value_ptr(m_body_color));
  m_body.draw();

  glLineWidth(3);
  glEnable(GL_LINE_SMOOTH);
  glUniformMatrix4fv(m_program->ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
  glUniform4fv(m_program->ul_color, 1, glm::value_ptr(m_outline_color));
  m_outline.draw();

  mvp_mat = glm::translate(mvp_mat, vec3(0, m_radius * 0.4f,  0));
  glUniformMatrix4fv(m_program->ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
  glUniform4fv(m_program->ul_color, 1, glm::value_ptr(m_eye_color));
  m_eye.draw();

}

//--------------------------------------------------------------------
void pacman::set_mouth_angle(GLfloat angle)
{
  m_mouth_angle = angle;
  m_outline.start(angle * 0.5f);
  m_outline.end(f2pi - m_outline.start());
  m_outline.build_mesh();

  m_body.start(angle * 0.5f);
  m_body.end(f2pi - m_body.start());
  m_body.build_mesh();
}

//--------------------------------------------------------------------
void pacman::current_frame(GLuint v)
{
  m_current_frame = v % m_num_frames;
  float pos = (m_current_frame + 1) / static_cast<GLfloat>(m_num_frames);
  float pct = glm::abs(pos - 0.5f) * 2; // mouse closed at 0.5
  set_mouth_angle(glm::mix(m_mouth_min, m_mouth_max, pct));
}

}
