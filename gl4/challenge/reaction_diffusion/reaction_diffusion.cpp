#include <limits>
#include <sstream>

#include "app.h"
#include "bitmaptext.h"
#include "quad.h"
#include "common_program.h"
#include "pingpong.h"
#include "quad.h"
#include "circle.h"
#include "pacman.h"
#include "cycle_path.h"

#define WIDTH 720
#define HEIGHT 720
#define TEX_WIDTH WIDTH*1
#define TEX_HEIGHT HEIGHT*1

namespace zxd {

// ping pong
pingpong tex;
pingpong fbo;
circle circle0;
bool add_pacman = true;

bool display_help = true;
GLfloat da = 1;
GLfloat db = 0.5;
GLfloat feed = 0.0297;
GLfloat kill = 0.0605;
GLfloat delta_time = 1;
GLuint speed = 20;

key_control_item* kci_da;
key_control_item* kci_db;
key_control_item* kci_feed;
key_control_item* kci_kill;
key_control_item* kci_delta_time;
key_control_item* kci_speed;
cycle_path pacman_path(1, 4, 1, 4, 2, 1);

std::shared_ptr<lightless_program> ll_prg;
mat4 pacman_m_mat;

void create_circle_seeds(const vec2& pos, float size, bool clear = true)
{
  glBindFramebuffer(GL_FRAMEBUFFER, fbo.pong());
  glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
  if(clear)
    glClear(GL_COLOR_BUFFER_BIT);

  ll_prg->use();
  mat4 mvp_mat = glm::scale(glm::translate(ll_prg->p_mat, vec3(pos, 0)), vec3(size, size, 1));
  glUniformMatrix4fv(ll_prg->ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
  glUniform4f(ll_prg->ul_color, 1, 1, 0, 0);

  circle0.draw();

  fbo.swap();
  tex.swap();
};

class reaction_diffusion_program : public program
{

public:
  GLint ul_diffuse_map;
  GLint ul_da;
  GLint ul_db;
  GLint ul_feed;
  GLint ul_kill;
  GLint ul_delta_time;
  GLint ul_time;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader/reaction_diffusion.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/reaction_diffusion.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_da        ,  "da");
    uniform_location(&ul_db        ,  "db");
    uniform_location(&ul_feed      ,  "feed");
    uniform_location(&ul_kill      ,  "kill");
    uniform_location(&ul_delta_time,  "delta_time");
    uniform_location(&ul_time,  "time");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }
} prg;

class reaction_diffusion_draw_program : public program
{

public:
  GLint ul_quad_map;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader/reaction_diffusion_draw.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/reaction_diffusion_draw.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_quad_map, "quad_map");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }
} prg1;

class reaction_diffusion_app : public app {
protected:
  bitmap_text m_text;
  pacman m_pacman;

public:
  virtual void init_info();
  virtual void create_scene();

  void update();

  void replay();

  void display();

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h);

  virtual void glfw_key(
      GLFWwindow *wnd, int key, int scancode, int action, int mods);

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y);
};

void reaction_diffusion_app::init_info() {
  app::init_info();
  m_info.title = "reaction diffusion";
  m_info.wnd_width = 0;
  m_info.wnd_height = 0;
  m_info.wnd_width = WIDTH;
  m_info.wnd_height = HEIGHT;
  m_info.wm.x = 100;
  m_info.wm.y = 100;
  m_info.samples = 16;
  //m_info.fullscreen = GL_TRUE;
}
void reaction_diffusion_app::create_scene() {

  m_text.init();
  m_text.reshape(m_info.wnd_width, m_info.wnd_height);

  prg.init();
  prg1.init();

  ll_prg = std::make_shared<lightless_program>();
  ll_prg->init();
  ll_prg->p_mat = zxd::rect_ortho(100, 100, wnd_aspect());

  m_pacman.radius(15);
  m_pacman.init(ll_prg);
  m_pacman.outline_color(vec4(0,0,0,1));
  m_pacman.body_color(vec4(1,1,1,1));
  m_pacman.eye_color(vec4(0,0,0,1));

  glGenFramebuffers(2, fbo.pointer());
  glGenTextures(2, tex.pointer());

  for (int i = 0; i < 2; ++i) 
  {
    glBindTexture(GL_TEXTURE_2D, tex[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16, TEX_WIDTH, TEX_HEIGHT, 0, GL_RG, GL_UNSIGNED_SHORT, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);
    glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[i], 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer%i\n", fbo[i]);
  }

  auto update_value = [&](const key_control_item* item){
    da         = kci_da->value;
    db         = kci_db->value;
    feed       = kci_feed->value;
    kill       = kci_kill->value;
    delta_time = kci_delta_time->value;
    speed      = kci_speed->value;
  };

  kci_da = m_control.add_control(GLFW_KEY_Q, da, -10, 10, 0.1, update_value);
  kci_db = m_control.add_control(GLFW_KEY_W, db, -10, 10, 0.1, update_value);
  kci_feed = m_control.add_control(GLFW_KEY_E, feed, -10, 10, 0.001, update_value);
  kci_kill = m_control.add_control(GLFW_KEY_R, kill, -10, 10, 0.001, update_value);
  kci_delta_time = m_control.add_control(GLFW_KEY_U, delta_time, -10, 10, 0.1, update_value);
  kci_speed = m_control.add_control(GLFW_KEY_I, speed, 1, 50, 1, update_value);

  circle0.type(circle::FILL);
  circle0.slice(256);
  circle0.radius(1);
  circle0.build_mesh();

  replay();
}

void reaction_diffusion_app::update() {

  if(!add_pacman)
    return;

  m_pacman.update();
  GLfloat t = m_current_time * f2pi / 12;
  vec2 pos = pacman_path.get(t) * 85.0f;
  vec2 tangent = pacman_path.tangent(t);

  //create_circle_seeds(pos, m_pacman.radius(), false);

  // draw b as pacman
  pacman_m_mat = glm::rotate(glm::translate(vec3(pos, 0)), glm::atan(tangent.y, tangent.x), pza);
  vec4 body_color = m_pacman.body_color();
  vec4 eye_color = m_pacman.eye_color();
  vec4 outline_color = m_pacman.outline_color();

  m_pacman.body_color(vec4(1, 1, 1, 1));
  m_pacman.eye_color(vec4(1, 1, 1, 1));
  m_pacman.outline_color(vec4(1, 1, 1, 1));

  glBindFramebuffer(GL_FRAMEBUFFER, fbo.pong());

  m_pacman.draw(pacman_m_mat * glm::scale(vec3(0.85, 0.85, 1)));

  fbo.swap();
  tex.swap();

  m_pacman.body_color(body_color);
  m_pacman.eye_color(eye_color);
  m_pacman.outline_color(outline_color);
}

void reaction_diffusion_app::replay()
{
  // create seeds in both texture
  create_circle_seeds(vec2(0), 20);
  create_circle_seeds(vec2(0), 20);
}

void reaction_diffusion_app::display() {

  for (int i = 0; i < speed; ++i) 
  {
    glBindTexture(GL_TEXTURE_2D, tex.ping());
    glBindFramebuffer(GL_FRAMEBUFFER, fbo.pong());

    prg.use();

    glUniform1i(prg.ul_diffuse_map, 0);
    glUniform1f(prg.ul_da        ,  da);
    glUniform1f(prg.ul_db        ,  db);
    glUniform1f(prg.ul_feed      ,  feed);
    glUniform1f(prg.ul_kill      ,  kill);
    glUniform1f(prg.ul_delta_time,  delta_time);
    glUniform1f(prg.ul_time, m_current_time);

    draw_quad();

    tex.swap();
    fbo.swap();
  }

  prg1.use();
  // render to screen
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glClear(GL_COLOR_BUFFER_BIT);

  glBindTexture(GL_TEXTURE_2D, tex.pong());
  glUniform1i(prg1.ul_quad_map, 0);

  draw_quad();

  if(add_pacman)
  {
    //glEnable(GL_BLEND);
    //glBlendColor(0, 0, 0, 0.5);
    //glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    m_pacman.draw(pacman_m_mat);
    //glDisable(GL_BLEND);
  }

  if(!display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "qQ : da :" << da << std::endl;
  ss << "wW : db :" << db << std::endl;
  ss << "eE : feed :" << feed << std::endl;
  ss << "rR : kill :" << kill << std::endl;
  ss << "uU : delta_time :" << delta_time << std::endl;
  ss << "iI : speed :" << speed << std::endl;
  ss << "g : : restart " << std::endl;
  ss << "y : : pacman " << add_pacman << std::endl;
  ss << "h : display help :" << display_help << std::endl;
  ss << m_fps;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20, vec4(0, 0, 1, 1));
  glDisable(GL_BLEND);
}

void reaction_diffusion_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
  m_text.reshape(m_info.wnd_width, m_info.wnd_height);
}

void reaction_diffusion_app::glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;

      case GLFW_KEY_G:
        replay();
        break;
      case GLFW_KEY_H:
        display_help ^= 1;
        break;

      case GLFW_KEY_Y:
        add_pacman ^= 1;
        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void reaction_diffusion_app::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  vec2 pos(x, glfw_to_gl(y));
  pos /= vec2(m_info.wnd_width, m_info.wnd_height);
  pos = (pos*2.0f - 1.0f) * vec2(100, 100);
  create_circle_seeds(pos, 20, false);
}

}

int main(int argc, char *argv[]) {
  zxd::reaction_diffusion_app app;
  app.run();
}
