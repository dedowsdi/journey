/*
 *  double.c
 *  This is a simple double buffered program.
 *  Pressing the left mouse button rotates the rectangle.
 *  Pressing the middle mouse button stops the rotation.
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include "common.h"

static GLfloat spin = 0.0;

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glPushMatrix();
  glRotatef(spin, 0.0, 0.0, 1.0);
  glColor3f(1.0, 1.0, 1.0);
  glRectf(-25.0, -25.0, 25.0, 25.0);
  glPopMatrix();

  //swap buffer if you are using double buffer
  glutSwapBuffers();
}

void spinDisplay(void) {
  spin = spin + 2.0;
  if (spin > 360.0) spin = spin - 360.0;
  glutPostRedisplay();  // notify main loop to call display in next loop.
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-50.0, 50.0, -50.0, 50.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouse(int button, int state, int x, int y) {
  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) glutIdleFunc(spinDisplay);
      break;
    case GLUT_MIDDLE_BUTTON:
    case GLUT_RIGHT_BUTTON:
      if (state == GLUT_DOWN) glutIdleFunc(NULL);
      break;
    default:
      break;
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

/*
 *  Request double buffer display mode.
 *  Register mouse input callback functions
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(250, 250);
  glutInitWindowPosition(100, 100);
  glutInitContextFlags(GLUT_DEBUG);
  glutCreateWindow(argv[0]);
  init();
  loadGL();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0; /* ANSI C requires main to return int. */
}
