#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include "debugger.h"
#include <sstream>

#define WIDTH 800.0
#define HEIGHT 800.0
#define FF_ROW 40
#define FF_COL 40
#define NUM_PARTICLE 20000
#define MAX_SPEED 1.5f
#define FORCE_MAGNITUDE 0.35f
#define ALPHA 0.05

namespace zxd {

GLboolean draw_flow_field = GL_TRUE;
lightless_program prg;
GLfloat field_width  = WIDTH/FF_COL;
GLfloat field_height = HEIGHT/FF_ROW;
GLfloat zoff = 0;
GLfloat xoff_start;
GLfloat yoff_start;
GLfloat zoff_start;

kcip xoff_step;
kcip yoff_step;
kcip zoff_step;

struct field_cell
{
  GLfloat time = 0; // last updated time
  vec2 force;
};

struct ink_geometry
{
  GLuint vao;
  GLuint vbo;
  vec2_vector vertices;
} ink;

std::vector<field_cell> flow_fields;

const vec2& get_field_force(GLshort row, GLshort col, GLfloat time)
{
  GLuint index = row * FF_COL + col;
  field_cell& cell = flow_fields[index];

  // it's expensive to do perlin noise in 3d. this time based lazy update will
  // only update force on demand field.
  if(cell.time != time)
  {
    cell.time = time;
    //std::cout << row << ":" << col << std::endl;
    GLfloat angle = glm::perlin(
        glm::vec3(xoff_step->get_float() * col + xoff_start, yoff_step->get_float() * row + yoff_start, zoff)) * f2pi;
    cell.force = vec2(glm::cos(angle), glm::sin(angle)) * FORCE_MAGNITUDE;
  }
  return cell.force;
}

class particle
{
protected:
  vec2 m_pos;
  vec2 m_accel;
  vec2 m_velocity; 
  vec2 m_last_pos;

public:

  particle()
  {
    m_pos = glm::linearRand(vec2(0.0f), vec2(WIDTH, HEIGHT));
    m_last_pos = m_pos;
    m_accel = glm::circularRand(1);
    m_velocity = vec2(0);
  }

  // after cross border, last_pos must be reset
  void reset_last_pos()
  {
    m_last_pos = m_pos;
    //std::cout << m_last_pos << std::endl;
  }

  void update(const vec2& force)
  {
    m_last_pos = m_pos;
    m_pos += m_velocity;
    m_velocity += m_accel;
    m_accel = force;
    GLfloat l2 = glm::length2(m_velocity);
    if(l2 > MAX_SPEED * MAX_SPEED)
    {
      m_velocity = MAX_SPEED * m_velocity / glm::sqrt(l2);
    }

    // wrap border
    if(m_pos.x > WIDTH)
    {
      m_pos.x -= WIDTH;
      reset_last_pos();
    }
    else if(m_pos.x < 0)
    {
      m_pos.x += WIDTH;
      reset_last_pos();
    }
    if(m_pos.y > HEIGHT)
    {
      m_pos.y -= HEIGHT;
      reset_last_pos();
    }
    else if(m_pos.y < 0)
    {
      m_pos.y += HEIGHT;
      reset_last_pos();
    }

  }

  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }

  const vec2& accel() const { return m_accel; }
  void accel(const vec2& v){ m_accel = v; }

  const vec2& velocity() const { return m_velocity; }
  void velocity(const vec2& v){ m_velocity = v; }


  vec2 last_pos() const { return m_last_pos; }
  void last_pos(vec2 v){ m_last_pos = v; }
};

typedef std::vector<particle> particle_vector;
particle_vector particles;

class flow_field_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "flow_field_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    //flow_fields.reserve(FF_ROW * FF_COL);
    flow_fields.resize(FF_ROW * FF_COL);

    xoff_step = m_control.add_control<GLfloat>(GLFW_KEY_W, 0.05, -10000, 10000, 0.01);
    yoff_step = m_control.add_control<GLfloat>(GLFW_KEY_E, 0.05, -10000, 10000, 0.01);
    zoff_step = m_control.add_control<GLfloat>(GLFW_KEY_R, 0.002, -10000, 10000, 0.002);

    particles.resize(NUM_PARTICLE);

    glGenVertexArrays(1, &ink.vao);
    glGenBuffers(1, &ink.vbo);
    
    glBindVertexArray(ink.vao);
    glBindBuffer(GL_ARRAY_BUFFER, ink.vbo);
    glBufferData(GL_ARRAY_BUFFER, particles.size() * sizeof(vec2)*2, 0, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
    
    ink.vertices.resize(particles.size() * 2);
    xoff_start = glm::linearRand(0.0, 100.0);
    yoff_start = glm::linearRand(0.0, 100.0);
    zoff_start = glm::linearRand(0.0, 100.0);
    zoff += zoff_start;
  }

  virtual void update() {
    zoff += zoff_step->get_float();

    for (int i = 0; i < particles.size(); ++i) 
    {
      particle& p = particles[i];
      GLuint x = p.pos().x / field_width;
      GLuint y = p.pos().y / field_height;
      p.update(get_field_force(y, x, m_current_time));
    }

    if(!draw_flow_field)
    {
      for (int i = 0; i < particles.size(); ++i) {
        ink.vertices[i*2] = particles[i].last_pos();
        ink.vertices[i*2+1] = particles[i].pos();
      }
      glBindBuffer(GL_ARRAY_BUFFER, ink.vbo);
      glBufferSubData(GL_ARRAY_BUFFER, 0, ink.vertices.size() * sizeof(vec2), glm::value_ptr(ink.vertices.front()));
    }
  }

  virtual void display() {

    //debugger::draw_point(vec3(400, 400, 0), prg.vp_mat, 10);
    //debugger::draw_line(vec3(0), vec3(500, 500, 0), prg.vp_mat, 4);

    // debug draw flow field
    if(draw_flow_field)
    {
      glClear(GL_COLOR_BUFFER_BIT);

      vec3_vector lines;
      for (GLshort i = 0; i < FF_ROW; ++i) {
        vec3 translate = vec3(0, i*field_height, 0);

        for (GLshort j = 0; j < FF_COL; ++j) {
          translate.x += field_width;
          const vec2& v = get_field_force(i, j, m_current_time) / FORCE_MAGNITUDE;
          lines.push_back(translate);
          lines.push_back(translate + vec3(v, 0) * field_width);
        }
      }
      debugger::draw_line(GL_LINES, lines, prg.vp_mat);

      vec3_vector points;
      for (int i = 0; i < particles.size(); ++i) {
        points.push_back(vec3(particles[i].pos(), 0));
        //std::cout << points.back() << std::endl;
      }
      debugger::draw_point(points, prg.vp_mat, 4);
    }
    else
    {
      glEnable(GL_BLEND);
      glBlendColor(0, 0, 0, ALPHA);
      glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
      // no glClear
      prg.use();
      glBindVertexArray(ink.vao);
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat));
      glUniform4f(prg.ul_color, 0, 0, 0, 1);
      glLineWidth(1.5);
      glDrawArrays(GL_LINES, 0, ink.vertices.size());

      glDisable(GL_BLEND);
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : draw flow field" << std::endl;
    ss << "w : xoff step : " << xoff_step->get_float() <<  std::endl;
    ss << "e : yoff step : " << yoff_step->get_float() <<  std::endl;
    ss << "r : zoff step : " << zoff_step->get_float() <<  std::endl;
    ss << "fps : " << m_fps << std::endl;
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

        case GLFW_KEY_Q:
          glClear(GL_COLOR_BUFFER_BIT);
          draw_flow_field ^= 1;
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
  zxd::flow_field_app app;
  app.run();
}
