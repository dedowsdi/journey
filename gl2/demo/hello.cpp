#include "app.h"

namespace zxd {
class app0 : public zxd::app {
  void init_info() {
    app::init_info();
    m_info.title = "hello world";
    m_info.display_mode = GLUT_RGB | GLUT_SINGLE;
  }
  void display(void) {
    /* clear all pixels  */
    glClear(GL_COLOR_BUFFER_BIT);

    /* draw white polygon (rectangle) with corners at
     * (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
     */
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glVertex3f(0.25, 0.25, 0.0);
    glVertex3f(0.75, 0.25, 0.0);
    glVertex3f(0.75, 0.75, 0.0);
    glVertex3f(0.25, 0.75, 0.0);
    glEnd();

    /* don't wait!
     * start processing buffered open_g_l routines
     * force previously issued opengl commands to begin execution
     */
    glFlush();

  }

  void create_scene(void) {
    /* select clearing color 	*/
    glClearColor(0.0, 0.0, 0.0, 0.0);

    /* initialize viewing values  */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
  }
};
}

int main(int argc, char** argv) {
  zxd::app0 ap;
  ap.run(argc, argv);
  return 0; /* ANSI C requires main to return int. */
}
