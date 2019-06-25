#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include <set>
#include "geometry.h"
#include "geometry_util.h"
#include <memory>
#include "circle.h"
#include "debugger.h"
#include "common_program.h"
#include "pingpong.h"
#include "quad.h"
#include "filter.h"

#define WIDTH 640
#define HEIGHT 360

namespace zxd
{

glm::mat4 m_mat;
glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 mvp_mat;

bool display_help = false;
lightless_program prg;
//GLint draw_start;
GLuint draw_count = 0;
GLuint slices = 1024;
GLuint draw_step = slices/16;
GLfloat line_width = 2.5;
GLint blur_times = 7;
GLuint numbers = 300;
GLfloat last_max_number = 2;
GLfloat max_number = 2;
GLfloat right = 2;
GLfloat right_step;
GLfloat birghtness_threshold = 0.01;
GLfloat bloom_exposure = 2.5;
int_vector sequence;
geometry_base mesh;

brightness_color_filter bc_filter;
gaussian_blur_filter gb_filter(3, 0, 1);

GLuint diffuse_map;
GLuint color_map;
GLuint brightness_map;
GLuint fbo;
pingpong blur_tex;

kcip kci_blur_times;
kcip kci_bloom_exposure;

quad q;

//  a(0) = 0; for n > 0, a(n) = a(n-1) - n if positive and not already in the
//  sequence, otherwise a(n) = a(n-1) + n.
int_vector recaman_sequence(GLuint count)
{
  // the same number might appear twise as you jump forward multiples times,
  // then jump back.
  int_vector res;
  res.reserve(count);

  std::set<int> used_numbers;

  res.push_back(0);
  used_numbers.insert(0);
  GLint n = 1;

  while(--count)
  {
    GLint an = res.back() - n;
    if(an < 0 || used_numbers.find(an) != used_numbers.end())
      an = res.back() + n;

    res.push_back(an);
    used_numbers.insert(an);
    ++n;
  }

  //for(auto n : res)
  //{
    //std::cout << n << std::endl;
  //}

  return res;
}

GLuint create_texture(GLvoid* data = 0)
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  return tex;
}

GLuint create_multi_sample_texture(GLvoid* data = 0)
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, tex);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 16, GL_RGB32F, WIDTH, HEIGHT, GL_TRUE);
  return tex;
}

class recaman_sequence_app : public app
{
protected:
  bitmap_text m_text;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "recaman_sequence_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 16;
    m_info.decorated = false;
    m_info.wm.x = 100;
    m_info.wm.y = 100;
  }
  virtual void create_scene()
  {
    glfwSetWindowPos(m_wnd, 100, 100);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    sequence = recaman_sequence(numbers);

    circle unit_circle;
    unit_circle.type(circle::LINE);
    unit_circle.slice(slices);
    unit_circle.start(fpi);
    unit_circle.end(0);
    unit_circle.build_mesh();
    vec3_vector unit_vertices = geometry_util::vec2_vector_to_vec3_vector(
        unit_circle.attrib_vec2_array(0)->get_vector());

    auto vertices = std::make_shared<vec2_array>();
    mesh.attrib_array(0, vertices);
    vertices->reserve((slices*1) * (sequence.size() - 1));
    // build mesh based on sequence
    
    auto colors = std::make_shared<vec4_array>();
    mesh.attrib_array(1, colors);
    colors->reserve(vertices->capacity());

    vec2 r = glm::linearRand(vec2(0), vec2(1000));
    for (int i = 0; i < sequence.size() - 1; ++i) 
    {
      GLint start = sequence[i];
      GLint end = sequence[i+1];
      GLfloat radius = abs((end - start) * 0.5f);
      GLfloat center = (end + start) * 0.5f;
      GLfloat pstep = 0.1;
      vec4 color0 = vec4(zxd::hsb2rgb(vec3(glm::perlin(r + vec2(i*pstep)), 1, 1)), 1);
      vec4 color1 = vec4(zxd::hsb2rgb(vec3(glm::perlin(r + vec2(i*pstep+pstep)), 1, 1)), 1);
      vec3 scale = vec3(radius, radius, 1);
      if(i&1)
        scale.y *= -1;
      if(end < start)
        scale.x *= -1;

      mat4 m = glm::scale(glm::translate(vec3(center, 0, 0)), scale);

      auto transformed_vertices = geometry_util::transform(unit_vertices, m);
      for (int i = 0; i < transformed_vertices.size(); ++i)
      {
        vertices->push_back(transformed_vertices[i].xy());
        colors->push_back(glm::mix(color0, color1, static_cast<GLfloat>(i)/transformed_vertices.size()));
      }
    }

    mesh.bind_and_update_buffer();
    mesh.add_primitive_set(new draw_arrays(GL_LINE_STRIP, 0, vertices->size()));

    prg.with_color = true;
    prg.init();
    v_mat = mat4(1);

    glGenFramebuffers(1, &fbo);
    color_map = create_texture();
    brightness_map = create_texture();
    diffuse_map = create_texture();

    q.include_texcoord(true);
    q.build_mesh();

    blur_tex.ping(create_texture());
    blur_tex.pong(create_texture());

    kci_blur_times = m_control.add_control(GLFW_KEY_Q, blur_times, 1, 99, 1);
    kci_bloom_exposure = m_control.add_control<GLfloat>(GLFW_KEY_W, bloom_exposure, 1, 99, 0.1);
  }

  virtual void update() 
  {
    blur_times = kci_blur_times->get_int();
    bloom_exposure = kci_bloom_exposure->get_float();

    draw_count += draw_step;

    draw_count = glm::min(mesh.num_vertices(), draw_count);
    draw_arrays* da = static_cast<draw_arrays*>(mesh.get_primitive_set(0));
    da->count(draw_count);

    GLint seq_idnex = zxd::ceil(static_cast<GLfloat>(draw_count)/(slices + 1));
    GLint n = sequence[seq_idnex];
    if(n > max_number)
    {
      right_step = (n - max_number)/20.0f;
      max_number = n;
    }

    if(right_step != 0)
    {
      last_max_number += right_step;
      last_max_number = std::min(max_number, last_max_number);

      if(last_max_number == max_number)
        right_step = 0;
    }

    GLfloat hw = last_max_number * 0.5f * 1.02;
    GLfloat hh = hw / wnd_aspect();
    p_mat = glm::ortho(0.0f, hw*2, -hh, hh, -1.0f, 1.0f);
    mvp_mat = p_mat * v_mat;
  }

  virtual void display()
  {
    //GLenum draw_buffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    // draw scene to diffuse_map
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //glDrawBuffers(1, draw_buffers);
    //glFramebufferTexture2D(
      //GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuse_map, 0);
    //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      //printf("incomplete frame buffer\n");

    // too bulky to get line smooth and multisample from rtt, so i use copy tex
    // image instead.
    glClear(GL_COLOR_BUFFER_BIT);
    prg.use();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glLineWidth(line_width);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mesh.draw();
    glDisable(GL_BLEND);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, WIDTH, HEIGHT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    bc_filter.filter(diffuse_map, color_map, brightness_map);
    gb_filter.filter(brightness_map, blur_tex, blur_times);

    // draw final result
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    bloom(color_map, blur_tex.pong(), bloom_exposure);
    
    if(!display_help)
      return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps << std::endl;
    ss << "q : blur times " << blur_times << std::endl;
    ss << "w : bloom exposure " << bloom_exposure << std::endl;
    ss << "h : toggle help" << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  virtual void glfw_key(
      GLFWwindow *wnd, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_H:
          display_help ^= 1;
          break;
        case GLFW_KEY_Q:
          draw_count = 0;
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[])
{
  srand(time(0));
  zxd::recaman_sequence_app app;
  app.run();
}
