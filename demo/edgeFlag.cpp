/*
 *  edgeFlag.c
 *  This program demonstrates usage of glEdgeFlag
 *  glEdgeFlag don't work for strip type primitives.
 *  glEdgeFlag only works under GL_LINE or GL_POINT polygon mode.
 */
#include <GL/glut.h>
#include <stdlib.h>
#include <GL/freeglut_ext.h>

GLenum polygonMode = GL_LINE;

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void display(void) {

  glClear(GL_COLOR_BUFFER_BIT);

  glPolygonMode(GL_FRONT_AND_BACK,
    polygonMode);  // don't put this between glBegin and glEnd

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

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'p':
      polygonMode = GL_POINT + (polygonMode + 1 - GL_POINT) % 3;
      glutPostRedisplay();
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(300, 300);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
