/*
 *  torus.c
 *  this program demonstrates the creation of a display list.
 *
 *
 *  display list is a cash of commands, you can't changed it after it's created.
 *  commands like glRotate store the computed result to boost performance.
 *
 *
 *  not all commands can not be placed in a display list:
 *    glIs*  glAre* glGen* glDel* glBind*.....(see red book 5 334)
 *
 */

#include <app.h>

namespace zxd {

#define PI_ 3.14159265358979323846
GLuint the_torus;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "torus";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  /* draw a torus */
  static void torus(int numc, int numt) {
    int i, j, k;
    double s, t, x, y, z, twopi;

    twopi = 2 * PI_;
    for (i = 0; i < numc; i++) {
      glBegin(GL_QUAD_STRIP);
      for (j = 0; j <= numt; j++) {
        for (k = 1; k >= 0; k--) {
          s = (i + k) % numc + 0.5;
          t = j % numt;

          x = (1 + .1 * cos(s * twopi / numc)) * cos(t * twopi / numt);
          y = (1 + .1 * cos(s * twopi / numc)) * sin(t * twopi / numt);
          z = .1 * sin(s * twopi / numc);
          glVertex3f(x, y, z);
        }
      }
      glEnd();
    }
  }

  /* create display list with torus and initialize state */
  void create_scene(void) {
    the_torus = glGenLists(1);
    glNewList(the_torus, GL_COMPILE);
    torus(8, 25);
    glEndList();

    glShadeModel(GL_FLAT);
    glClearColor(0.0, 0.0, 0.0, 0.0);
  }

  /* clear window and draw torus */
  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glCallList(the_torus);

    const unsigned char info[] =
      "q : spin along local x \n"
      "w : spin along local y \n"
      "e : reset rotation ";

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

    glFlush();
  }

  /* handle window resize */
  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(30, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
  }

  /* rotate about x-axis when "x" typed; rotate about y-axis
     when "y" typed; "i" returns torus to original view */
  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
      case 'Q':
        glRotatef(30., 1.0, 0.0, 0.0);
        break;
      case 'w':
      case 'W':
        glRotatef(30., 0.0, 1.0, 0.0);
        break;
      case 'e':
      case 'E':
        glLoadIdentity();
        gluLookAt(0, 0, 10, 0, 0, 0, 0, 1, 0);
        break;
    }
  }
};
}
int main(int argc, char **argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
