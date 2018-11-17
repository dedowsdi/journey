#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include "sphere.h"
#include "texutil.h"

#define WIDTH 800
#define HEIGHT 800
#define SPEED 1.2
#define NUM_ORBITS 4

namespace zxd {

blinn_program prg;
light_vector lights;
light_model lm;
material orbit_mtl;
material sun_mtl;
sphere geometry;

GLuint textures[6];
std::string texture_files[6] = {"sunmap.jpg", "mercurymap.jpg", "venusmap.jpg", "earthmap.jpg", "marsmap.jpg", "jupitermap.jpg"};

class planet
{
protected:
  std::vector<planet> m_orbits;
  vec3 m_axis; // rotatoin axix
  vec3 m_unit_pos;
  GLfloat m_angle;  // rotating angle
  GLfloat m_distance; // distance from mother
  GLfloat m_radius;
  GLfloat m_speed;
  GLuint m_texture;

public:

  planet(GLfloat radius = 1, GLfloat distance = 0):
    m_radius(radius),
    m_distance(distance)
  {
    m_angle = glm::linearRand(0.0f, f2pi);
    m_speed = glm::linearRand(0.0, SPEED);
    m_unit_pos = glm::sphericalRand(1.0); // raondom start unit positoin
    // rotation axis should perp to unit pos
    vec3 rv = glm::sphericalRand(1.0); // random vector that's not parallel to start unit position
    while(abs(dot(rv, m_unit_pos)) > 0.99)
      rv = glm::sphericalRand(1.0);

    m_axis = glm::normalize(glm::cross(rv, m_unit_pos));
    m_texture = textures[glm::linearRand(1, 5)];
  }

  void update(GLfloat dt)
  {
    m_angle += dt * m_speed;
    for(auto iter = m_orbits.begin(); iter != m_orbits.end(); ++iter){
      iter->update(dt);
    }
  }

  mat4 m_mat()
  {
    return m_mat_no_scale() * scale(vec3(m_radius));
  };

  mat4 m_mat_no_scale()
  {
    return rotate(m_angle, m_axis) * translate(m_unit_pos * m_distance);
  };

  void draw(material& mtl, const mat4& parent_m_mat = mat4(1))
  {
    glBindTexture(GL_TEXTURE_2D, m_texture);
    prg.tex_unit = 0;
    mat4 m = parent_m_mat * m_mat();
    prg.update_model(m);
    prg.update_lighting_uniforms(lights, lm, mtl);

    geometry.draw();
    for(auto iter = m_orbits.begin(); iter != m_orbits.end(); ++iter){
      iter->draw(orbit_mtl, parent_m_mat * m_mat_no_scale());
    }
  }

  GLfloat speed() const { return m_speed; }
  void speed(GLfloat v){ m_speed = v; }

  GLuint texture() const { return m_texture; }
  void texture(GLuint v){ m_texture = v; }

  void spawn_orbits(GLint level)
  {
    if(level == 0)
      return;

    --level;
    GLint num_orbits = glm::linearRand(1, NUM_ORBITS);
    while(num_orbits--)
    {
      GLfloat r = m_radius * 0.5;
      GLfloat d = m_radius + r;
      d += glm::linearRand(d*2, d * 6) * glm::pow(0.9, level) ;
      m_orbits.push_back(planet(r, d));
      m_orbits.back().spawn_orbits(level);
    }
    
  }
};

class solar_app : public app {
protected:
  bitmap_text m_text;
  planet m_planet;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    light_source light0;
    light0.position = vec4(0, 0, 0, 1);
    light0.diffuse = vec4(1, 1, 1, 1);
    light0.specular = vec4(1, 1, 1, 1);
    light0.ambient = vec4(0.15, 0.15, 0.15, 0.15);
    lights.push_back(light0);

    orbit_mtl.diffuse = vec4(1, 1, 1 ,1);
    orbit_mtl.specular = vec4(0.5, 0.5, 0.5, 0.5);
    orbit_mtl.shininess = 50;

    //sun_mtl.ambient = vec4(0.2, 0.2, 0.2, 1);
    sun_mtl.emission = vec4(1, 1, 1, 1);

    lm.local_viewer = true;

    prg.with_texcoord = GL_TRUE;
    prg.init();
    prg.v_mat = glm::lookAt(vec3(0, -10, 10), vec3(0), pza);
    prg.p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
    prg.bind_lighting_uniform_locations(lights, lm, orbit_mtl);
    sun_mtl.bind_uniform_locations(prg, "mtl");
    set_v_mat(&prg.v_mat);

    geometry.slice(32);
    geometry.stack(32);
    geometry.include_normal(true);
    geometry.include_texcoord(true);
    geometry.build_mesh();

    glGenTextures(6, textures);
    for (size_t i = 0; i < 6; ++i) 
    {
      glBindTexture(GL_TEXTURE_2D, textures[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      fipImage fi = fipLoadResource("texture/" + texture_files[i]);
      
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fi.getWidth(), fi.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, fi.accessPixels());
    }

    m_planet.speed(0);
    m_planet.texture(textures[0]);
    m_planet.spawn_orbits(3);

  }

  virtual void update() {
    m_planet.update(m_delta_time);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    prg.use();

    m_planet.draw(sun_mtl);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_text.print("", 10, m_info.wnd_height - 20);
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
};
}

int main(int argc, char *argv[]) {
  zxd::solar_app app;
  app.run();
}
