#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include <algorithm>
#include "common_program.h"
#include "quad.h"
#include "debugger.h"
#include "string_util.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

glm::mat4 v_mat;
glm::mat4 p_mat;

lightless_program prg;
//point_program dot_prg;
geometry_base rocket_geometry;
GLint life = 200; // in frames
quad obstacle;

vec2 target = vec2(WIDTH*0.5, HEIGHT - 50);

float unit_random()
{
  return static_cast<float>( (rand() % RAND_MAX)) / RAND_MAX;
}

class dna
{

protected:
  GLfloat m_fitness;
  vec2_vector m_genes;

public:
  dna():
    m_fitness(0)
  {
    m_genes.reserve(life);
    for (int i = 0; i < life; ++i)
    {
      m_genes.push_back(glm::circularRand(1.0f));
    }
  }

  dna(const vec2_vector& thrusters):
    m_genes(thrusters)
  {
  }

  GLfloat fitness() const { return m_fitness; }
  void fitness(GLfloat v){ m_fitness = v; }

  const vec2_vector& thrusters() const { return m_genes; }
  void thrusters(const vec2_vector& v){ m_genes = v; }

  const vec2& thruster(GLint index)
  {
    if(index >= m_genes.size())
      throw std::runtime_error("thruster index overflow : " + string_util::to(index) 
          + " out of " + string_util::to(m_genes.size()));
    return m_genes[index];
  }

  dna cross_over(dna& rhs)
  {
    dna child(*this);
    int i = m_genes.size() / 2;
    std::copy(rhs.m_genes.begin() + i, rhs.m_genes.end(), m_genes.begin() + i);
    return child;
  }

  void mutate(float rate)
  {
      // should i mutate the tood gene too?
    for (int i = 0; i < m_genes.size(); ++i)
    {
      if(unit_random() < rate)
        m_genes[i] = glm::circularRand(1.0f);
    }
  }
};

typedef std::vector<dna> dna_vector;

class rocket
{
protected:
  bool m_crashed = false;
  bool m_hit = false;
  vec2 m_pos = vec2(WIDTH*0.5, 0);
  vec2 m_vel = vec2(0);
  vec2 m_acc = vec2(0);
  dna m_dna ;

public:
  rocket(){}
  rocket(const dna& dna_):
    m_dna(std::move(dna_))
  {}
  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }

  const vec2& vel() const { return m_vel; }
  void vel(const vec2& v){ m_vel = v; }

  const vec2& acl() const { return m_acc; }
  void acl(const vec2& v){ m_acc = v; }

  dna& get_dna() { return m_dna; }
  void set_dna(const dna& v){ m_dna = v; }

  bool crashed() const { return m_crashed; }
  void crashed(bool v){ m_crashed = v; }

  bool hit() const { return m_hit; }
  void hit(bool v){ m_hit = v; }

  void apply_force(const vec2& force)
  {
    m_acc += force;
  }

  void update(GLint life_count, const vec2& target)
  {
    if(m_crashed || m_hit)
      return;

    apply_force(m_dna.thruster(life_count));
    m_pos += m_vel;
    m_vel += m_acc;
    m_acc = vec2(0);
    ++life_count;

    if(m_pos.x < 0 || m_pos.x > WIDTH || m_pos.y < 0 || m_pos.y > HEIGHT)
    {
      crashed(true);
      return;
    }

    if(m_pos.x > obstacle.left() && m_pos.x < obstacle.right() 
        && m_pos.y > obstacle.bottom() && m_pos.y < obstacle.top())
    {
      crashed(true);
      return;
    }


    if(glm::distance2(m_pos, target) <= 100)
      hit(true);
  }

  void draw()
  {
    prg.use();
    mat4 mvp_mat =  
      glm::rotate(glm::translate(p_mat, vec3(m_pos, 0)), glm::atan(m_vel.y, m_vel.x), vec3(0, 0, 1));
    glUniform4f(prg.ul_color, 1,1,1,1);
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    rocket_geometry.draw();
  }

};

typedef std::vector<rocket> rocket_vector;


class population
{
protected:
  GLint m_size;
  GLint m_life_count = 0;
  GLfloat m_mutation_rate;
  rocket_vector m_rockets;
  rocket* m_best = 0;
public:
  population(GLint size, GLfloat mutation_rate):
    m_size(size),
    m_mutation_rate(mutation_rate)
  {
    for (int i = 0; i < m_size; ++i)
    {
      m_rockets.push_back(rocket());
      m_rockets.back().pos(vec2(WIDTH*0.5, 0));
    }
  }

  void update()
  {
    static int generation = 0;
    if(m_life_count >= life)
    {
      evaluate();
      std::cout << "generation " << ++generation 
                << " : fitness : " << m_best->get_dna().fitness()
                << " pos " << m_best->pos() << std::endl;

      reproduce();
      m_life_count = 0;
    }

    for (int i = 0; i < m_size; ++i)
    {
      m_rockets[i].update(m_life_count, target);
    }
    ++m_life_count;

  }

  void evaluate()
  {
    float max_fitness = 0;
    for (int i = 0; i < m_rockets.size(); ++i)
    {
      rocket& r = m_rockets[i];
      dna& d = r.get_dna();
      if(r.hit())
      {
        max_fitness = 0.1; // hit radius is 10
        max_fitness *= 10; // scale weight
        d.fitness(max_fitness);
        m_best = &r;
      }
      else
      {
        float l = glm::distance(r.pos(), target);
        GLfloat f = 1/l;
        if(r.crashed())
          f *= 0.25;

        d.fitness(f);
        if(max_fitness < d.fitness())
        {
          max_fitness = d.fitness();
          m_best = &r;
        }
      }
    }

    //for (int i = 0; i < m_rockets.size(); ++i)
    //{
      //rocket& r = m_rockets[i];
      //dna& d = r.get_dna();
      //d.fitness(d.fitness() / max_fitness);
    //}
    

  }

  dna* pick_by_fitness()
  {
    float best_fitness = m_best->get_dna().fitness();
    while(true)
    {
      dna* d = &m_rockets[rand() % m_size].get_dna();
      // use another random to apply fitness as selection weight
      if(best_fitness == 0 || unit_random() * best_fitness <  d->fitness())
        return d;
    }
  }

  void reproduce()
  {
    dna_vector dnas;
    dnas.reserve(m_size);

    for (int i = 0; i < m_size; ++i)
    {
      dna* parent0 = pick_by_fitness();
      dna* parent1 = pick_by_fitness();
      dnas.push_back(parent0->cross_over(*parent1));
      dnas.back().mutate(m_mutation_rate);
    }

    m_rockets.clear();
    for (int i = 0; i < m_size; ++i)
    {
      m_rockets.emplace_back(dnas[i]);
    }

  }

  void draw()
  {
    for (int i = 0; i < m_rockets.size(); ++i)
    {
      m_rockets[i].draw();
    }
  }

};


class smart_rocket_app : public app
{
protected:
  population m_population = population(100, 0.01);

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "smart_rocket_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    prg.init();
    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());

    //dot_prg.init();
    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());

    vec3_array* vertices = new vec3_array;
    vertices->push_back(vec3(-10, 5, 0));
    vertices->push_back(vec3(-10, -5, 0));
    vertices->push_back(vec3(10, 0, 0));
    rocket_geometry.attrib_array(0, array_ptr(vertices));
    rocket_geometry.bind_and_update_buffer();
    rocket_geometry.add_primitive_set(new draw_arrays(GL_TRIANGLES, 0, 3));

    obstacle.setup(vec3(150, 380, 0), vec3(500, 0, 0), vec3(0, 40, 0));
    obstacle.build_mesh();
  }

  virtual void update()
  {
    m_population.update();
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat));
    glUniform4f(prg.ul_color, 1,1,1,1);
    obstacle.draw();

    m_population.draw();

    //dot_prg.use();
    //mat4 mvp_mat = glm::translate(dot_prg.vp_mat, vec3(target, 0));
    //glUniformMatrix4fv(dot_prg.ul_mvp_mat, 1, 0, glm::value_ptr(dot_prg.vp_mat));
    //glUniform4f(prg.ul_color, 1,1,1,1);

    debugger::draw_point(vec3(target, 0), p_mat, 20, vec4(1,1,1,1));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
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
  zxd::smart_rocket_app app;
  app.run();
}
