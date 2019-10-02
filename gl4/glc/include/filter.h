#ifndef GL_GLC_FILTER_H
#define GL_GLC_FILTER_H
#include <gl.h>
#include <memory>

namespace zxd
{

class pingpong;

class brightness_color_filter
{
public:

  void filter(GLuint input_map, GLuint color_map, GLuint brightness_map);

  GLfloat brightness_threshold() const { return m_brightness_threshold; }
  void brightness_threshold(GLfloat v){ m_brightness_threshold = v; }

private:
  GLfloat m_brightness_threshold = 0.01;


};

class program;

class gaussian_blur_filter
{

public:
  gaussian_blur_filter(int radius = 5, float mean = 0, float deviation = 1, float step = 0.5);
  // final result in pong_map
  void filter(GLuint input_map, pingpong& pp, GLint times);

  GLint radius() const { return m_radius; }
  void radius(GLint v){ m_radius = v; }

  std::shared_ptr<program> get_program();

private:
  GLint m_radius;
  GLfloat m_mean;
  GLfloat m_deviation;
  GLfloat m_step;

  std::shared_ptr<program> m_program;
};

void bloom(GLuint hdr_map, GLuint blured_brightness_map, GLfloat exposure = 2.5f);

}

#endif /* GL_GLC_FILTER_H */
