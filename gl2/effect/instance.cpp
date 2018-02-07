#include "glad/glad.h"
#include "app.h"
#include "program.h"
#include "light.h"
#include "sphere.h"
#include <sstream>

namespace zxd {

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512

vec3 camera_pos = vec3(0, -30, 30);

std::vector<zxd::light_source> lights;
zxd::light_model light_model;
zxd::material material;

zxd::sphere sphere0(0.5, 16, 16);

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "instance";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = WINDOWS_WIDTH;
    m_info.wnd_height = WINDOWS_HEIGHT;
  }

  GLuint num_instance = 1000;

  struct instance_program : public zxd::program {
    // GLint ul_eye;
    GLint al_vertex;
    GLint al_normal;
    GLint al_mvp_mat;
    GLint al_mv_mat;
    GLint al_mv_mat_it;

    virtual void update_frame() {
      v_mat_it = glm::inverse(glm::transpose(v_mat));

      material.update_uniforms();
      light_model.update_uniforms();
      for (int i = 0; i < lights.size(); ++i) {
        lights[i].update_uniforms(v_mat);
      }
    }
    virtual void model(const glm::mat4& m_mat) {
      // everything is done on vertex attribute
    }
    virtual void attach_shaders() {
      attach_shader_file(GL_VERTEX_SHADER, "data/shader/instance.vs.glsl");
      string_vector sv;
      sv.push_back("#define LIGHT_COUNT 8\n");
      sv.push_back(read_file("data/shader/blinn.frag"));
      attach_shader_source_and_file(
        GL_FRAGMENT_SHADER, sv, "data/shader/blinn.fs.glsl");
    }
    virtual void bind_uniform_locations() {
      light_model.bind_uniform_locations(object, "lm");
      for (int i = 0; i < lights.size(); ++i) {
        std::stringstream ss;
        ss << "lights[" << i << "]";
        lights[i].bind_uniform_locations(object, ss.str());
      }
      material.bind_uniform_locations(object, "mtl");
    }

    virtual void bind_attrib_locations() {
      al_vertex = attrib_location("vertex");
      al_normal = attrib_location("normal");
      al_mvp_mat = attrib_location("mvp_mat");
      al_mv_mat = attrib_location("mv_mat");
      al_mv_mat_it = attrib_location("mv_mat_it");
    }

  } prg;

  void render(zxd::program& program) {
    prg.use();
    prg.update_frame();

    mat4 model = mat4(1.0f);
    prg.update_model(model);
    sphere0.draw(num_instance);
  }

  // update transform attribute
  void reset_mat_attribute() {
    std::vector<mat4> mvp_mats;
    std::vector<mat4> mv_mats;
    std::vector<mat4> mv_mat_its;
    mvp_mats.reserve(num_instance);
    mv_mats.reserve(num_instance);
    mv_mat_its.reserve(num_instance);

    for (int i = 0; i < num_instance; ++i) {
      mat4 m_mat = glm::translate(
        glm::linearRand(vec3(-10.0, -10.0, -10.0), vec3(10.0, 10.0, 10.0)));
      mat4 mv_mat = prg.v_mat * m_mat;
      mat4 mvp_mat = prg.p_mat * mv_mat;
      mat4 mv_mat_it = glm::inverse(glm::transpose(mv_mat));

      mv_mats.push_back(mv_mat);
      mvp_mats.push_back(mvp_mat);
      mv_mat_its.push_back(mv_mat_it);
    }

    glBindVertexArray(sphere0.vao());

    {
      GLuint buffer;
      glGenBuffers(1, &buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, mvp_mats.size() * sizeof(mat4),
        value_ptr(mvp_mats[0]), GL_STATIC_DRAW);

      matrix_attrib_pointer(prg.al_mvp_mat);
    }

    {
      GLuint buffer;
      glGenBuffers(1, &buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, mv_mats.size() * sizeof(mat4),
        value_ptr(mv_mats[0]), GL_STATIC_DRAW);

      matrix_attrib_pointer(prg.al_mv_mat);
    }

    {
      GLuint buffer;
      glGenBuffers(1, &buffer);
      glBindBuffer(GL_ARRAY_BUFFER, buffer);
      glBufferData(GL_ARRAY_BUFFER, mv_mat_its.size() * sizeof(mat4),
        value_ptr(mv_mat_its[0]), GL_STATIC_DRAW);

      matrix_attrib_pointer(prg.al_mv_mat_it);
    }
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render(prg);

    glDisable(GL_TEXTURE_2D);

    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 492);
    GLchar info[512];

    // clang-format off
  sprintf(info, 
      "q : num_instance : %d\n"
      "fps : %.2f",
      num_instance,
      m_fps
      );
    // clang-format on

    glUseProgram(0);

    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

    glutSwapBuffers();
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    // glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    zxd::light_source dir_light;
    dir_light.position = vec4(1, 0, 0, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);

    zxd::light_source point_light;
    point_light.position = vec4(0, 0, 5, 1);
    point_light.diffuse = vec4(0, 1, 0, 1);
    point_light.specular = vec4(1, 1, 1, 1);

    zxd::light_source spot_light;
    spot_light.position = vec4(-30, 0, 0, 1);
    spot_light.diffuse = vec4(0, 0, 1, 1);
    spot_light.specular = vec4(0, 0, 1, 1);
    spot_light.spot_direction = vec3(vec3(0) - spot_light.position.xyz());
    spot_light.spot_cutoff = 30;
    spot_light.spot_cos_cutoff = std::cos(spot_light.spot_cutoff);
    spot_light.spot_exponent = 3;

    lights.push_back(dir_light);
    lights.push_back(point_light);
    lights.push_back(spot_light);

    material.shininess = 80;
    material.specular = vec4(1.0, 1.0, 1.0, 1.0);

    prg.init();
    prg.p_mat =
      glm::perspective(glm::radians(45.0f), wnd_aspect(), 0.1f, 50.0f);
    prg.v_mat = glm::lookAt(camera_pos, vec3(0, 0, 0), vec3(0, 0, 1));

    sphere0.build_mesh();
    sphere0.bind(prg.al_vertex, prg.al_normal);

    reset_mat_attribute();
  }

  void reshape(int w, int h) { app::reshape(w, h); }

  void mouse(int button, int state, int x, int y) {
    switch (button) {
      default:
        break;
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        num_instance += 100;
        reset_mat_attribute();
        break;

      case 'Q':
        num_instance -= 100;
        reset_mat_attribute();
        break;

      default:
        break;
    }
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
