#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include <list>
#include "texutil.h"
#include <algorithm>
#include <iterator>

using namespace glm;

namespace zxd
{

  glm::mat4 v_mat;
  glm::mat4 p_mat;

struct particle_program : public zxd::program
  {
  GLint ul_diffuse_map;
  GLint ul_camera_pos;
  GLint ul_camera_up;
  GLint ul_vp_mat;
  vec3 camera_pos;
  vec3 camera_up;

  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/particle.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/particle.fs.glsl");
  }
  virtual void bind_uniform_locations()
  {
    uniform_location(&ul_vp_mat, "vp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_camera_pos, "camera_pos");
    uniform_location(&ul_camera_up, "camera_up");
  }

  virtual void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "xyzs");
    bind_attrib_location(2, "color");
  };
};
GLuint max_particles = 10000;
GLuint emits_per_second = 2000;

GLfloat min_speed = 0.5;
GLfloat max_speed = 2.0;
GLfloat min_life = 0.5;
GLfloat max_life = 3;
GLfloat min_acceleration = 0.1;
GLfloat max_acceleration = 2.0;
GLfloat min_size = 0.5;
GLfloat max_size = 3;

GLuint vao;
GLuint xyzs_buffer;
GLuint color_buffer;

struct particle
{
  vec3 pos;
  vec3 velocity;
  vec3 accleleration;
  vec4 color;
  GLfloat size;
  GLfloat life;
  float camera_distance;  // squared distance, used to sort particles
};

// clang-format off
glm::vec4 vertices[] =
{ 
  vec4(-0.5, 0.5, 0, 1), // x y s t
  vec4(-0.5, -0.5, 0, 0),
  vec4(0.5, 0.5, 1, 1),
  vec4(0.5, -0.5, 1, 0)
};
// clang-format on

typedef std::vector<particle> particle_vector;
typedef std::list<particle *> particle_list;

vec4_vector positions;  // x,y,z,size
vec4_vector colors;
particle_vector particles;
particle_list alive_particles;
particle_list dead_particles;


GLuint tex;

particle_program prg;

class particle_app : public app
{
protected:

public:
  particle_app() {}

protected:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "hello world";
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    // load particle texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto image = zxd::fipLoadResource("texture/flare.png");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(),
      0, GL_BGR, GL_UNSIGNED_BYTE, image.accessPixels());
    // init particles
    particles.reserve(max_particles);
    for (int i = 0; i < max_particles; ++i)
    {
      particles.push_back(particle());
    }
    for (int i = 0; i < max_particles; ++i)
    {
      dead_particles.push_back(&particles[i]);
    }

    // always use max particle sized buffer?
    positions.resize(max_particles);
    colors.resize(max_particles);

    // init program
    prg.init();
    v_mat = glm::lookAt(vec3(0, -15, 15), vec3(0), vec3(0, 0, 1));
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
    set_v_mat(&v_mat);

    // create buffer
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), value_ptr(vertices[0]),
      GL_STATIC_DRAW);

    glVertexAttribPointer( 0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &xyzs_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, xyzs_buffer);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(vec4) * max_particles, 0, GL_STREAM_DRAW);

    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    glGenBuffers(1, &color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(vec4) * max_particles, 0, GL_STREAM_DRAW);

    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    update();
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 vp_mat = p_mat * v_mat;

    glUseProgram(prg);

    glBindTexture(GL_TEXTURE_2D, tex);
    glUniformMatrix4fv(prg.ul_vp_mat, 1, 0,
    value_ptr(vp_mat));
    glUniform1i(prg.ul_diffuse_map, 0);
    glUniform3fv(prg.ul_camera_pos, 1, value_ptr(prg.camera_pos));
    glUniform3fv(prg.ul_camera_up, 1, value_ptr(prg.camera_up));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, alive_particles.size());

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : emits per second : " << emits_per_second << std::endl;
    ss << "current particles : " << alive_particles.size() << std::endl;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 25);
    glDisable(GL_BLEND);
  }

  virtual void update()
  {
    static GLfloat t0 = glfwGetTime();
    static GLfloat t = 0;  // emitter timer

    GLfloat t1 = glfwGetTime();
    GLfloat dt = t1 - t0;
    GLfloat emit_period = 1.0f / emits_per_second;
    t += dt;

    prg.camera_pos = glm::inverse(v_mat)[3].xyz();
    prg.camera_up = glm::row(v_mat, 1).xyz();

    GLint num_new_particles = t / emit_period;
    t -= num_new_particles * emit_period;

    num_new_particles = glm::min(static_cast<GLuint>(num_new_particles),
      static_cast<GLuint>(emits_per_second * 0.16));

    // generate new particles
    for (int i = 0; i < num_new_particles; ++i)
    {
      if (dead_particles.empty()) break;

      particle &p = *dead_particles.front();
      p.life = glm::linearRand(min_life, max_life);
      p.color = glm::linearRand(vec4(0.0), vec4(1.0));
      p.pos = vec3(0);
      vec3 dir(0);
      while (dir == vec3(0)) dir = glm::linearRand(vec3(-1.0), vec3(1.0));
      dir = glm::normalize(dir);
      p.velocity = dir * glm::linearRand(min_speed, max_speed);
      p.accleleration = dir * glm::linearRand(min_acceleration,
      max_acceleration);
      p.size = glm::linearRand(min_size, max_size);

      alive_particles.push_back(&p);
      dead_particles.pop_front();
    }

    // animating particles
    GLuint index = 0;
    for (auto iter = alive_particles.begin(); iter != alive_particles.end();)
    {
      particle &p = **iter;
      p.life -= dt;
      if (p.life < 0)
      {
        auto it = iter;
        ++it;
        alive_particles.erase(iter);
        dead_particles.push_back(&p);
        iter = it;
        continue;
      }

      p.pos += p.velocity * dt + p.accleleration * 0.5f * dt * dt;
      p.velocity += p.accleleration * dt;
      p.camera_distance = length2(p.pos - prg.camera_pos);
      ++iter;
    }

    // particles are translucent, they need to be sorted by camera distance
    alive_particles.sort([&](particle *p0, particle *p1) -> bool
        {
      return p0->camera_distance < p1->camera_distance;
    });

    index = 0;
    for (auto iter = alive_particles.begin(); iter != alive_particles.end();
         ++iter)
    {
      particle &p = **iter;
      positions[index] = vec4(p.pos, p.size);
      colors[index] = p.color;
      ++index;
    }

    // update buffer
    glBindBuffer(GL_ARRAY_BUFFER, xyzs_buffer);
    //glBufferData(GL_ARRAY_BUFFER, alive_particles.size() * sizeof(vec4), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, alive_particles.size() * sizeof(vec4),
      value_ptr(positions.front()));

    glBindBuffer(GL_ARRAY_BUFFER, color_buffer);
    //glBufferData(GL_ARRAY_BUFFER, alive_particles.size() * sizeof(vec4), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, alive_particles.size() * sizeof(vec4),
      value_ptr(colors.front()));

    t0 = t1;
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
          if (mods & GLFW_KEY_Q)
          {
            if (emits_per_second > 5)
            {
              emits_per_second -= 5;
            }
          } else
          {
            emits_per_second += 5;
          }

          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }

  virtual void glfw_mouse_button(
    GLFWwindow *wnd, int button, int action, int mods)
  {
    app::glfw_mouse_button(wnd, button, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y)
  {
    app::glfw_mouse_move(wnd, x, y);
  }

  virtual void glfw_mouse_wheel(GLFWwindow *wnd, double xoffset, double yoffset)
  {
    app::glfw_mouse_wheel(wnd, xoffset, yoffset);
  }
  virtual void glfw_char(GLFWwindow *wnd, unsigned int codepoint)
  {
    app::glfw_char(wnd, codepoint);
  }
  virtual void glfw_charmod(GLFWwindow *wnd, unsigned int codepoint, int mods)
  {
    app::glfw_charmod(wnd, codepoint, mods);
  }
};
}

int main(int argc, char *argv[])
{
  zxd::particle_app app;
  app.run();
}
