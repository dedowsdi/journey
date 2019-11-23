#include <app.h>
#include <bitmap_text.h>
#include <common_program.h>
#include <lsystem.h>
#include <quad.h>
#include <pingpong.h>
#include <quad.h>
#include <sphere.h>
#include <light.h>
#include <debugger.h>
#include "lightning.h"
#include <filter.h>

#include <sstream>

#define WIDTH 512
#define HEIGHT 512

namespace zxd
{

bool verbose = false;
bool use_cpu = false;

glm::mat4 v_mat;
glm::mat4 p_mat;

GLuint ms_map;
GLuint diffuse_map;
GLuint color_map;
GLuint brightness_map;
GLuint mfbo;
GLuint fbo;
GLuint depth_map;
pingpong blur_tex;

GLfloat inner_ball_radius = 20;
GLfloat outer_ball_radius = 80;
GLuint outer_ball_slices = 16;
GLuint outer_ball_stacks = 16;
GLfloat sphere_billboard_width = 1;
GLfloat unstable_billboard_width = 2;
GLfloat inner_billboard_width = 1.5;
GLfloat pole_billboard_width = 6;
vec3_vector sphere_lines;

GLuint inner_lightning_count = 0;
GLuint blur_times = 7;
GLfloat birghtness_threshold = 0.01;
GLfloat bloom_exposure = 2.5;
GLuint unstable_start = 0;

brightness_color_filter bc_filter;
gaussian_blur_filter gb_filter(3, 0, 1.1);

sphere sphere0;

light_vector lights;
light_model lm;
material mtl;

blinn_program blinn_prg;

mat4 m_mat;
std::vector<lightning> lightnings;

kcip kci_blur_times;
kcip kci_bloom_exposure;

GLuint create_texture(GLvoid* data = 0);

class lightining_app : public app
{
protected:
  lsystem m_lsystem;

public:
  virtual void init_info();
  virtual void create_scene();

  virtual void update();

  virtual void display();

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h);

  virtual void glfw_key(
      GLFWwindow *wnd, int key, int scancode, int action, int mods);
};

void lightining_app::init_info()
{
  app::init_info();
  m_info.title = "lightining_app";
  m_info.wnd_width = WIDTH;
  m_info.wnd_height = HEIGHT;
  m_info.decorated = false;
  m_info.samples = 8;
}

void lightining_app::create_scene()
{
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

  glfwSetWindowPos(m_wnd, 100, 100);


  // scene
  sphere0.radius(inner_ball_radius);
  sphere0.slice(64);
  sphere0.stack(64);
  sphere0.build_mesh({attrib_semantic::vertex, attrib_semantic::normal});

  // light
  light_source l0;
  l0.position = vec4(0, 0, 1, 0);
  l0.diffuse = vec4(0.8);
  l0.specular = vec4(0.8);
  lights.push_back(l0);

  mtl.diffuse = vec4(0.8, 0.8, 0, 1);
  mtl.specular = vec4(0.3);
  mtl.shininess = 30;
  mtl.ambient = vec4(0.5);

  lm.local_viewer = true;
  lm.ambient = vec4(0.2);

  blinn_prg.init();
  blinn_prg.bind_lighting_uniform_locations(lights, lm, mtl);
  p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
  v_mat = zxd::isometric_projection(150.0f);
  set_v_mat(&v_mat);

  color_map = create_texture();
  brightness_map = create_texture();
  diffuse_map = create_texture();

  blur_tex.ping(create_texture());
  blur_tex.pong(create_texture());

  glGenTextures(1, &depth_map);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_map);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 
      m_info.samples, GL_DEPTH_COMPONENT32, WIDTH, HEIGHT, false);

  glGenTextures(1, &ms_map);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, ms_map);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_info.samples, GL_RGBA, WIDTH, HEIGHT, false);
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);

  glGenFramebuffers(1, &fbo);

  glGenFramebuffers(1, &mfbo);
  glBindFramebuffer(GL_FRAMEBUFFER, mfbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, ms_map, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_map, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("incomplete frame buffer\n");

  kci_blur_times = m_control.add<GLint>(GLFW_KEY_Q, blur_times, 1, 99, 1);
  kci_bloom_exposure = m_control.add<GLfloat>(GLFW_KEY_W, bloom_exposure, 1, 99, 0.1);

  // sphere lightning
  vec3_vector sphere_point = sphere::create_points( outer_ball_radius,
      outer_ball_slices, outer_ball_stacks);
  sphere_lines.reserve((outer_ball_stacks - 1) * (outer_ball_slices*2) 
      + outer_ball_slices * (outer_ball_stacks * 2));

  GLint stack_size = outer_ball_slices + 1;
  for (int i = 0; i < outer_ball_stacks; ++i) 
  {
    GLint stack_start = stack_size * i;
    GLint next_stack_start = stack_start + stack_size;
    for (int j = 0; j < outer_ball_slices; ++j) 
    {
      if(i != 0)
      {
        // latitude line
        sphere_lines.push_back(sphere_point[stack_start + j]);
        sphere_lines.push_back(sphere_point[stack_start + j + 1]);
      }
      // longtitude line
      sphere_lines.push_back(sphere_point[stack_start + j]);
      sphere_lines.push_back(sphere_point[next_stack_start + j]);
    }
  }

  lightnings.push_back(lightning("jjj",  sphere_lines, sphere_billboard_width, 0.5f, fpi4));
  // pole lightning
  lightnings.push_back(lightning("jjjjj", {vec3(0, 0, outer_ball_radius), vec3(0, 0, -outer_ball_radius)},
        pole_billboard_width, 0.15f, fpi4));
  unstable_start = lightnings.size();
}

void lightining_app::update() 
{
  if(m_frame_number % 2 != 0)
    return;

  blur_times = kci_blur_times->get_int();
  bloom_exposure = kci_bloom_exposure->get_float();

  GLfloat slice_per_second = 2.0/3.0;
  GLfloat angle_per_second = f2pi / outer_ball_slices * slice_per_second;
  m_mat = glm::rotate(static_cast<GLfloat>(m_current_time*angle_per_second), pza);

  lightnings.erase(lightnings.begin() + unstable_start, lightnings.end());
  // unstable lightning
  GLint unstable_lightning_count = glm::linearRand(1, 10);
  while(unstable_lightning_count--)
  {
    vec3 point = glm::sphericalRand(glm::linearRand(inner_ball_radius, outer_ball_radius*1.5f));
    GLint subdivides = glm::linearRand(3, 6);
    std::string pattern;
    for (int i = 0; i < subdivides; ++i)
      pattern.push_back(glm::linearRand(0.0f, 1.0f) < 0.5f ? 'f' : 'j');

    lightnings.push_back(lightning(pattern, {vec3(0), point}, unstable_billboard_width, 0.5f, fpi2));
    lightnings.back().set_use_cpu(use_cpu);
  }

  // inner lightning
  static vec4 perlin_seed = glm::linearRand(vec4(0), vec4(1000));
  static vec4 perlin_step = vec4(0.001);
  perlin_seed += perlin_step;
  for (int i = 0; i < inner_lightning_count; ++i)
  {
    GLfloat phi = glm::perlin(vec2(perlin_seed) + inner_lightning_count * 1.0f * vec2(200)) * fpi;
    GLfloat theta = glm::perlin(vec2(perlin_seed) + inner_lightning_count * 1.0f * vec2(200)) * f2pi;
    GLfloat cos_phi = cos(phi) ;
    GLfloat sin_phi = sin(phi) ;
    vec3 dir = vec3(sin_phi * cos(theta), sin_phi * sin(theta), cos_phi);

    // jitter dir to get target
    GLfloat jitter_angle = glm::linearRand(0.0f, fpi16);
    vec3 target_dir = dir * cos(jitter_angle)  + zxd::random_orthogonal(dir) * sin(jitter_angle) ;

    lightnings.push_back(lightning("fjjj",
          {dir * inner_ball_radius * 1.02f, target_dir * outer_ball_radius},
          inner_billboard_width, 0.5f, fpi4));
    lightnings.back().set_use_cpu(use_cpu);
  }

  vec3 camera_pos = zxd::eye_pos(v_mat * m_mat);
  for(auto& seed : lightnings)
    seed.update_buffer(camera_pos);

}

void lightining_app::display()
{
  // render scene
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  blinn_prg.use();
  blinn_prg.update_uniforms(m_mat, v_mat, p_mat);
  blinn_prg.update_lighting_uniforms(lights, lm, mtl, v_mat);

  // sphere0.draw();
  //debugger::draw_line(GL_LINES, sphere_lines, blinn_prg.mvp_mat);

  // blit depth buffer to mfbo
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, mfbo);
  glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  // use scene depth, draw multisample lightning in another fbo
  glBindFramebuffer(GL_FRAMEBUFFER, mfbo);
  glClearColor(0, 0, 0, 0);
  glClear(GL_COLOR_BUFFER_BIT); // leave depth

  mat4 mvp_mat = p_mat * v_mat * m_mat;
  for(auto& seed : lightnings)
    seed.draw(mvp_mat);

  glDisable(GL_BLEND);

  // blit final depth to default buffer
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);

  // copy multi sample lightning map to diffuse map
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
  glDisable(GL_DEPTH_TEST);
  glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, diffuse_map, 0);
  glBlitFramebuffer(0, 0, WIDTH, HEIGHT, 0, 0, WIDTH, HEIGHT, GL_COLOR_BUFFER_BIT, GL_LINEAR);

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  bc_filter.filter(diffuse_map, color_map, brightness_map);
  gb_filter.filter(brightness_map, blur_tex, blur_times);

  // draw final result
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  bloom(color_map, blur_tex.pong(), bloom_exposure);
  glDisable(GL_BLEND);

  glDisable(GL_CULL_FACE);
  glDisable(GL_DEPTH_TEST);
  glEnable(GL_BLEND);
  glBlendEquation(GL_FUNC_ADD);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  if (m_save_image)
  {
    save_colorbuffer("lightning.png");
    m_save_image = false;
  }

  if (!m_display_help)
  {
    return;
  }

  std::stringstream ss;
  if(verbose)
  {
    ss << "qQ : blur times : " << blur_times << std::endl;
    ss << "wW : bloom exposure : " << bloom_exposure << std::endl;
    ss << "u : use cpu : " << use_cpu << std::endl;
  }
  ss << "fps : " << m_fps << std::endl;
  m_text.print(ss.str(), 10, wnd_height()- 20);
  glDisable(GL_BLEND);
}

void lightining_app::glfw_resize(GLFWwindow *wnd, int w, int h)
{
  app::glfw_resize(wnd, w, h);
}

void lightining_app::glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (key)
    {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;
      case GLFW_KEY_P:
        for(auto& item : lightnings)
          item.debug_print_billboards(zxd::eye_pos(v_mat * m_mat));

        break;
      case GLFW_KEY_U:
        use_cpu ^= 1;
        for(auto& seed : lightnings)
          seed.set_use_cpu(use_cpu);

        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

GLuint create_texture(GLvoid* data)
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, WIDTH, HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  return tex;
}

}

int main(int argc, char *argv[])
{
  zxd::lightining_app app;
  app.run();
}
