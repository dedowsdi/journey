/*
 *  planet.c
 *  this program shows how to composite modeling transformations
 *  to draw translated and rotated models.
 *  interaction:  pressing the d and y keys (day and year)
 *  alters the rotation of the planet around the sun.
 */
#include <app.h>

namespace zxd {

int year = 0, day = 0;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "planet";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);

    glPushMatrix();

    glutWireSphere(1.0, 20, 16); /* draw sun at world center */

    glRotatef((GLfloat)year, 0.0, 1.0, 0.0);
    glTranslatef(2.0, 0.0, 0.0);
    glRotatef(25, 0, 0, 1);  // tilt
    glRotatef((GLfloat)day, 0.0, 1.0, 0.0);

    glutWireSphere(0.2, 10, 8); /* draw smaller planet */

    // draw  moons
    int num_moons = 8;
    for (int i = 0; i < num_moons; ++i) {
      glPushMatrix();
      glRotatef(360 * i / num_moons, 0, 1, 0);
      glTranslatef(0.25f, 0, 0);
      glutWireSphere(0.04, 10, 8);

      glPopMatrix();
    }

    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 780);
    const GLubyte info[] =
      "qQ : change day \n"
      "eE : change year \n";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        day = (day + 10) % 360;
        break;
      case 'Q':
        day = (day - 10) % 360;
        break;
      case 'e':
        year = (year + 5) % 360;
        break;
      case 'E':
        year = (year - 5) % 360;
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
