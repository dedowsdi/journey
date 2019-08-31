#include "app.h"
#include "bitmap_text.h"
#include "spritesheet.h"
#include "common_program.h"
#include "texutil.h"
#include <sstream>
#include "quad.h"
#include "pingpong.h"

#define WIDTH 640
#define HEIGHT 360

namespace zxd
{

lightless_program lprg;
spritesheet sheet;
npingpong ripple_tex;

vec2_vector sprite_anim;
std::vector<quad> quads;
GLfloat horse_size = 100;
GLuint num_horses = 20;
GLuint fbo;
bool display_help = false;

GLfloat ripple_dampen = 0.85;
GLfloat ripple_start = 1.8;
GLuint  ripple_frame = 4;

kcip kci_ripple_dampen;
kcip kci_ripple_start;
kcip kci_ripple_frame;
quad q;

glm::mat4 v_mat;
glm::mat4 p_mat;

class ripple_program : public program
{
public:
  GLint ul_buffer0;
  GLint ul_buffer1;
  GLint ul_dampen;

  void attach_shaders() override
  {
    attach(GL_VERTEX_SHADER, "shader4/ripple.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/ripple.fs.glsl");
  }

  void bind_uniform_locations() override
  {
    ul_buffer0 = get_uniform_location("buffer0");
    ul_buffer1 = get_uniform_location("buffer1");
    ul_dampen = get_uniform_location("dampen");
  }

  void bind_attrib_locations() override
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

}rprg;

class horse
{
protected:
  GLfloat m_speed;
  GLfloat m_anim_index = 0;
  vec2 m_pos;

public:

  GLfloat speed() const { return m_speed; }
  void speed(GLfloat v){ m_speed = v; }

  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }

  void update()
  {
    m_anim_index += m_speed;
    m_pos.x += m_speed * 10;
    if(m_pos.x > WIDTH)
    {
      m_pos.x = -horse_size;
      m_pos.y = glm::linearRand(0.0f, HEIGHT - horse_size);
      random_speed();
    }
  }

  void random_speed()
  {
    m_speed = glm::linearRand(0.3, 0.7);
  }

  void draw()
  {
    mat4 mvp_mat = glm::translate(p_mat, vec3(m_pos, 0));
    glUniformMatrix4fv(lprg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, sheet.tex());
    GLuint index = static_cast<GLuint>(m_anim_index) % quads.size();
    quads[index].draw();
  }
};

std::vector<horse> horses;

GLuint create_ripple_texture()
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  
  // need float texture, as ripple start might be greater than 1
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
  return tex;
}

class spritesheet_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "spritesheet_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.decorated = false;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glfwSetWindowPos(m_wnd, 100, 100);


    lprg.with_texcoord = true;
    lprg.init();
    p_mat = glm::ortho<GLfloat>(0, WIDTH, 0, HEIGHT, -1, 1);

    rprg.init();

    fipImage img = fipLoadResource("texture/horse.png");
    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0,
        GL_BGRA, GL_UNSIGNED_BYTE, img.accessPixels());

    q.include_texcoord(true);
    q.build_mesh();

    sheet.tex(tex);
    sheet.rows(4);
    sheet.cols(3);

    sprite_anim.reserve(7);
    sprite_anim.push_back(sheet.get_sprite(3, 0));
    sprite_anim.push_back(sheet.get_sprite(3, 1));
    sprite_anim.push_back(sheet.get_sprite(3, 2));
    sprite_anim.push_back(sheet.get_sprite(2, 0));
    sprite_anim.push_back(sheet.get_sprite(2, 1));
    sprite_anim.push_back(sheet.get_sprite(2, 2));
    sprite_anim.push_back(sheet.get_sprite(1, 0));

    quads.reserve(sprite_anim.size());
    for (int i = 0; i < sprite_anim.size(); ++i)
    {
      quad q;
      q.setup(0, 0, horse_size, horse_size);
      q.tc0(sprite_anim[i]);
      q.tc1(sprite_anim[i] + sheet.sprite_size());
      q.include_texcoord(true);
      q.build_mesh();
      quads.push_back(q);
    }

    horses.reserve(num_horses);
    for (int i = 0; i < num_horses; ++i)
    {
      horse h;
      h.pos(glm::linearRand(vec2(0), vec2(WIDTH, HEIGHT) - vec2(horse_size)));
      h.random_speed();
      horses.push_back(h);
    }

    ripple_tex.add_resource(create_ripple_texture());
    ripple_tex.add_resource(create_ripple_texture());
    ripple_tex.add_resource(create_ripple_texture());

    glGenFramebuffers(1, &fbo);

    kci_ripple_dampen = m_control.add<GLfloat>(GLFW_KEY_Q, ripple_dampen, 0, 10, 0.01);
    kci_ripple_start = m_control.add<GLfloat>(GLFW_KEY_W, ripple_start, 0, 500, 1);
    kci_ripple_frame = m_control.add<GLint>(GLFW_KEY_E, ripple_frame, 1, 60, 1);
  }

  virtual void update() 
  {
    ripple_start = kci_ripple_start->get_float();
    ripple_dampen = kci_ripple_dampen->get_float();
    ripple_frame = kci_ripple_frame->get_int();

    if((m_frame_number % ripple_frame) == 0)
      ripple_tex.shift();

    glBindTexture(GL_TEXTURE_2D, ripple_tex.first_ping());
    for(auto& item : horses)
    {
      item.update();
      vec2 pixel_pos = item.pos() + vec2(horse_size, horse_size * 0.1);
      if(pixel_pos.x > 0 && pixel_pos.x < WIDTH)
      {
        vec3 size = vec3(ripple_start * pow(2, item.speed()) * 2);
        glTexSubImage2D(GL_TEXTURE_2D, 0, pixel_pos.x, pixel_pos.y, 1, 1, 
            GL_RGB, GL_FLOAT, &size);
      }
    }
  }

  virtual void display()
  {

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ripple_tex.pong(), 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ripple_tex.first_ping());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, ripple_tex.last_ping());
    rprg.use();
    glUniform1i(rprg.ul_buffer0, 0);
    glUniform1i(rprg.ul_buffer1, 1);
    glUniform1f(rprg.ul_dampen, ripple_dampen);
    q.draw();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    draw_quad(ripple_tex.pong(), 0);

    lprg.use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    for(auto& item : horses)
    {
      item.draw();
    }
    glDisable(GL_BLEND);

    if(!display_help)
      return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << " fps : " << m_fps << std::endl;
    ss << " qQ : dampen : " << ripple_dampen << std::endl;
    ss << " wW : ripple start : " << ripple_start << std::endl;
    ss << " eE : ripple frame : " << ripple_frame << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
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
  zxd::spritesheet_app app;
  app.run();
}
