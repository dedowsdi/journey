#include "glad/glad.h"
#include "app.h"
#include "program.h"
#include "light.h"
#include <sstream>
#include "sphere.h"

namespace zxd {

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512

vec3 rim_color = vec3(1, 0, 0);
GLfloat rim_power = 10.0f;

std::vector<zxd::light_source> lights;
light_model lm;
material mtl;
sphere sphere0(1, 32, 32);

struct rim_program : public zxd::program {
  GLint ul_rim_color;
  GLint ul_rim_power;
  GLint al_vertex;
  GLint al_normal;

  virtual void update_frame() {
    v_mat_it = glm::inverse(glm::transpose(v_mat));

    mtl.update_uniforms();
    lm.update_uniforms();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].update_uniforms(v_mat);
    }

    glUniform3fv(ul_rim_color, 1, value_ptr(rim_color));
    glUniform1f(ul_rim_power, rim_power);
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
      GL_FRAGMENT_SHADER, sv, "data/shader/rim.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    lm.bind_uniform_locations(object, "lm");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(object, ss.str());
    }
    mtl.bind_uniform_locations(object, "mtl");

    // uniform_location(&ul_eye, "eye");
    uniform_location(&ul_mv_mat, "mv_mat");
    uniform_location(&ul_mv_mat_it, "mv_mat_it");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_rim_color, "rim_color");
    uniform_location(&ul_rim_power, "rim_power");
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_normal = attrib_location("normal");
  }
} prg0;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "rim";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = WINDOWS_WIDTH;
    m_info.wnd_height = WINDOWS_HEIGHT;
  }

  void render(zxd::program& program) {
    mat4 model = mat4(1.0f);
    glUseProgram(prg0);
    prg0.update_frame();
    prg0.update_model(model);
    sphere0.draw();
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render(prg0);

    glDisable(GL_TEXTURE_2D);

    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 492);
    GLchar info[512];

    // clang-format off
  sprintf(info, 
      "qQ : rim_power : %.2f\n"
      "w : rim_color : %.2f %.2f %.2f \n",
      rim_power, rim_color[0], rim_color[1], rim_color[2]
      );
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

    zxd::light_source point_light;
    point_light.position = vec4(0, 0, 5, 0);
    point_light.diffuse = vec4(1, 1, 1, 1);
    point_light.specular = vec4(1, 1, 1, 1);

    lights.push_back(point_light);

    lm.local_viewer = 1;

    mtl.shininess = 80;
    mtl.specular = vec4(1.0, 1.0, 1.0, 1.0);
    mtl.diffuse = vec4(0.5);
    mtl.emission = vec4(0);
    mtl.ambient = vec4(0);

    prg0.init();
    prg0.p_mat =
      glm::perspective(glm::radians(45.0f), wnd_aspect(), 0.1f, 30.0f);
    prg0.v_mat = glm::lookAt(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));

    sphere0.build_mesh();
    sphere0.bind(prg0.al_vertex, prg0.al_normal);
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        rim_power += 1;
        break;

      case 'Q':
        rim_power -= 1;
        if (rim_power < 0) {
          rim_power = 0;
        }
        break;

      case 'w':
      case 'W':
        rim_color = glm::linearRand(vec3(0), vec3(1));
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
