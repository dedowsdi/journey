#ifndef REACTION_DIFFUISON_PACMAN_H
#include "circle.h"

namespace zxd
{

class lightless_program;

class pacman
{
public:
  void init(std::shared_ptr<lightless_program> prg);
  void update();
  void draw(const mat4& m_mat);

  float radius() const { return m_radius; }
  void radius(float v){ m_radius = v; }

  GLfloat get_mouth_angle(){return m_mouth_angle;}

  float mouth_max() const { return m_mouth_max; }
  void mouth_max(float v){ m_mouth_max = v; }

  float mouth_min() const { return m_mouth_min; }
  void mouth_min(float v){ m_mouth_min = v; }

  const vec4& body_color() const { return m_body_color; }
  void body_color(const vec4& v){ m_body_color = v; }

  const vec4& outline_color() const { return m_outline_color; }
  void outline_color(const vec4& v){ m_outline_color = v; }

  const vec4& eye_color() const { return m_eye_color; }
  void eye_color(const vec4& v){ m_eye_color = v; }

  GLuint num_frames() const { return m_num_frames; }
  void num_frames(GLuint v){ m_num_frames = v; }

  GLuint current_frame() const { return m_current_frame; }
  void current_frame(GLuint v);

private:
  void set_mouth_angle(GLfloat angle);

  GLuint m_num_frames = 20;
  GLuint m_current_frame = 0; // open at frame0, close at mid frame
  float m_mouth_min = 0.001;
  float m_mouth_max = fpi2;
  float m_mouth_angle;
  float m_radius = 1;

  vec4 m_body_color = vec4(1, 1, 0, 1);
  vec4 m_outline_color = vec4(0, 0, 0, 1);
  vec4 m_eye_color = vec4(0, 0, 0, 1);

  circle m_eye;
  circle m_outline;
  circle m_body;
  std::shared_ptr<lightless_program> m_program;
};

}

#define REACTION_DIFFUISON_PACMAN_H
#endif /* ifndef REACTION_DIFFUISON_PACMAN_H */
