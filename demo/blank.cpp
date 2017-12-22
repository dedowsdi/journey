/*
 *  blank.c
 *  Tmp program, always wipe clean after you experiment
 */
#include <GL/freeglut.h>
#include <stdlib.h>

#define drawOneLine(x1, y1, x2, y2) \
  glBegin(GL_LINES);                \
  glVertex2f((x1), (y1));           \
  glVertex2f((x2), (y2));           \
  glEnd();

void init(void) {
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
