#include "app.h"
#include "bitmaptext.h"
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

#define WIDTH 640
#define HEIGHT 360

namespace zxd {

bool display_help = false;
lightless_program prg;
//GLint draw_start;
GLuint draw_count = 0;
GLuint slices = 1024;
GLuint draw_step = slices/16;
GLuint line_width = 4;
GLuint blur_times = 7;
GLfloat last_max_number = 2;
GLfloat max_number = 2;
GLfloat right = 2;
GLfloat right_step;
GLfloat birghtness_threshold = 0.01;
GLfloat bloom_exposure = 2.5;
int_vector sequence;
geometry_base mesh;

GLuint diffuse_map;
GLuint color_map;
GLuint brightness_map;
GLuint fbo;
pingpong blur_tex;

key_control_item* kci_blur_times;
key_control_item* kci_bloom_exposure;

quad q;

int_vector recaman_sequence(GLuint count)
{
  // the same number migght appear twise as you jump forward multiples than jump
  // back.
  int_vector res;
  res.reserve(count);

  std::set<int> used_numbers;

  res.push_back(0);
  used_numbers.insert(0);
  GLint next_step = 1;

  while(--count)
  {
    GLint n = res.back() - next_step;
    if(n < 0 || used_numbers.find(n) != used_numbers.end())
      n = res.back() + next_step;

    res.push_back(n);
    used_numbers.insert(n);
    ++next_step;
  }

  //for(auto n : res)
  //{
    //std::cout << n << std::endl;
  //}

  return res;
}

class filter_program : public quad_program
{
public:
  GLint ul_threshold;
protected:
  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/filter_color_brightness.fs.glsl");
  }
  void bind_uniform_locations()
  {
    quad_program::bind_uniform_locations();
    uniform_location(&ul_threshold, "threshold");
  }
} filter_prg;

class blur_program : public quad_program
{
public:
  GLint ul_horizontal;
  GLint horizontal;

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/gaussian_blur_0_1_0.5_4.fs.glsl");
  }

  virtual void bind_uniform_locations() {
    quad_program::bind_uniform_locations();
    uniform_location(&ul_horizontal, "horizontal");
  }
} blur_prg;

struct bloom_program : public program {
  GLint ul_exposure;
  GLint ul_hdr_map;
  GLint ul_brightness_map;

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/bloom.fs.glsl");
  }

  virtual void bind_uniform_locations() {
    uniform_location(&ul_exposure, "exposure");
    uniform_location(&ul_hdr_map, "hdr_map");
    uniform_location(&ul_brightness_map, "brightness_map");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }
} bloom_prg;


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

class recaman_sequence_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "recaman_sequence_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 16;
    m_info.decorated = false;
  }
  virtual void create_scene() {
    glfwSetWindowPos(m_wnd, 100, 100);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_MULTISAMPLE);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    sequence = recaman_sequence(150);

    circle unit_circle;
    unit_circle.type(circle::LINE);
    unit_circle.slice(slices);
    unit_circle.start(fpi);
    unit_circle.end(0);
    unit_circle.build_mesh();
    vec3_vector unit_vertices = geometry_util::vec2_vector_to_vec3_vector(
        unit_circle.attrib_vec2_array(0)->get_vector());

    vec2_array* vertices = new vec2_array();
    mesh.attrib_array(0, std::shared_ptr<vec2_array>(vertices));
    vertices->reserve((slices*1) * (sequence.size() - 1));
    // build mesh based on sequence
    
    vec4_array* colors = new vec4_array();
    mesh.attrib_array(1, std::shared_ptr<vec4_array>(colors));
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
      for (int i = 0; i < transformed_vertices.size(); ++i) {
        vertices->push_back(transformed_vertices[i].xy());
        colors->push_back(glm::mix(color0, color1, static_cast<GLfloat>(i)/transformed_vertices.size()));
      }
    }

    mesh.bind_and_update_buffer();
    mesh.add_primitive_set(new draw_arrays(GL_LINE_STRIP, 0, vertices->size()));

    prg.with_color = true;
    prg.init();
    prg.v_mat = mat4(1);

    filter_prg.init();
    blur_prg.init();
    bloom_prg.init();

    glGenFramebuffers(1, &fbo);
    color_map = create_texture();
    brightness_map = create_texture();
    diffuse_map = create_texture();

    q.include_texcoord(true);
    q.build_mesh();

    blur_tex.ping(create_texture());
    blur_tex.pong(create_texture());

    kci_blur_times = m_control.add_control(GLFW_KEY_Q, blur_times, 1, 99, 1);
    kci_bloom_exposure = m_control.add_control(GLFW_KEY_W, bloom_exposure, 1, 99, 0.1);
  }

  virtual void update() 
  {
    blur_times = kci_blur_times->value;
    bloom_exposure = kci_bloom_exposure->value;

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
    prg.p_mat = glm::ortho(0.0f, hw*2, -hh, hh, -1.0f, 1.0f);
    prg.mvp_mat = prg.p_mat * prg.v_mat;
  }

  virtual void display() {
    GLenum draw_buffers[2] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};

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
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.mvp_mat));
    glLineWidth(line_width);
    mesh.draw();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, WIDTH, HEIGHT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    // extract color and brightness
    glDrawBuffers(2, draw_buffers);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_map, 0);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightness_map, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");

    filter_prg.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glUniform1i(filter_prg.ul_quad_map, 0);
    glUniform1f(filter_prg.ul_threshold, birghtness_threshold);
    q.draw();

    // blur brightness texture
    glDrawBuffers(1, draw_buffers);
    blur_prg.use();
    for (int i = 0; i < blur_times*2; ++i) {
      blur_tex.swap();

      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, blur_tex.pong(), 0);
      glBindTexture(GL_TEXTURE_2D, i == 0 ? brightness_map : blur_tex.ping());
      bool horizontal = i&1;
      glUniform1i(blur_prg.ul_quad_map, 0);
      glUniform1i(blur_prg.ul_horizontal, horizontal);
      q.draw();
    }

    // draw final result
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //draw_quad(diffuse_map, 0);
    bloom_prg.use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, blur_tex.pong());

    glUniform1i(bloom_prg.ul_hdr_map, 0);
    glUniform1i(bloom_prg.ul_brightness_map, 1);
    glUniform1f(bloom_prg.ul_exposure, bloom_exposure);
    q.draw();

    //glClear(GL_COLOR_BUFFER_BIT);
    //prg.use();
    //mesh.draw();
    
    if(!display_help)
      return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps << std::endl;
    ss << "q : blur times " << blur_times << std::endl;
    ss << "w : bloom exposure " << bloom_exposure << std::endl;
    ss << "h : toggle help" << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
  }

  virtual void glfw_key(
      GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_H:
          display_help ^= 1;
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[]) {
  srand(time(0));
  zxd::recaman_sequence_app app;
  app.run();
}
