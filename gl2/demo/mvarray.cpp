/*
 *  mvarray.c
 *  this program demonstrates multiple vertex arrays,
 *  specifically the opengl routine glMultiDrawElements().
 *
 *  you can render multiple sets of primitives in a single call:
 *    glMultiDrawArrays
 *    glMultiDrawElements
 */
#include "app.h"

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "mvarray";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 350;
    m_info.wnd_height = 350;
  }

  void setup_pointer(void) {
    // clang-format off
  static GLint vertices[] = {
    25, 25, 75,
    75, 100, 125,
    150, 75, 200,
    175, 250, 150,
    300, 125, 100,
    200, 150, 250,
    200, 225, 250,
    300, 300, 250
  };
    // clang-format on

    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(2, GL_INT, 0, vertices);
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    setup_pointer();
  }

  void display(void) {
    // use the same vertices to create two indices
    static GLubyte one_indices[] = {0, 1, 2, 3, 4, 5, 6};
    static GLubyte two_indices[] = {1, 7, 8, 9, 10, 11};
    static GLsizei count[] = {7, 6};
    static const GLvoid* indices[2] = {one_indices, two_indices};

    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glMultiDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_BYTE, indices, 2);
    glFlush();
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
    }
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
