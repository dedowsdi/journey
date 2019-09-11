#include <sstream>

#include <app.h>
#include <bitmap_text.h>
#include <common_program.h>
#include <fractal_spiral.h>
#include <debugger.h>
#include <pingpong.h>
#include <quad.h>
#include <circle.h>

#define WIDTH 720
#define HEIGHT 720

namespace zxd
{

const GLuint resolution = 256;
const GLuint num_vertices = 60000;
const GLfloat pen_width = 1.5;
GLfloat fractal_half_height;
GLuint build_eye_frames = 80;
GLuint draw_start = 0;
GLuint draw_step = num_vertices / build_eye_frames;

GLuint mfbo;
GLuint mtex;
GLuint fbo;
GLuint tex;
GLuint vao;
GLuint vbo;

vec2 target_pos;
vec2 bug_pos;
circle bug;

glm::mat4 v_mat;
glm::mat4 p_mat;

vec2_vector vertices;

struct eyeball
{
  mat4 m_mat;
  GLfloat r0;
  GLfloat r1;
  vec2 pos;
  vec2 pupil_pos;
};
std::vector<eyeball> eyeballs;

lightless_program ll_prg;

class eyeball_program : public program
{

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, {"#version 430 core\n #define NUM_EYEBALLS 2\n"},
        "shader4/eyeball.fs.glsl");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} prg;

class eyeball_app : public app
{

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key( GLFWwindow *wnd, int key, int scancode, int action, int mods) override;

  void glfw_mouse_move(GLFWwindow *wnd, double x, double y) override;

private:

  vec2 world_to_wnd(const vec2& v);
  GLfloat world_to_wnd(GLfloat radius);
  void resize_eyeball();

  std::shared_ptr<spiral_seed> m_seed;

};

void eyeball_app::init_info()
{
  app::init_info();
  m_info.title = "eyeball_app";
  m_info.wnd_width = WIDTH;
  m_info.wnd_height = HEIGHT;
  m_info.samples = 16;
  m_info.double_buffer = true;
  m_info.wm.x = 100;
  m_info.wm.y = 100;
}

void eyeball_app::create_scene()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
  glEnable(GL_LINE_SMOOTH);


  prg.init();
  p_mat = zxd::rect_ortho(100, 100, wnd_aspect());
  ll_prg.init();

  bug.radius(3);
  bug.build_mesh();

  // init fractal spiral vertices
  m_seed = std::make_shared<spiral_seed>();
  m_seed->origin_scale(1);
  m_seed->angular_scale(-2);
  m_seed->radius_scale(0.33);
  m_seed->angular_speed(0.02);
  m_seed->radius(100);
  m_seed->rose_n(32);
  m_seed->rose_d(1);
  m_seed->rose_offset(-0.1);
  m_seed->type(lissajous::LT_ROSE);
  m_seed->xscale(1);
  m_seed->yscale(1);
  m_seed->add_child();

  auto child = m_seed->child();
  child->origin_scale(-0.8);
  child->angular_scale(-2);
  child->radius_scale(1.33);

  fractal_half_height = m_seed->approximate_height(180) * 1.05f;

  vertices.reserve(num_vertices);
  auto pen = m_seed->get_pen();
  for (int i = 0; i < num_vertices; ++i)
  {
    m_seed->update(resolution);
    vertices.push_back(pen->pos());
  }

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2),
    value_ptr(vertices[0]), GL_STATIC_DRAW);

  glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
  glEnableVertexAttribArray(0);

  glGenFramebuffers(1, &mfbo);
  glGenTextures(1, &mtex);
  glGenFramebuffers(1, &fbo);
  glGenTextures(1, &tex);

  resize_eyeball();
}

void eyeball_app::update()
{
  vec2 trans = glm::diskRand(2.0f);
  if(glm::linearRand(0.0f, 1.0f) < 0.15f)
      trans *= 30;

  bug_pos += trans;
  if(bug_pos.x > 100) bug_pos.x -= 100;
  if(bug_pos.y > 100) bug_pos.y -= 100;
  if(bug_pos.x < -100) bug_pos.x += 100;
  if(bug_pos.y < -100) bug_pos.y += 100;

  target_pos = world_to_wnd(bug_pos);

  for (auto& eb : eyeballs)
  {
    eb.pupil_pos = target_pos;
    vec2 offset = eb.pupil_pos - eb.pos;
    GLfloat max_offset_length = eb.r1 - eb.r0;
    if (glm::length2(offset) > max_offset_length * max_offset_length)
    {
      eb.pupil_pos = eb.pos + glm::normalize(offset) * max_offset_length;
    }
  }
}

void eyeball_app::display()
{
  if(draw_start < num_vertices)
  {
    glBindFramebuffer(GL_FRAMEBUFFER, mfbo);

    ll_prg.use();

    glEnable(GL_BLEND);
    glBlendColor(0, 0, 0, 0.5);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

    GLuint start = draw_start == 0 ? draw_start : draw_start - 1;
    GLuint count = start + draw_step > num_vertices ? num_vertices - start : draw_step;

    glBindVertexArray(vao);
    for (const auto& eyeball : eyeballs)
    {
      glUniformMatrix4fv(ll_prg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat * eyeball.m_mat));
      glUniform4f(ll_prg.ul_color, 0, 1, 1, 1);
      glDrawArrays(GL_LINE_STRIP, start, count);
    }
    glDisable(GL_BLEND);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fbo);
    glBlitFramebuffer(0, 0, wnd_width(), wnd_height(), 0, 0, m_info.wnd_width, m_info.wnd_height,
        GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    draw_start += draw_step;
    draw_start = glm::min(draw_start, num_vertices);
    draw_quad(tex, 0);
  }
  else
  {
    prg.use();
    glUniform1i(0, 0);
    glUniform2f(1, wnd_width(), wnd_height());
    glBindTexture(GL_TEXTURE_2D, tex);
    prg.uniform1f("eyeballs[0].r0", eyeballs[0].r0);
    prg.uniform1f("eyeballs[0].r1", eyeballs[0].r1);
    prg.uniform2fv("eyeballs[0].pos", 1, glm::value_ptr(eyeballs[0].pos));
    prg.uniform2fv("eyeballs[0].pupil_pos", 1, glm::value_ptr(eyeballs[0].pupil_pos));
    prg.uniform1f("eyeballs[1].r0", eyeballs[1].r0);
    prg.uniform1f("eyeballs[1].r1", eyeballs[1].r1);
    prg.uniform2fv("eyeballs[1].pos", 1, glm::value_ptr(eyeballs[1].pos));
    prg.uniform2fv("eyeballs[1].pupil_pos", 1, glm::value_ptr(eyeballs[1].pupil_pos));
    draw_quad();
  }

  ll_prg.use();

  glUniformMatrix4fv(ll_prg.ul_mvp_mat, 1, 0, glm::value_ptr(glm::translate(p_mat, vec3(bug_pos, 0))));
  glUniform4f(ll_prg.ul_color, 0, 0, 1, 1);
  bug.draw();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "";
  m_text.print(ss.str(), 10, wnd_height()- 20);
  glDisable(GL_BLEND);
}

void eyeball_app::glfw_resize(GLFWwindow *wnd, int w, int h)
{
  app::glfw_resize(wnd, w, h);

  p_mat = p_mat = zxd::rect_ortho(100, 100, wnd_aspect());
  resize_eyeball();
}

void eyeball_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (key)
    {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;
      case GLFW_KEY_Q:
        draw_start = 0;
        glBindFramebuffer(GL_FRAMEBUFFER, mfbo);
        glClear(GL_COLOR_BUFFER_BIT);
        break;

      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void eyeball_app::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
  target_pos = vec2(x, glfw_to_gl(y));
}

vec2 eyeball_app::world_to_wnd(const vec2& v)
{
  mat4 m = zxd::compute_window_mat(0, 0, wnd_width(), wnd_height()) * p_mat;
  return vec2(m * vec4(v, 0, 1));
}

GLfloat eyeball_app::world_to_wnd(GLfloat radius)
{
  //mat4 m = zxd::compute_window_mat(0, 0, wnd_width(), wnd_height()) * p_mat;
  GLfloat double_right_inverse = 0.5f * p_mat[0][0];
  return radius * wnd_width() * double_right_inverse ;
}

void eyeball_app::resize_eyeball()
{

  draw_start = 0;

  // multisample rtt
  glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, mtex);
  glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, m_info.samples,
      GL_RGB, wnd_width(), wnd_height(), true);

  glBindFramebuffer(GL_FRAMEBUFFER, mfbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, mtex, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("incomplete frame buffer\n");

  // normal tex, will be bound to fbo to copy texture from multisample texture
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wnd_width(), wnd_height(),
      0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("incomplete frame buffer\n");

  glClear(GL_COLOR_BUFFER_BIT);

  eyeballs.clear();
  // two eyeballs
  vec2 eye_pos = {50.0f, 0.0f};
  GLfloat eye_size = 35;
  GLfloat eye_wnd_size = world_to_wnd(eye_size);
  GLfloat eye_scale = eye_size / fractal_half_height;
  mat4 m0 = glm::scale(glm::translate(vec3(eye_pos, 0)), vec3(eye_scale, eye_scale, 1));
  mat4 m1 = glm::scale(glm::translate(vec3(-eye_pos, 0)), vec3(eye_scale, eye_scale, 1));
  eyeballs.push_back({m0, eye_wnd_size * 0.2f, eye_wnd_size, world_to_wnd(eye_pos),  world_to_wnd(eye_pos)});
  eyeballs.push_back({m1, eye_wnd_size * 0.2f, eye_wnd_size, world_to_wnd(-eye_pos), -world_to_wnd(-eye_pos)});
}

}

int main(int argc, char *argv[])
{
  zxd::eyeball_app app;
  app.run();
}
