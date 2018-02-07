/*
 *  cube.c
 *  this program demonstrates a single modeling transformation,
 *  glScalef() and a single viewing transformation, gluLookAt().
 *  A wireframe cube is rendered.
 *
 *  gluPerspective, glFrustum takes only positive near and far plane.
 *  glOrtho takes both positive and negative near and far plane.
 */
#include "app.h"

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "cube";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glLoadIdentity(); /* clear the matrix */
                      /* viewing transformation  */
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glScalef(1.0, 2.0, 1.0); /* modeling transformation */
    glutWireCube(1.0);
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // both near and far plane must be positive.
    glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
    glMatrixMode(GL_MODELVIEW);  // current matrix
  }

  void keyboard(unsigned char key, int x, int y) { app::keyboard(key, x, y); }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
