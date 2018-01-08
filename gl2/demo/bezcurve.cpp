/*  bezcurve.c
 *  This program uses evaluators to draw a Bezier curve.
 *
 *  You can assign any float to u1 and u2, 
 *  the real u is calculated by (u - u1) / (u2 - u1)
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include "common.h"

GLfloat ctrlpoints[4][3] = {
  {-4.0, -4.0, 0.0}, {-2.0, 4.0, 0.0}, {2.0, -4.0, 0.0}, {4.0, 4.0, 0.0}};

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  
  glMap1f(GL_MAP1_VERTEX_3, 0.0, 1.0, 3, 4, &ctrlpoints[0][0]);
  glEnable(GL_MAP1_VERTEX_3);

  //30 even steps from 0 to 1
  glMapGrid1f(30, 0, 1);
}

void display(void) {
  int i;

  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);

  //from point to 0 to point 30(include)
  glEvalMesh1(GL_LINE, 0, 30);
  /*glBegin(GL_LINE_STRIP);*/
  /*for (i = 0; i <= 30; i++) glEvalCoord1f((GLfloat)i / 30.0);*/
  /*glEnd();*/

  /*The following code displays the control points as dots. */
  glPointSize(5.0);
  glColor3f(1.0, 1.0, 0.0);
  glBegin(GL_POINTS);
  for (i = 0; i < 4; i++) glVertex3fv(&ctrlpoints[i][0]);
  glEnd();
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    glOrtho(-5.0, 5.0, -5.0 * (GLfloat)h / (GLfloat)w,
      5.0 * (GLfloat)h / (GLfloat)w, -5.0, 5.0);
  else
    glOrtho(-5.0 * (GLfloat)w / (GLfloat)h, 5.0 * (GLfloat)w / (GLfloat)h, -5.0,
      5.0, -5.0, 5.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
