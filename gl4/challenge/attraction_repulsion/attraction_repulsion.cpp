#include <sstream>
#include <list>

#include "app.h"
#include "bitmap_text.h"
#include "common_program.h"
#include "sphere.h"

#define WIDTH 800
#define HEIGHT 800
#define G 6.674 * 10000
#define NUM_ATTRACTOR 20
#define SPAWN_PER_FRAME 1

namespace zxd
{

glm::mat4 v_mat;
glm::mat4 p_mat;

blinn_program light_prg;
lightless_program ll_prg;

light_vector lights;
light_model lm;
material mtl;
sphere sphere_geometry;

GLuint vao;
GLuint vbo;
vec3_vector vertices;

class particle
{
protected:
  GLfloat m_mass = 1;
  GLfloat m_radius = 1;
  vec3 m_pos;
  vec3 m_accel;
  vec3 m_velocity; 
  vec3 m_last_pos;
  GLint m_life;

public:

  particle()
  {
    m_velocity = vec3(0);
    m_accel = vec3(0);
    m_life = 60 * 10;
  }

  // after cross border, last_pos must be reset
  void reset_last_pos()
  {
    m_last_pos = m_pos;
    //std::cout << m_last_pos << std::endl;
  }

  void update()
  {
    m_last_pos = m_pos;
    m_pos += m_velocity;
    m_velocity += m_accel;
    m_accel = vec3(0);
    --m_life;
  }

  bool dead()
  {
    return m_life <= 0;
  }

  void apply_force(const vec3& force)
  {
    m_accel += force / m_mass;
  }

  const vec3& pos() const { return m_pos; }
  void pos(const vec3& v){ m_pos = v; }

  const vec3& accel() const { return m_accel; }
  void accel(const vec3& v){ m_accel = v; }

  const vec3& velocity() const { return m_velocity; }
  void velocity(const vec3& v){ m_velocity = v; }

  const vec3& last_pos() const { return m_last_pos; }
  void last_pos(const vec3& v){ m_last_pos = v; }

  GLfloat mass() const { return m_mass; }
  void mass(GLfloat v){ m_mass = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }
};

class attractor
{
protected:
  GLfloat m_mass = 1;
  GLfloat m_radius = 1;
  vec3 m_pos;

public:

  mat4 m_mat()
  {
    return glm::translate(m_pos) * glm::scale(vec3(m_radius));
  }

  GLfloat mass() const { return m_mass; }
  void mass(GLfloat v){ m_mass = v; }

  vec3 pos() const { return m_pos; }
  void pos(const vec3& v){ m_pos = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  void attract(particle* p)
  {
    vec3 force = m_pos - p->pos();
    GLfloat distance = glm::length(force);
    force /= distance;
    GLfloat r = m_radius + p->radius();
    if(distance <= r)
    {
      force *= -1 * 0.5;
    }else
    {
      force *= G * m_mass * p->mass() /(distance * distance);
    }
    p->apply_force(force);
  }
};

typedef std::vector<particle> particle_vector;
typedef std::list<particle> particle_list;
particle_list particles;

typedef std::vector<attractor> attractor_vector;
attractor_vector attractors;

class attractoin_repulsion_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "attractoin_repulsion.cpp";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    light_source l0;
    l0.diffuse = vec4(1);
    l0.position = vec4(1, -1, 1,0);
    l0.specular = vec4(0.5);
    lights.push_back(l0);

    mtl.diffuse = vec4(0.8);
    mtl.specular = vec4(0.5);
    mtl.shininess = 50;

    lm.local_viewer = true;

    light_prg.init();
    v_mat = glm::lookAt(vec3(0, -1500, 1500), vec3(0), pza);
    p_mat = glm::perspective(fpi4, wnd_aspect(), 1.0f, 10000.0f);
    light_prg.bind_lighting_uniform_locations(lights, lm, mtl);

    ll_prg.init();

    set_v_mat(&v_mat);

    sphere_geometry.include_normal(true);
    sphere_geometry.build_mesh();

    //prg.init();
    for (int i = 0; i < NUM_ATTRACTOR; ++i) 
    {
      attractor atr;
      atr.pos(glm::linearRand(vec3(-200.0f), vec3(200.0f)));
      //std::cout << atr.pos() << std::endl;
      atr.radius(10);
      attractors.push_back(atr);
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  virtual void update()
  {
    for (int i = 0; i < SPAWN_PER_FRAME; ++i)
    {
      particle p;
      p.pos(glm::linearRand(vec3(-500.0f), vec3(500.0f)));
      p.velocity(glm::sphericalRand(2.0f));
      particles.push_back(p);
    }
    
    vertices.clear();
    vertices.reserve(particles.size() * 2);
    for(auto iter = particles.begin(); iter != particles.end();)
    {
      if(iter->dead())
      {
        iter = particles.erase(iter);
        continue;
      }

      iter->update();

      for (int i = 0; i < attractors.size(); ++i)
        attractors[i].attract(&*iter);
      vertices.push_back(iter->last_pos());
      vertices.push_back(iter->pos());
      ++iter;
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(vec3), 
        glm::value_ptr(vertices.front()), GL_DYNAMIC_DRAW);
  }

  virtual void display()
  {
    glEnable(GL_DEPTH_TEST);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //prg.use();
    light_prg.use();
    light_prg.update_lighting_uniforms(lights, lm, mtl, v_mat);
    for (int i = 0; i < attractors.size(); ++i)
    {
      light_prg.update_uniforms(attractors[i].m_mat(), v_mat, p_mat);
      sphere_geometry.draw();
    }

    glPointSize(2);
    ll_prg.use();
    mat4 mvp_mat = p_mat * v_mat;
    glUniform4f(ll_prg.ul_color, 1,1,1,1);
    glUniformMatrix4fv(ll_prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));

    glBindVertexArray(vao);
    glEnable(GL_BLEND);
    glBlendColor(0,0,0,0.2);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    glDrawArrays(GL_LINES, 0, vertices.size());
    glDisable(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps " << m_fps;
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
        case GLFW_KEY_Q:
          glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
  zxd::attractoin_repulsion_app app;
  app.run();
}
