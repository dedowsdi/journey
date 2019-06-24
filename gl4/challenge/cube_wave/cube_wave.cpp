#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"
#include "cuboid.h"

#define WIDTH 800
#define HEIGHT 800
#define CUBE_COUNT 21
#define CUBE_SIZE 20

namespace zxd {

blinn_program prg;
light_vector lights;
light_model lm;
material mtl;
cuboid cube;

glm::mat4 v_mat;
glm::mat4 p_mat;

class cube_wave_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "cube_wave_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    cube.half_diag(vec3(CUBE_SIZE*0.5));
    cube.include_normal(true);
    cube.build_mesh();

    prg.init();
    p_mat = glm::perspective(fpi4, wnd_aspect(), 1.0f, 5000.0f);
    v_mat = zxd::isometric_projection(1000);
    set_v_mat(&v_mat);

    light_source l0;
    l0.position = vec4(-1, 1, 1, 0);
    l0.diffuse = vec4(1);
    l0.specular = vec4(0.8);
    lights.push_back(l0);

    lm.ambient = vec4(0.2);
    lm.local_viewer = true;
    
    mtl.diffuse = vec4(0.8);
    mtl.specular = vec4(0.6);
    mtl.shininess = 50;
    mtl.ambient = vec4(0.8);

    prg.bind_lighting_uniform_locations(lights, lm, mtl);
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    prg.use();
    prg.update_lighting_uniforms(lights, lm, mtl, v_mat);
    prg.update_uniforms(glm::scale(vec3(100)), v_mat, p_mat);

    static GLfloat sin_offset = 0;

    vec2 sin_offset_center = vec2((CUBE_COUNT-1)/2.0);
    float offset = -CUBE_SIZE * CUBE_COUNT/2.0 + CUBE_SIZE * 0.5;
    vec3 corner = vec3(offset, offset, 0);
    GLfloat max_distance = glm::length(vec2((CUBE_COUNT - 1)/2.0));
    for (int y = 0; y < CUBE_COUNT; ++y) 
    {
      for (int x = 0; x < CUBE_COUNT; ++x) 
      {
        GLfloat distance = glm::length(vec2(x, y) - sin_offset_center);
        GLfloat cube_sin_offset = mix(-fpi, fpi, distance/max_distance);
        float h = glm::mix(1.0, 15.0, (sin(sin_offset + cube_sin_offset) + 1.0) * 0.5f);
        vec3 translation = corner + vec3(x * CUBE_SIZE, y * CUBE_SIZE, 0);
        mat4 m = glm::scale(glm::translate(translation), vec3(1, 1, h));
        prg.update_uniforms(m, v_mat, p_mat);
        cube.draw();
      }
    }
    sin_offset += 0.07;

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
  zxd::cube_wave_app app;
  app.run();
}
