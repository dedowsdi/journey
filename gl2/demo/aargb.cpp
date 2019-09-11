/*
 *  aargb.c
 *  this program draws shows how to draw anti-aliased lines. it draws
 *  two diagonal lines to form an X; when 'q' is typed in the window,
 *  the lines are rotated in opposite directions.
 *
 *  in order to use rgba antialiasing, you must turn on smooth and blend, set
 *  blend func to GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA(or GL_ONE).
 *
 *  without antialiasing glLineWidth only used rounded integer, points are
 *  always rect(it becomes round if antialiasing is enabled, either rgba or
 *  multisample).
 */
#include <app.h>

namespace zxd {

float rot_angle = 0.;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "aargb";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 200;
    m_info.wnd_height = 200;
  }

  /*  initialize antialiasing for RGBA mode, including alpha
   *  blending, hint, and line width.  print out implementation
   *  specific info on line width granularity and width.
   */
  void create_scene(void) {
    GLfloat values[2];
    glGetFloatv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, values);
    printf("GL_SMOOTH_LINE_WIDTH_GRANULARITY value is %3.4f\n", values[0]);

    glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, values);
    printf("GL_ALIASED_LINE_WIDTH_RANGE values are %3.4f %3.4f\n", values[0],
      values[1]);

    glGetFloatv(GL_SMOOTH_LINE_WIDTH_RANGE, values);
    printf("GL_SMOOTH_LINE_WIDTH_RANGE values are %3.4f %3.4f\n", values[0],
      values[1]);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glLineWidth(1.5);

    glClearColor(0.0, 0.0, 0.0, 0.0);
  }

  /* draw 2 diagonal lines to form an X
   */
  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // draw two cross lines
    glColor3f(0.0, 1.0, 0.0);
    glPushMatrix();
    glRotatef(-rot_angle, 0.0, 0.0, 1);
    glBegin(GL_LINES);
    glVertex2f(-0.5, 0.5);
    glVertex2f(0.5, -0.5);
    glEnd();
    glPopMatrix();

    glColor3f(0.0, 0.0, 1.0);
    glPushMatrix();
    glRotatef(rot_angle, 0.0, 0.0, 1);
    glBegin(GL_LINES);
    glVertex2f(0.5, 0.5);
    glVertex2f(-0.5, -0.5);
    glEnd();
    glPopMatrix();

    // draw two big dots
    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(20.0f);  // can't be called between beg and end
    glBegin(GL_POINTS);
    glVertex2f(-0.5f, 0);
    glVertex2f(0.5f, 0);
    glEnd();

    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glRasterPos2f(-0.99, 0.9);

    const unsigned char info[] =
      "qQ : rotate\n"
      "w : GL_LINE_SMOOTH\n"
      "e : GL_POINT_SMOOTH\n"
      "r : GL_BLEND";

    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      gluOrtho2D(-1.0, 1.0, -1.0 * (GLfloat)h / (GLfloat)w,
        1.0 * (GLfloat)h / (GLfloat)w);
    else
      gluOrtho2D(-1.0 * (GLfloat)w / (GLfloat)h, 1.0 * (GLfloat)w / (GLfloat)h,
        -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
      case 'Q':
        rot_angle += 20.;
        if (rot_angle >= 360.) rot_angle = 0.;
        break;
      case 'w':
        if (glIsEnabled(GL_LINE_SMOOTH))
          glDisable(GL_LINE_SMOOTH);
        else
          glEnable(GL_LINE_SMOOTH);
        break;
      case 'e':
        if (glIsEnabled(GL_POINT_SMOOTH))
          glDisable(GL_POINT_SMOOTH);
        else
          glEnable(GL_POINT_SMOOTH);
        break;
      case 'r':
        if (glIsEnabled(GL_BLEND))
          glDisable(GL_BLEND);
        else
          glEnable(GL_BLEND);
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
