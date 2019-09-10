#include <sstream>

#include "glad/glad.h"
#include "app.h"
#include "program.h"
#include "light.h"
#include "sphere.h"
#include "stream_util.h"

namespace zxd {

using namespace glm;

glm::mat4 v_mat;
glm::mat4 p_mat;

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
  GLint ul_mv_mat_it;
  GLint ul_mv_mat;
  GLint ul_mvp_mat;

  mat4 v_mat_it;

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
  virtual void update_uniforms(const glm::mat4& m_mat, const glm::mat4& v_mat, const glm::mat4& p_mat) {
    // m_mat_i = glm::inverse(m_mat);
    mat4 mv_mat = v_mat * m_mat;
    mat4 mv_mat_it = glm::inverse(glm::transpose(mv_mat));
    mat4 mvp_mat = p_mat * mv_mat;

    glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
    glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  }
  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "shader2/blinn.vs.glsl");
    string_vector sv;
    sv.push_back("#define LIGHT_COUNT 1\n");
    sv.push_back(stream_util::read_resource("shader2/blinn.frag"));
    attach(
      GL_FRAGMENT_SHADER, sv, "shader2/rim.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    lm.bind_uniform_locations(get_object(), "lm");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(get_object(), ss.str());
    }
    mtl.bind_uniform_locations(get_object(), "mtl");

    ul_mv_mat = get_uniform_location("mv_mat");
    ul_mv_mat_it = get_uniform_location("mv_mat_it");
    ul_mvp_mat = get_uniform_location("mvp_mat");
    ul_rim_color = get_uniform_location("rim_color");
    ul_rim_power = get_uniform_location("rim_power");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
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
    prg0.update_uniforms(model, v_mat, p_mat);
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
    p_mat =
      glm::perspective(glm::radians(45.0f), wnd_aspect(), 0.1f, 30.0f);
    v_mat = glm::lookAt(vec3(0, 0, 3), vec3(0, 0, 0), vec3(0, 1, 0));

    sphere0.build_mesh({attrib_semantic::vertex, attrib_semantic::normal});
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
