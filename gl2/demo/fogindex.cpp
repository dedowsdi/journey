/*
 *  fogindex.c
 *  this program draws 5 wireframe spheres, each at
 *  a different z distance from the eye, in linear fog.
 */
#include "app.h"

namespace zxd {

#define NUMCOLORS 32
#define RAMPSTART 16

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "fogindex";
    m_info.display_mode = GLUT_SINGLE | GLUT_INDEX | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  /*  initialize color map and fog.  set screen clear color
   *  to end of color ramp.
   */

  void create_scene(void) {
    int i;

    glEnable(GL_DEPTH_TEST);

    for (i = 0; i < NUMCOLORS; i++) {
      GLfloat shade;
      shade = (GLfloat)(NUMCOLORS - i) / (GLfloat)NUMCOLORS;
      glutSetColor(RAMPSTART + i, shade, shade, shade);
    }
    glEnable(GL_FOG);

    glFogi(GL_FOG_MODE, GL_LINEAR);
    glFogi(GL_FOG_INDEX, NUMCOLORS);
    glFogf(GL_FOG_START, 1.0);
    glFogf(GL_FOG_END, 6.0);
    glHint(GL_FOG_HINT, GL_NICEST);
    glClearIndex((GLfloat)(NUMCOLORS + RAMPSTART - 1));
  }

  static void render_sphere(GLfloat x, GLfloat y, GLfloat z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glutWireSphere(0.4, 16, 16);
    glPopMatrix();
  }

  /*  display() draws 5 spheres at different z positions.
   */
  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glIndexi(RAMPSTART);

    render_sphere(-2., -0.5, -1.0);
    render_sphere(-1., -0.5, -2.0);
    render_sphere(0., -0.5, -3.0);
    render_sphere(1., -0.5, -4.0);
    render_sphere(2., -0.5, -5.0);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      glOrtho(-2.5, 2.5, -2.5 * (GLfloat)h / (GLfloat)w,
        2.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
      glOrtho(-2.5 * (GLfloat)w / (GLfloat)h, 2.5 * (GLfloat)w / (GLfloat)h,
        -2.5, 2.5, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }

  /*  main loop
   *  open window with initial window size, title bar,
   *  color index mode, depth buffer, and handle input events.
   */
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
