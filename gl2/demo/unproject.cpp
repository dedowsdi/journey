/*
 *  unproject.c
 *  when the left mouse button is pressed, this program
 *  reads the mouse position and determines two 3D points
 *  from which it was transformed.  very little is displayed.
 */
#include "app.h"

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "unproject";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
  }

  /* change these values for a different transformation  */
  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  // x y in left hand frame, orgin at left top
  void mouse(int button, int state, int x, int y) {
    GLint viewport[4];
    GLdouble mvmatrix[16], projmatrix[16];
    GLint realy;         /*  open_g_l y coordinate position  */
    GLdouble wx, wy, wz; /*  returned world x, y, z coords  */

    switch (button) {
      case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
          glGetIntegerv(GL_VIEWPORT, viewport);
          glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
          glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
          /*  note viewport[3] is height of window in pixels, that why -1 is
           *  needed, only works if viewport height is the same as window
           * height,
           *  otherwise you need window height to get realy  */
          realy = viewport[3] - (GLint)y - 1;
          printf("coordinates at cursor are (%4d, %4d)\n", x, realy);
          gluUnProject((GLdouble)x, (GLdouble)realy, 0.0, mvmatrix, projmatrix,
            viewport, &wx, &wy, &wz);
          printf("world coords at z=0.0 are (%f, %f, %f)\n", wx, wy, wz);
          gluUnProject((GLdouble)x, (GLdouble)realy, 1.0, mvmatrix, projmatrix,
            viewport, &wx, &wy, &wz);
          printf("world coords at z=1.0 are (%f, %f, %f)\n", wx, wy, wz);
        }
        break;
      case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) exit(0);
        break;
      default:
        break;
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }

  /*
   *  open window, register input callback functions
   */
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
