#include "glad/glad.h"
#include "app.h"
#include "program.h"
#include "quad.h"

namespace zxd {

GLfloat time_scale = 5;
GLfloat length_scale = 60;
GLfloat fps = 0;
GLint follow_mouse = 0;
vec2 origin;

quad quad0;

struct circular_program : public program {
  GLint al_vertex;

  GLint ul_time;
  GLint ul_time_scale;
  GLint ul_resolution;
  GLint ul_length_scale;
  GLint ul_origin;
  GLint ul_use_origin;

  virtual void attach_shaders() {
    attach_shader_file(GL_VERTEX_SHADER, "data/shader/circular.vs.glsl");
    attach_shader_file(GL_FRAGMENT_SHADER, "data/shader/circular.fs.glsl");
  };
  virtual void bind_uniform_locations() {
    uniform_location(&ul_time, "time");
    uniform_location(&ul_resolution, "resolution");
    uniform_location(&ul_time_scale, "time_scale");
    uniform_location(&ul_length_scale, "length_scale");
    uniform_location(&ul_origin, "origin");
    uniform_location(&ul_use_origin, "use_origin");
  };
  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
  };

} prg0;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "circular";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 512;
    m_info.wnd_height = 256;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    prg0.init();

    quad0.build_mesh();
    quad0.bind(prg0.al_vertex, -1, -1);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(prg0.object);
    glUniform1f(prg0.ul_length_scale, length_scale);
    glUniform1f(prg0.ul_time_scale, time_scale);
    glUniform1i(prg0.ul_use_origin, follow_mouse);
    glUniform1f(prg0.ul_time, m_current_time);
    glUniform2f(prg0.ul_resolution, m_info.wnd_width, m_info.wnd_height);
    glUniform1i(prg0.ul_use_origin, follow_mouse);
    glUniform2fv(prg0.ul_origin, 1, value_ptr(origin));

    quad0.draw();

    glUseProgram(0);
    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, m_info.wnd_height - 20);
    GLchar info[512];
    sprintf(info,
      "fps : %.2f\n"
      "qQ : length scale %.2f\n"
      "wW : time scale %.2f\n"
      "e : toggle follow_mouse %d",
      m_fps, length_scale, time_scale, follow_mouse);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

    glutSwapBuffers();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        length_scale += 0.5;
        break;
      case 'Q':
        length_scale -= 0.5;
        break;
      case 'w':
        time_scale += 0.5;
        break;
      case 'W':
        time_scale -= 0.5;
        break;
      case 'e':
        follow_mouse = !follow_mouse;
        break;
      default:
        break;
    }
  }

  void mouse(int button, int state, int x, int y) {
    origin = vec2(x, glut_to_gl(y));
  }

  void passive_motion(int x, int y) { origin = vec2(x, glut_to_gl(y)); }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
