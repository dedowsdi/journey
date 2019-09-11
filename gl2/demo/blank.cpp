/*
 *  blank.c
 *  tmp program, always wipe clean after you experiment
 */
#include <app.h>

namespace zxd {

#define draw_one_line(x1, y1, x2, y2) \
  glBegin(GL_LINES);                  \
  glVertex2f((x1), (y1));             \
  glVertex2f((x2), (y2));             \
  glEnd();

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "blank";
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

    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    glNormal3f(0, 0, 1);
    glColor3f(1.0f, 1.0f, 1.0f);
    glRectd(-1, -1, 1, 1);

    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glEndList();

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
    switch (key) {}
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
