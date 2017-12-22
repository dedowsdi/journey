/*
 *  stencil.c
 *  This program demonstrates use of the stencil buffer for
 *  masking nonrectangular regions.
 *  Whenever the window is redrawn, a value of 1 is drawn
 *  into a diamond-shaped region in the stencil buffer.
 *  Elsewhere in the stencil buffer, the value is 0.
 *  Then a blue sphere is drawn where the stencil value is 1,
 *  and yellow torii are drawn where the stencil value is not 1.
 *
 *  glStencilFunc setup func, ref and mask
 *  func takes arguments from ref & mask and stencil & mask
 *  glStencilOp decide what operation should be done when
 *     stencil fail
 *     stecil pass, but depth fail
 *     stencil pass, depth pass
 *
 */
#include <GL/freeglut.h>
#include <stdlib.h>
#include "common.h"

#define YELLOWMAT 1
#define BLUEMAT 2

void init(void) {
  GLfloat yellow_diffuse[] = {0.7, 0.7, 0.0, 1.0};
  GLfloat yellow_specular[] = {1.0, 1.0, 1.0, 1.0};

  GLfloat blue_diffuse[] = {0.1, 0.1, 0.7, 1.0};
  GLfloat blue_specular[] = {0.1, 1.0, 1.0, 1.0};

  GLfloat position_one[] = {1.0, 1.0, 1.0, 0.0};

  glNewList(YELLOWMAT, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, yellow_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
  glEndList();

  glNewList(BLUEMAT, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, 45.0);
  glEndList();

  glLightfv(GL_LIGHT0, GL_POSITION, position_one);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

  // clear stencil to 0
  glClearStencil(0x0);
  glEnable(GL_STENCIL_TEST);
}

/* Draw a sphere in a diamond-shaped section in the
 * middle of a window with 2 torii.
 */
void display(void) {
  // don't clear stencil, it's prepared in reshape
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  /* draw blue sphere where the stencil is 1 */
  glStencilFunc(GL_EQUAL, 0x1, 0x1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);  // don't change stencil
  glCallList(BLUEMAT);
  glutSolidSphere(0.8, 15, 15);

  /* draw the tori where the stencil is not 1 */
  glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
  glPushMatrix();
  glRotatef(45.0, 0.0, 0.0, 1.0);
  glRotatef(45.0, 0.0, 1.0, 0.0);
  glCallList(YELLOWMAT);
  glutSolidTorus(0.275, 0.85, 15, 15);
  glPushMatrix();
  glRotatef(90.0, 1.0, 0.0, 0.0);
  glutSolidTorus(0.275, 0.85, 15, 15);
  glPopMatrix();
  glPopMatrix();

  glFlush();
}

/*  Whenever the window is reshaped, redefine the
 *  coordinate system and redraw the stencil area.
 */
void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    gluOrtho2D(
      -3.0, 3.0, -3.0 * (GLfloat)h / (GLfloat)w, 3.0 * (GLfloat)h / (GLfloat)w);
  else
    gluOrtho2D(
      -3.0 * (GLfloat)w / (GLfloat)h, 3.0 * (GLfloat)w / (GLfloat)h, -3.0, 3.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  /* create a diamond shaped stencil area */
  glClear(GL_STENCIL_BUFFER_BIT);  // clear all stencil to 0
  glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glBegin(GL_QUADS);
  glVertex2f(-1.0, 0.0);
  glVertex2f(0.0, 1.0);
  glVertex2f(1.0, 0.0);
  glVertex2f(0.0, -1.0);
  glEnd();

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 3.0, 7.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -5.0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

/* Main Loop
 * Be certain to request stencil bits.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
