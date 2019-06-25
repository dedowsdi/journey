#include "filter.h"

#include <sstream>
#include <iostream>

#include "common_program.h"
#include "quad.h"
#include "pingpong.h"
#include "string_util.h"

namespace zxd
{

namespace
{

class color_brightness_program : public quad_program
{
public:
  GLint ul_threshold;
protected:
  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/filter_color_brightness.fs.glsl");
  }
  void bind_uniform_locations()
  {
    quad_program::bind_uniform_locations();
    uniform_location(&ul_threshold, "threshold");
  }
} filter_prg;

class gaussian_blur_program : public quad_program
{
public:
  GLint ul_horizontal;
  GLint ul_weights;

  GLint horizontal;
  GLint radius = 4;
  GLfloat mean = 0.0f;
  GLfloat deviation = 1.0f;
  GLfloat step = 0.5f;
  float weights[32];

  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    string_vector sv;
    sv.push_back("#version 430 core\n");
    sv.push_back("#define RADIUS " + string_util::to(radius) + "\n");
    attach(GL_FRAGMENT_SHADER, sv, "shader4/gaussian_blur.fs.glsl");
  }

  virtual void bind_uniform_locations()
  {
    quad_program::bind_uniform_locations();
    uniform_location(&ul_horizontal, "horizontal");
    uniform_location(&ul_weights, "weights");
  }

  void init()
  {
    quad_program::init();

    // blend between 0 and 1 2 3 4 ... -1 -2 -3 -4 ....
    GLfloat total_weights = 0;
    for (int i = 0; i < radius+1; ++i)
    {
      weights[i] = zxd::gaussian_weight(step * i, mean, deviation);
      total_weights += weights[i] * (i == 0 ? 1 : 2);
    }
    for (int i = 0; i < radius+1; ++i)
    {
      weights[i] /= total_weights;
      //std::cout << weights[i] << std::endl;
    }

    use();
    glUniform1fv(ul_weights, 32, weights);
  }
};

struct bloom_program : public program
{
  GLint ul_exposure;
  GLint ul_hdr_map;
  GLint ul_brightness_map;

  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/bloom.fs.glsl");
  }

  virtual void bind_uniform_locations()
  {
    uniform_location(&ul_exposure, "exposure");
    uniform_location(&ul_hdr_map, "hdr_map");
    uniform_location(&ul_brightness_map, "brightness_map");
  }

  virtual void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }
} bloom_prg;
}

//--------------------------------------------------------------------
void brightness_color_filter::filter(GLuint input_map, GLuint color_map, GLuint brightness_map)
{
  GLenum draw_buffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, draw_buffers);
  glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_map, 0);
  glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightness_map, 0);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("incomplete frame buffer\n");

  filter_prg.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, input_map);
  glUniform1i(filter_prg.ul_quad_map, 0);
  glUniform1f(filter_prg.ul_threshold, m_brightness_threshold);
  draw_quad();

  glDrawBuffers(1, draw_buffers);
}

//--------------------------------------------------------------------
gaussian_blur_filter::gaussian_blur_filter(int radius/* = 5*/, float mean/* = 0*/, float deviation/* = 1*/, float step/* = 0.5*/):
  m_radius(radius),
  m_mean(mean),
  m_deviation(deviation),
  m_step(step)
{
}

//--------------------------------------------------------------------
void gaussian_blur_filter::filter(GLuint input_map, pingpong& pp, GLint times)
{
  gaussian_blur_program* prg = static_cast<gaussian_blur_program*>(get_program().get());
  prg->use();
  for (int i = 0; i < times*2; ++i)
  {
    pp.swap();

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pp.pong(), 0);
    glBindTexture(GL_TEXTURE_2D, i == 0 ? input_map : pp.ping());
    bool horizontal = i&1;
    glUniform1i(prg->ul_quad_map, 0);
    glUniform1i(prg->ul_horizontal, horizontal);
    draw_quad();
  }
}

//--------------------------------------------------------------------
std::shared_ptr<program> gaussian_blur_filter::get_program()
{
  if(!m_program)
  {
    auto prg = std::make_shared<gaussian_blur_program>();
    m_program = prg;
    prg->mean = m_mean;
    prg->deviation = m_deviation;
    prg->step = m_step;
    prg->radius = m_radius;
  }
  return m_program;
}

//--------------------------------------------------------------------
void bloom(GLuint hdr_map, GLuint blured_brightness_map, GLfloat exposure/* = 2.5f*/)
{
  bloom_prg.use();
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, hdr_map);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, blured_brightness_map);

  glUniform1i(bloom_prg.ul_hdr_map, 0);
  glUniform1i(bloom_prg.ul_brightness_map, 1);
  glUniform1f(bloom_prg.ul_exposure, exposure);
  draw_quad();
}

}
