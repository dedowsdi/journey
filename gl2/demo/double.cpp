#include "app.h"

namespace zxd {

GLboolean spinning = GL_TRUE;
GLfloat spin = 0.0;
class app0 : public zxd::app {
  void init_info() {
    app::init_info();
    m_info.title = "double";
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glPushMatrix();
    glRotatef(spin, 0.0, 0.0, 1.0);
    glColor3f(1.0, 1.0, 1.0);
    glRectf(-25.0, -25.0, 25.0, 25.0);
    glPopMatrix();

    drawFpsText();

    glutSwapBuffers();

  }

  void update(void) {
    if(!spinning)
      return;
    spin = spin + 2.0;
    if (spin > 360.0) spin = spin - 360.0;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void mouse(int button, int state, int x, int y) {
    app::mouse(button, state, x, y);
    switch (button) {
      case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) spinning ^= 1;
        break;
    }
  }
};
}

int main(int argc, char** argv) {
  zxd::app0 ap;
  ap.run(argc, argv);
  return 0; /* ANSI C requires main to return int. */
}
