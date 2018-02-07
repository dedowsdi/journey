#include "glad/glad.h"
#include "app.h"
#include "program.h"
#include "light.h"

namespace zxd {

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wnd_aspect = 1;

vec3 camera_pos = vec3(0, -3, 3);

std::vector<zxd::light_source> lights;
zxd::light_model light_model;
zxd::material material;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "blinn";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = WINDOWS_WIDTH;
    m_info.wnd_height = WINDOWS_HEIGHT;
  }

  struct my_program : public zxd::program {
    // GLint ul_eye;
    my_program() {}
    virtual void do_update_frame() {
      v_mat_it = glm::inverse(glm::transpose(v_mat));

      material.update_uniforms();
      light_model.update_uniforms();
      for (int i = 0; i < lights.size(); ++i) {
        lights[i].update_uniforms(v_mat);
      }
    }
    virtual void update_model(const glm::mat4& _m_mat) {
      // m_mat_i = glm::inverse(m_mat);
      m_mat = _m_mat;
      mv_mat = v_mat * m_mat;
      mv_mat_it = glm::inverse(glm::transpose(mv_mat));
      mvp_mat = p_mat * mv_mat;

      glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
      glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
      glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
    }
    virtual void attach_shaders() {
      attach(GL_VERTEX_SHADER, "data/shader/blinn.vs.glsl");
      string_vector sv;
      sv.push_back("#define LIGHT_COUNT 8\n");
      sv.push_back(read_file("data/shader/blinn.frag"));
      attach(
        GL_FRAGMENT_SHADER, sv, "data/shader/blinn.fs.glsl");
    }
    virtual void bind_uniform_locations() {
      light_model.bind_uniform_locations(object, "light_model");
      for (int i = 0; i < lights.size(); ++i) {
        std::stringstream ss;
        ss << "lights[" << i << "]";
        lights[i].bind_uniform_locations(object, ss.str());
      }
      material.bind_uniform_locations(object, "material");

      // set_uniform_location(&ul_eye, "eye");
      set_uniform_location(&ul_mv_mat, "mv_mat");
      set_uniform_location(&ul_mv_mat_it, "mv_mat_it");
      set_uniform_location(&ul_mvp_mat, "mvp_mat");
    }
    virtual void bind_attrib_locations() {}
  } my_program;

  void render(zxd::program& program) {
    mat4 model = mat4(1.0f);
    my_program.update_frame();
    my_program.update_model(model);
    glutSolidSphere(1, 64, 64);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render(my_program);

    glDisable(GL_TEXTURE_2D);

    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 492);
    GLchar info[512];

    // clang-format off
  sprintf(info, "");
    // clang-format on

    glUseProgram(0);

    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

    glutSwapBuffers();
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    zxd::light_source dir_light;
    dir_light.position = vec4(1, 0, 0, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);

    zxd::light_source point_light;
    point_light.position = vec4(0, 0, 5, 0);
    point_light.diffuse = vec4(0, 1, 0, 1);
    point_light.specular = vec4(1, 1, 1, 1);

    zxd::light_source spot_light;
    spot_light.position = vec4(-5, 0, 0, 1);
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

    my_program.init();
    p_mat = glm::perspective(glm::radians(45.0f), wnd_aspect, 0.1f, 30.0f);
    v_mat = glm::lookAt(camera_pos, vec3(0, 0, 0), vec3(0, 0, 1));
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    wnd_aspect = static_cast<GLfloat>(w) / h;
    glLoadIdentity();
  }

  void mouse(int button, int state, int x, int y) {
    switch (button) {
      default:
        break;
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      default:
        break;
    }
  }
  void idle() {}
  void passive_motion(int x, int y) {}
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
