/*
 *  aargb.c
 *  This program draws shows how to draw anti-aliased lines. It draws
 *  two diagonal lines to form an X; when 'q' is typed in the window,
 *  the lines are rotated in opposite directions.
 *
 *  In order to use rgba antialiasing, you must turn on smooth and blend, set
 *  blend func to GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA(or GL_ONE).
 *
 *  Without antialiasing glLineWidth only used rounded integer, points are
 *  always rect(it becomes round if antialiasing is enabled, either rgba or
 *  multisample).
 */
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>

static float rotAngle = 0.;

/*  Initialize antialiasing for RGBA mode, including alpha
 *  blending, hint, and line width.  Print out implementation
 *  specific info on line width granularity and width.
 */
void init(void) {
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

/* Draw 2 diagonal lines to form an X
 */
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  // draw two cross lines
  glColor3f(0.0, 1.0, 0.0);
  glPushMatrix();
  glRotatef(-rotAngle, 0.0, 0.0, 1);
  glBegin(GL_LINES);
  glVertex2f(-0.5, 0.5);
  glVertex2f(0.5, -0.5);
  glEnd();
  glPopMatrix();

  glColor3f(0.0, 0.0, 1.0);
  glPushMatrix();
  glRotatef(rotAngle, 0.0, 0.0, 1);
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
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    gluOrtho2D(
      -1.0, 1.0, -1.0 * (GLfloat)h / (GLfloat)w, 1.0 * (GLfloat)h / (GLfloat)w);
  else
    gluOrtho2D(
      -1.0 * (GLfloat)w / (GLfloat)h, 1.0 * (GLfloat)w / (GLfloat)h, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q':
    case 'Q':
      rotAngle += 20.;
      if (rotAngle >= 360.) rotAngle = 0.;
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

    case 27: /*  Escape Key  */
      exit(0);
      break;
    default:
      break;
  }
  glutPostRedisplay();
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(200, 200);
  glutCreateWindow(argv[0]);
  init();
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
