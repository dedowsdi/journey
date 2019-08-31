#include "glad/glad.h"
#include "app.h"
#include "program.h"
#include "light.h"
#include <sstream>
#include "sphere.h"
#include "common_program.h"

namespace zxd {

using namespace glm;

glm::mat4 v_mat;
glm::mat4 p_mat;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wnd_aspect = 1;

GLboolean use_program = GL_TRUE;
vec3 camera_pos = vec3(0, -3, 3);

std::vector<zxd::light_source> lights;
light_model lm;
material mtl;
sphere sphere0(1, 64, 64);

blinn_program prg0;

void render() {
  mat4 model = mat4(1.0f);
  if (use_program) {
    glUseProgram(prg0.get_object());
    prg0.update_uniforms(model, v_mat, p_mat);
    prg0.update_lighting_uniforms(lights, lm, mtl, v_mat);
    sphere0.draw();
  } else {
    // setup matrix
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(value_ptr(p_mat));

    mat4 mv_mat = model * v_mat;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(value_ptr(mv_mat));

    // set up light and mtl
    lm.pipeline();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].pipeline(i, v_mat);
    }
    mtl.pipeline();
    glutSolidSphere(1, 64, 64);
  }
}

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "blinnphong";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = WINDOWS_WIDTH;
    m_info.wnd_height = WINDOWS_HEIGHT;
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    render();

    glDisable(GL_TEXTURE_2D);

    GLint program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &program);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 492);
    GLchar info[512];

    // clang-format off
  sprintf(info, 
      " q  : use program : %d \n"
      " w  : local viewer : %d \n"
      " eE : light model ambient : %.2f %.2f %.2f %.2f \n"
      " jJ : mtl emission : %.2f %.2f %.2f %.2f \n"
      " kK : mtl diffuse : %.2f %.2f %.2f %.2f \n" 
      " lL : mtl specular : %.2f %.2f %.2f %.2f \n" 
      " ;: : mtl ambient : %.2f %.2f %.2f %.2f \n" 
      " mM : mtl shininess : %.2f \n" ,
      use_program, lm.local_viewer, 
      lm.ambient[0], lm.ambient[1], lm.ambient[2], lm.ambient[3], 
      mtl.emission[0], mtl.emission[1], mtl.emission[2], mtl.emission[3], 
      mtl.diffuse[0], mtl.diffuse[1], mtl.diffuse[2], mtl.diffuse[3], 
      mtl.specular[0], mtl.specular[1], mtl.specular[2], mtl.specular[3], 
      mtl.ambient[0], mtl.ambient[1], mtl.ambient[2], mtl.ambient[3], 
      mtl.shininess
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

    zxd::light_source dir_light;
    dir_light.position = vec4(1, 0, 0, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);

    zxd::light_source point_light;
    point_light.position = vec4(0, 0, 5, 1);
    point_light.diffuse = vec4(0, 1, 0, 1);
    point_light.specular = vec4(1, 1, 1, 1);

    zxd::light_source spot_light;
    spot_light.position = vec4(-5, 0, 0, 1);
    spot_light.diffuse = vec4(0, 0, 1, 1);
    spot_light.specular = vec4(0, 0, 1, 1);
    spot_light.spot_direction = vec3(vec3(0) - vec3(spot_light.position));
    spot_light.spot_cutoff = 30;
    spot_light.spot_cos_cutoff = std::cos(spot_light.spot_cutoff);
    spot_light.spot_exponent = 3;

    lights.push_back(dir_light);
    lights.push_back(point_light);
    lights.push_back(spot_light);

    mtl.shininess = 80;
    mtl.specular = vec4(1.0, 1.0, 1.0, 1.0);

    prg0.legacy = 1;
    prg0.light_count = 3;
    prg0.init();
    prg0.bind_lighting_uniform_locations(lights, lm, mtl);
    p_mat =
      glm::perspective(glm::radians(45.0f), wnd_aspect(), 0.1f, 30.0f);
    v_mat = glm::lookAt(camera_pos, vec3(0, 0, 0), vec3(0, 0, 1));
    this->set_v_mat(&v_mat);

    sphere0.include_normal(true);
    sphere0.build_mesh();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q': {
        use_program = !use_program;
      } break;

      case 'w':
        lm.local_viewer ^= 1;
        break;

      case 'e':
        lm.ambient += 0.05;
        lm.ambient = glm::clamp(lm.ambient, 0.0f, 1.0f);
        break;
      case 'E':
        lm.ambient -= 0.05;
        lm.ambient = glm::clamp(lm.ambient, 0.0f, 1.0f);
        break;

      case 'j':
        mtl.emission += 0.05;
        mtl.emission = glm::clamp(mtl.emission, 0.0f, 1.0f);
        break;
      case 'J':
        mtl.emission -= 0.05;
        mtl.emission = glm::clamp(mtl.emission, 0.0f, 1.0f);
        break;

      case 'k':
        mtl.diffuse += 0.05;
        mtl.diffuse = glm::clamp(mtl.diffuse, 0.0f, 1.0f);
        break;
      case 'K':
        mtl.diffuse -= 0.05;
        mtl.diffuse = glm::clamp(mtl.diffuse, 0.0f, 1.0f);
        break;

      case 'l':
        mtl.specular += 0.05;
        mtl.specular = glm::clamp(mtl.specular, 0.0f, 1.0f);
        break;
      case 'L':
        mtl.specular -= 0.05;
        mtl.specular = glm::clamp(mtl.specular, 0.0f, 1.0f);
        break;

      case ';':
        mtl.ambient += 0.05;
        mtl.ambient = glm::clamp(mtl.ambient, 0.0f, 1.0f);
        break;

      case ':':
        mtl.ambient -= 0.05;
        mtl.ambient = glm::clamp(mtl.ambient, 0.0f, 1.0f);
        break;

      case 'm':
        mtl.shininess += 5;
        mtl.shininess = glm::clamp(mtl.shininess, 0.0f, 1000.0f);
        break;
      case 'M':
        mtl.shininess -= 5;
        mtl.shininess = glm::clamp(mtl.shininess, 0.0f, 1000.0f);
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
