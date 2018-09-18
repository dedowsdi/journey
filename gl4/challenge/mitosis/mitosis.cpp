#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include "sphere.h"
#include <algorithm>

#define WIDTH 800
#define HEIGHT 800

// constrain cells in a sphere
#define RADIUS 25.0f
#define MIN_SPEED 0.1f
#define MAX_SPEED 3.0f
#define NUM_CELLS 30
#define MITOSIS_TIME 1.5f
#define MITOSIS_SHRINK 0.8f

namespace zxd {

blinn_program prg;
sphere geometry;

light_vector lights;
light_model lm;
material material0;

class cell
{
protected:
  vec3 m_pos;
  vec3 m_velocity;
  GLfloat m_radius;
  GLfloat m_split_time;
  GLboolean m_mitosis = GL_FALSE;
  vec3 m_mitosis_start_pos;
  vec3 m_mitosis_end_pos;
  GLfloat m_mitosis_start_radius;
  GLfloat m_mitosis_end_radius;

public:

  cell(GLfloat radius = 3):
    m_radius(radius)
  {
    m_pos = glm::linearRand(vec3(-1), vec3(1));
    m_pos = glm::normalize(m_pos) * glm::linearRand(0.0f, RADIUS);

    reset_velocity();
  }

  void update(GLfloat dt)
  {
    if(m_mitosis)
    {
      m_split_time += dt;
      GLfloat p = m_split_time/MITOSIS_TIME;
      if(p > 1)
      {
        m_mitosis = GL_FALSE;
        m_velocity = glm::normalize(m_mitosis_end_pos - m_mitosis_start_pos);
        m_velocity *= glm::linearRand(MIN_SPEED, MAX_SPEED);
        return;
      }

      m_radius = mix(m_mitosis_start_radius, m_mitosis_end_radius, smoothstep(0.0f, 1.0f, p));
      m_pos = mix(m_mitosis_start_pos, m_mitosis_end_pos, smoothstep(0.0f, 1.0f, p));
      return;
    }

    m_pos += m_velocity * dt;

    if(glm::length2(m_pos) + 3 >= RADIUS * RADIUS && glm::dot(m_pos, m_velocity) > 0)
    {
      // reflect
      vec3 n = -glm::normalize(vec3(m_pos));
      m_velocity = glm::normalize(glm::reflect(m_velocity, -n)) * glm::linearRand(MIN_SPEED, MAX_SPEED) ;
    }
  }

  void reset_velocity()
  {
    m_velocity = glm::linearRand(vec3(-1), vec3(1));
    m_velocity = glm::normalize(m_velocity) * glm::linearRand(MIN_SPEED, MAX_SPEED);
  }

  mat4 m_m_mat()
  {
    return glm::scale(glm::translate(m_pos), vec3(m_radius));
  }

  void draw()
  {
    geometry.draw();
  }
  
  vec3 pos() const { return m_pos; }
  void pos(vec3 v){ m_pos = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  void mitosis(const vec3& mitosis_dir)
  {
    m_mitosis = GL_TRUE;
    m_mitosis_start_radius = m_radius;
    m_mitosis_end_radius = m_radius * MITOSIS_SHRINK;
    m_mitosis_start_pos = m_pos;
    m_mitosis_end_pos = m_pos + mitosis_dir * m_mitosis_end_radius;
    m_split_time = 0;
  }
};

typedef std::vector<cell> cell_vector;

class mitosis_app : public app {
protected:
  bitmap_text m_text;
  cell_vector m_cells;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene() {

    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    light_source light0;
    light0.diffuse = vec4(1);
    light0.position = vec4(1, -1, 1, 0);
    light0.specular = vec4(1, 1, 1, 1);
    lights.push_back(light0);

    material0.diffuse = vec4(0,0.6,0,1);
    material0.specular = vec4(0.5, 0.5, 0.5, 1);
    material0.shininess = 90;

    lm.local_viewer = true;

    geometry.slice(32);
    geometry.stack(32);
    geometry.include_normal(true);
    geometry.build_mesh();

    prg.init();
    prg.v_mat = glm::lookAt(vec3(RADIUS*1.5, -RADIUS*1.5, RADIUS*1.5), vec3(0), vec3(0, 0, 1));
    prg.p_mat = glm::perspective(fpi4, wnd_aspect(), 1.0f, 5000.0f);
    set_v_mat(&prg.v_mat);

    prg.bind_lighting_uniform_locations(lights, lm, material0);

    m_cells.resize(NUM_CELLS);
  }

  virtual void update() {
    for(auto iter = m_cells.begin(); iter != m_cells.end(); ++iter){
      iter->update(m_delta_time);
    }
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    prg.use();
    prg.update_lighting_uniforms(lights, lm, material0);

    geometry.bind_vn(prg.al_vertex, prg.al_normal);

    std::for_each(m_cells.begin(), m_cells.end(),[&](decltype(*m_cells.begin()) v) {
      prg.update_model(v.m_m_mat());
      v.draw();
    });

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_text.print("", 10, m_info.wnd_height - 10);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
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

  virtual void glfw_mouse_button(
    GLFWwindow *wnd, int button, int action, int mods)
  {
    app::glfw_mouse_button(wnd, button, action, mods);

    if(button != GLFW_MOUSE_BUTTON_LEFT || action != GLFW_PRESS)
      return;

    vec3 click_pos = this->unproject(vec3(current_mouse_position(), 0), prg.p_mat * prg.v_mat);
    vec3 camera_pos = glm::column(glm::inverse(prg.v_mat), 3).xyz();
    vec3 ray_dir = glm::normalize(click_pos - camera_pos);
    GLfloat d2 = 100000000;
    cell_vector::iterator it = m_cells.end();
    for(auto iter = m_cells.begin(); iter != m_cells.end(); ++iter)
    {
      GLfloat t = glm::length2(camera_pos - iter->pos());
      // only mitosis the nearest clicked one
      if(t > d2)
        continue;

      GLfloat ray_distance2 = zxd::ray_point_distance2(iter->pos(), camera_pos, ray_dir);
      if(ray_distance2 != -1 && ray_distance2 <= iter->radius() * iter->radius())
      {
        it = iter;
        d2 = t;
      }
    }

    if(it != m_cells.end())
    {
      vec3 mitosis_dir = glm::normalize(glm::linearRand(vec3(-1), vec3(1)));
      it->mitosis(mitosis_dir);

      cell new_cell(it->radius());
      new_cell.pos(it->pos());
      new_cell.mitosis(-mitosis_dir);

      m_cells.push_back(new_cell);
    }

  }
};
}

int main(int argc, char *argv[]) {
  zxd::mitosis_app app;
  app.run();
  return 0;
}
