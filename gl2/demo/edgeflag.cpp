/*
 * if edge flag of current vertex is true:
 *   line : line will be drawn between current vertex and the next one
 *   point : current point will be drawn
 */
#include <app.h>

namespace zxd {

GLenum polygon_mode = GL_LINE;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "edge_flag";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
    m_info.wnd_width = 300;
    m_info.wnd_height = 300;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK,
      polygon_mode);  // don't put this between glBegin and glEnd

    /* select white for all lines  */
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_POLYGON);
    glEdgeFlag(GL_TRUE);
    glVertex2f(100, 100);
    glEdgeFlag(GL_FALSE);
    glVertex2f(200, 100);
    glEdgeFlag(GL_TRUE);
    glVertex2f(200, 200);
    glEdgeFlag(GL_FALSE);
    glVertex2f(100, 200);
    glEnd();

    glRasterPos2f(10, 280);
    glutBitmapString(
      GLUT_BITMAP_9_BY_15, (const unsigned char*)"p : switch polygon mode");

    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 27:
        exit(0);
        break;
      case 'p':
        polygon_mode = GL_POINT + (polygon_mode + 1 - GL_POINT) % 3;
        glutPostRedisplay();
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
