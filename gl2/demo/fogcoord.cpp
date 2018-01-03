/*
 *  fogcoord.c
 *
 *  This program demonstrates the use of explicit fog
 *  coordinates.  You can press the keyboard and change
 *  the fog coordinate value at any vertex.  You can
 *  also switch between using explicit fog coordinates
 *  and the default fog generation mode.
 *
 *  Pressing the 'f' and 'b' keys move the viewer forward
 *  and backwards.
 *  Pressing 'c' initiates the default fog generation.
 *  Pressing capital 'C' restores explicit fog coordinates.
 *  Pressing '1', '2', '3', '8', '9', and '0' add or
 *  subtract from the fog coordinate values at one of the
 *  three vertices of the triangle.
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>

static GLfloat f1, f2, f3;

/*  Initialize fog
 */
static void init(void) {
  GLfloat fogColor[4] = {0.0, 0.25, 0.25, 1.0};
  f1 = 1.0f;
  f2 = 5.0f;
  f3 = 10.0f;

  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_EXP);
  glFogfv(GL_FOG_COLOR, fogColor);
  glFogf(GL_FOG_DENSITY, 0.25);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
  glFogi(GL_FOG_COORDINATE_SOURCE, GL_FOG_COORDINATE);
  glClearColor(0.0, 0.25, 0.25, 1.0); /* fog color */
}

/* display() draws a triangle at an angle.
 */
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(1.0f, 0.75f, 0.0f);
  glBegin(GL_TRIANGLES);
  glFogCoordf(f1);
  glVertex3f(2.0f, -2.0f, 0.0f);
  glFogCoordf(f2);
  glVertex3f(-2.0f, 0.0f, -5.0f);
  glFogCoordf(f3);
  glVertex3f(0.0f, 2.0f, -10.0f);
  glEnd();

  glDisable(GL_FOG);
  glColor3f(1.0,1.0,1.0);
  const unsigned char info[] =
    "q : toggle GL_FOG_COORDINATE_SOURCE between\n"
    "    fragment_depth and coordinate_ext\n"
    "wW : change fog coordinate of vertex 0\n"
    "eE : change fog coordinate of vertex 1\n"
    "rR : change fog coordinate of vertex 2\n"
    "jJ : change camera distance\n";
  glWindowPos2i(10, 480);
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);
  glEnable(GL_FOG);

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, 1.0, 0.25, 25.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -5.0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q': {
      int i;
      glGetIntegerv(GL_FOG_COORD_SRC, &i);
      if (i == GL_FRAGMENT_DEPTH)
        glFogi(GL_FOG_COORDINATE_SOURCE, GL_FOG_COORDINATE);
      else
        glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);
    }
      glutPostRedisplay();
      break;
    case 'w':
      f1 = f1 + 0.25;
      glutPostRedisplay();
      break;
    case 'e':
      f2 = f2 + 0.25;
      glutPostRedisplay();
      break;
    case 'r':
      f3 = f3 + 0.25;
      glutPostRedisplay();
      break;
    case 'W':
      if (f1 > 0.25) {
        f1 = f1 - 0.25;
        glutPostRedisplay();
      }
      break;
    case 'E':
      if (f2 > 0.25) {
        f2 = f2 - 0.25;
        glutPostRedisplay();
      }
      break;
    case 'R':
      if (f3 > 0.25) {
        f3 = f3 - 0.25;
        glutPostRedisplay();
      }
      break;
    case 'j':
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(0.0, 0.0, -0.25);
      glutPostRedisplay();
      break;
    case 'J':
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(0.0, 0.0, 0.25);
      glutPostRedisplay();
      break;
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, depth buffer, and handle input events.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutCreateWindow(argv[0]);
  init();
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
