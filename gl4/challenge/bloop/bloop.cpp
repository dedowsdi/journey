/*
 * bloop eat food to extend it's life. The longer it's life, the bigger it's
 * alpha, the more change it gets to reproduce.
 *
 * only bloop with proper size and speed can survive.
 */
#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"
#include "quad.h"
#include "circle.h"
#include <list>


#define WIDTH 800
#define HEIGHT 800
#define FOOD_SIZE 20
#define FOOD_PRODUCE_CHANCE 0.02
#define MAX_BLOOP_SIZE 50
#define FULL_ALPHA_LIFE 1200
#define MAX_BLOOP_SPEED 5
#define BLOOP_REPRODUCE_CHANCE 0.0003

namespace zxd {

quad food_geometry;
lightless_program prg;
circle bloop_geometry;

std::list<vec2> foods;

struct dna
{
  // normalized, propertion to init size, inverse propertion to speed
  GLfloat gene;
};

class bloop
{
protected:
  
  GLint m_life;
  GLfloat m_size;
  GLfloat m_speed;
  vec2 m_pos;
  dna m_dna;
  vec2 noise_off;

public:
  bloop()
  {
    m_life = FULL_ALPHA_LIFE;
    m_dna.gene = glm::linearRand(0.0f, 1.0f);
    m_size = MAX_BLOOP_SIZE * m_dna.gene;
    m_speed = MAX_BLOOP_SPEED * (1 - m_dna.gene);

    noise_off = glm::linearRand(vec2(-10000.0), vec2(10000.0));
  }

  GLint life() const { return m_life; }
  void life(GLint v){ m_life = v; }
  void add_life(GLint l){m_life += l;}

  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }

  GLfloat size() const { return m_size; }

  void update()
  {
    --m_life;
    GLfloat angle = glm::perlin(noise_off) * f2pi;
    vec2 vel = vec2(cos(angle), sin(angle)) * m_speed;
    m_pos += vel;
    noise_off += vec2(0.001, 0.001);

    if(m_pos.x < 0)
      m_pos.x += WIDTH;
    else if(m_pos.x > WIDTH)
      m_pos.x -= WIDTH;

    if(m_pos.y < 0)
      m_pos.y += HEIGHT;
    else if(m_pos.y > HEIGHT)
      m_pos.y -= HEIGHT;
  }

  void draw()
  {
    GLfloat alpha =  glm::min(1.0f, static_cast<GLfloat>(m_life) / FULL_ALPHA_LIFE);
    glBlendColor(0,0,0,alpha);

    mat4 mvp_mat = glm::scale(glm::translate(prg.vp_mat, vec3(m_pos, 0)), vec3(m_size, m_size, 1));
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4f(prg.ul_color, 0,0,0,0);
    bloop_geometry.draw();
  }

  bool dead()
  {
    return m_life <= 0;
  }

  bloop reproduce()
  {
    bloop child(*this);
    child.noise_off = glm::linearRand(vec2(-10000.0), vec2(10000.0));
    child.life(FULL_ALPHA_LIFE);
    return child;
  }
};

std::list<bloop> bloops;

class bloop_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "bloop_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    food_geometry.setup(vec3(-FOOD_SIZE*0.5,-FOOD_SIZE*0.5,0), vec3(FOOD_SIZE,0,0), vec3(0, FOOD_SIZE, 0));
    food_geometry.build_mesh();

    for (int i = 0; i < 10; ++i) 
      foods.push_back(glm::linearRand(vec2(0), vec2(WIDTH, HEIGHT)));

    for (int i = 0; i < 50; ++i)
    {
      bloops.push_back(bloop());
      bloops.back().pos(glm::linearRand(vec2(0), vec2(WIDTH, HEIGHT)));
    }

    bloop_geometry.slice(64);
    bloop_geometry.build_mesh();
  }

  virtual void update() 
  {
    if(glm::linearRand(0.0f, 1.0f) < FOOD_PRODUCE_CHANCE)
      foods.push_back(glm::linearRand(vec2(0), vec2(WIDTH, HEIGHT)));

    for(auto iter = bloops.begin(); iter != bloops.end(); )
    {
      iter->update();
      if(iter->dead())
      {
        iter = bloops.erase(iter);
        continue;
      }

      if(glm::linearRand(0.0f, 1.0f) < BLOOP_REPRODUCE_CHANCE)
        bloops.push_back(iter->reproduce());

      // eat foot
      for(auto iter2 = foods.begin(); iter2 != foods.end(); ++iter2)
      {
        if(glm::distance2(*iter2, iter->pos()) <= iter->size() * iter->size())
        {
          iter->add_life(FULL_ALPHA_LIFE);
          foods.erase(iter2);
          break;
        }
      }

      ++iter;
    }

  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();

    for(auto iter = foods.begin(); iter != foods.end(); ++iter)
    {
      mat4 mvp_mat =  glm::translate(prg.vp_mat, vec3(*iter, 0));
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
      glUniform4f(prg.ul_color, 0, 1, 0, 1);
      food_geometry.draw();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    for(auto iter = bloops.begin(); iter != bloops.end(); ++iter)
      iter->draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
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
  zxd::bloop_app app;
  app.run();
}
