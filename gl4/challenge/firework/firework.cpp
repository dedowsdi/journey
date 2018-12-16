/*
 * i'm useing point sprite as particle in this challenge. which means particle
 * will not sclae with camera distance.
 */
#include "app.h"
#include "bitmaptext.h"
#include <list>
#include <sstream>
#include "debugger.h"

#define WIDTH 800
#define HEIGHT 800
#define GRAVITY vec3(0, 0, -0.1)

namespace zxd {

struct geometry
{
  GLuint vao;
  GLuint vbo;

  GLuint capacity; 
  GLuint vertex_count;

  void init()
  {
    capacity = 5000;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, (sizeof(vec3) + sizeof(vec4) + sizeof(float)) * capacity, 0, GL_STREAM_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(12));
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, 32, BUFFER_OFFSET(28));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
  }

  void update_buffer(const float_vector& vertices)
  {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    vertex_count = vertices.size() / 8;
    if(vertex_count > capacity)
    {
      capacity = vertex_count;
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices.front(), GL_STREAM_DRAW);
    }
    else
    {
      glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), &vertices.front());
    }
  }

  void draw()
  {
    glBindVertexArray(vao);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_POINTS, 0, vertex_count);
  }
};

struct quad
{
  GLuint vao;
  void init()
  {
    glGenVertexArrays(1, &vao);
  }

  void draw()
  {
    glBindVertexArray(vao);
    glEnable(GL_BLEND);
    glBlendColor(0, 0, 0, 0.9);
    glBlendFunc(GL_ZERO, GL_CONSTANT_ALPHA);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDisable(GL_BLEND);
  }
}q;

class particle
{
protected:
  GLfloat m_size;
  vec3 m_pos;
  vec3 m_vel;
  vec3 m_acc;
  vec4 m_color;

public:

  void update()
  {
    m_pos += m_vel;
    m_vel += m_acc;
    //std::cout << m_pos << std::endl;
  }

  vec3 pos() const { return m_pos; }
  void pos(vec3 v){ m_pos = v; }

  vec3 vel() const { return m_vel; }
  void vel(vec3 v){ m_vel = v; }

  const vec3& acc() const { return m_acc; }
  void acc(const vec3& v){ m_acc = v; }

  const vec4& color() const { return m_color; }
  void color(const vec4& v){ m_color = v; }

  void decre_alpha(GLfloat a){m_color.a -= a; }

  GLfloat size() const { return m_size; }
  void size(GLfloat v){ m_size = v; }

  void compile(float_vector& vertices)
  {
    vertices.push_back(m_pos.x);
    vertices.push_back(m_pos.y);
    vertices.push_back(m_pos.z);
    vertices.push_back(m_color.x);
    vertices.push_back(m_color.y);
    vertices.push_back(m_color.z);
    vertices.push_back(m_color.w);
    vertices.push_back(m_size);
  }

};
typedef std::vector<particle> particle_vector;


class firework
{
protected:
  bool m_exploded = false;
  particle_vector m_particles; // exploded
  particle m_particle; // init one

public:
  firework(const vec3& pos, const vec3& color)
  {
    m_particle.size(6);
    m_particle.pos(pos);
    m_particle.color(vec4(color, 1));
    m_particle.vel(vec3(0, 0, glm::linearRand(6, 10)));
    m_particle.acc(GRAVITY);
  }

  void update()
  {
    if(!m_exploded)
    {
      m_particle.update();

      if(m_particle.vel().z < 0)
        explode();

      return;
    }

    for(auto& item : m_particles)
    {
      item.vel(item.vel()*0.95f);
      item.update();
      item.decre_alpha(0.01);
    }
  }

  void debug_draw(const mat4& m)
  {
    if(!m_exploded)
      debugger::draw_point(m_particle.pos(), m , 4);
    else
    {
      for(auto& item : m_particles)
      {
        debugger::draw_point(item.pos(), m , 4);
      }
    }
  }

  bool done()
  {
    return m_exploded && m_particles.front().color().a <= 0;
  }

  void explode()
  {
    m_exploded = true;
    GLuint count = glm::linearRand(50, 120);

    while(count--)
    {
      particle p;
      p.pos(m_particle.pos());
      p.vel(glm::ballRand(8.0));
      p.acc(GRAVITY);
      p.color(m_particle.color());
      p.size(2);
      m_particles.push_back(p);
    }

  }

  void compile(float_vector& vertices)
  {
    if(!m_exploded)
      m_particle.compile(vertices);
    else
    {
      for(auto& item : m_particles)
      {
        item.compile(vertices);
      }
      
    }
  }
};

typedef std::list<firework> firework_list;

class firework_program : public program
{

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader/firework.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/firework.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "color");
    bind_attrib_location(2, "size");
  }

} prg;

class trail_program : public program
{
protected:
  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader/filter.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/filter_blank.fs.glsl");
  }
} filter_prg;

class firework_app : public app {
protected:
  bitmap_text m_text;
  firework_list m_fireworks;
  geometry m_geometry;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "firework_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    prg.init();
    prg.p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 3000.0f);
    prg.v_mat = glm::lookAt(vec3(0, -800, 0), vec3(0), pza);
    set_v_mat(&prg.v_mat);

    filter_prg.init();

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    m_geometry.init();
    q.init();
  }

  virtual void update() 
  {
    GLfloat r = glm::linearRand(0.0, 1.0);
    if(r > 0.9)
    {
      // create new firework
      vec3 pos = glm::linearRand(vec3(-500), vec3(500, 500, -50));
      m_fireworks.push_back(firework(pos, glm::linearRand(vec3(0.0f), vec3(1.0f))));
    }

    float_vector vertices;
    vertices.reserve(m_fireworks.size() * 100 * 8);
    for(auto iter = m_fireworks.begin(); iter != m_fireworks.end();){
      if(iter->done())
      {
        iter = m_fireworks.erase(iter);
        continue;
      }
      iter->update();
      iter->compile(vertices);
      ++iter;
    }

    // collect data
    m_geometry.update_buffer(vertices);
  }

  virtual void display() {
    // in order to fake the trail effect, don't clear frame, instead use a
    // filter to fade last frame
    //glClear(GL_COLOR_BUFFER_BIT);
    filter_prg.use();
    q.draw();

    prg.use();
    mat4 mvp_mat = prg.p_mat * prg.v_mat;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));

    //for(auto& item : m_fireworks)
    //{ 
      //item.debug_draw(prg.p_mat * prg.v_mat);
    //}
    m_geometry.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps " << m_fps << std::endl;
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
  zxd::firework_app app;
  app.run();
}
