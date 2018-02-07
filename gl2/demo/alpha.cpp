/*
 *  alpha.c
 *  this program draws several overlapping filled polygons
 *  to demonstrate the effect order has on alpha blending results.
 *  use the 't' key to toggle the order of drawing polygons.
 */
#include "app.h"

namespace zxd {

static int left_first = GL_TRUE;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "alpha";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 200;
    m_info.wnd_height = 200;
  }

  /*  initialize alpha blending function.
   */
  void create_scene(void) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glShadeModel(GL_FLAT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
  }

  void draw_left_triangle(void) {
    /* draw yellow triangle on LHS of screen */

    glBegin(GL_TRIANGLES);
    glColor4f(1.0, 1.0, 0.0, 0.75);
    glVertex3f(0.1, 0.9, 0.0);
    glVertex3f(0.1, 0.1, 0.0);
    glVertex3f(0.7, 0.5, 0.0);
    glEnd();
  }

  void draw_right_triangle(void) {
    /* draw cyan triangle on RHS of screen */

    glBegin(GL_TRIANGLES);
    glColor4f(0.0, 1.0, 1.0, 0.75);
    glVertex3f(0.9, 0.9, 0.0);
    glVertex3f(0.3, 0.5, 0.0);
    glVertex3f(0.9, 0.1, 0.0);
    glEnd();
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    if (left_first) {
      draw_left_triangle();
      draw_right_triangle();
    } else {
      draw_right_triangle();
      draw_left_triangle();
    }

    glDisable(GL_BLEND);
    glColor3f(1.0, 1.0, 1.0);
    glWindowPos2i(10, 180);
    const GLubyte info[] = "t : toggle sequence \n";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);
    glEnable(GL_BLEND);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      gluOrtho2D(0.0, 1.0, 0.0, 1.0 * (GLfloat)h / (GLfloat)w);
    else
      gluOrtho2D(0.0, 1.0 * (GLfloat)w / (GLfloat)h, 0.0, 1.0);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 't':
      case 'T':
        left_first = !left_first;
        break;
      default:
        break;
    }
  }

  /*  main loop
   *  open window with initial window size, title bar,
   *  RGBA display mode, and handle input events.
   */
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
