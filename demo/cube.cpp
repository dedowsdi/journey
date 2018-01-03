/*
 *  cube.c
 *  This program demonstrates a single modeling transformation,
 *  glScalef() and a single viewing transformation, gluLookAt().
 *  A wireframe cube is rendered.
 *
 *  gluPerspective, glFrustum takes only positive near and far plane. 
 *  glOrtho takes both positive and negative near and far plane.
 */
#include <GL/freeglut.h>
#include <stdlib.h>

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glLoadIdentity(); /* clear the matrix */
                    /* viewing transformation  */
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  glScalef(1.0, 2.0, 1.0); /* modeling transformation */
  glutWireCube(1.0);
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  //both near and far plane must be positive. 
  glFrustum(-1.0, 1.0, -1.0, 1.0, 1.5, 20.0);
  glMatrixMode(GL_MODELVIEW); //current matrix
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
