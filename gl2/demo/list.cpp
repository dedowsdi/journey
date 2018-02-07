/*
 *  list.c
 *  this program demonstrates how to make and execute a
 *  display list.  note that attributes, such as current
 *  color and matrix, are changed.
 */
#include "app.h"

namespace zxd {

GLuint list_name;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "list";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 800;
    m_info.wnd_height = 50;
  }

  void create_scene(void) {
    list_name = glGenLists(1);

    glNewList(list_name, GL_COMPILE);
    /*glPushAttrib(GL_CURRENT_BIT);*/  // you can use this to restore current
                                       // color
    glColor3f(1.0, 0.0, 0.0);          /*  current color red  */
    glBegin(GL_TRIANGLES);
    glVertex2f(0.0, 0.0);
    glVertex2f(1.0, 0.0);
    glVertex2f(0.0, 1.0);
    glEnd();
    glTranslatef(1.5, 0.0, 0.0); /*  move position  */
    /*glPopAttrib();*/
    glEndList();
    glShadeModel(GL_FLAT);
  }

  void draw_line(void) {
    glBegin(GL_LINES);
    glVertex2f(0.0, 0.5);
    glVertex2f(15.0, 0.5);
    glEnd();
  }

  void display(void) {
    GLuint i;

    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glColor3f(0.0, 1.0, 0.0); /*  current color green  */
    for (i = 0; i < 10; i++) /*  draw 10 triangles, also change current color */
      glCallList(list_name);
    draw_line(); /*  is this line green?  NO!  */
                 /*  where is the line drawn?  */
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      gluOrtho2D(0.0, 2.0, -0.5 * (GLfloat)h / (GLfloat)w,
        1.5 * (GLfloat)h / (GLfloat)w);
    else
      gluOrtho2D(0.0, 2.0 * (GLfloat)w / (GLfloat)h, -0.5, 1.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
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
