/*
 *  mvarray.c
 *  This program demonstrates multiple vertex arrays,
 *  specifically the OpenGL routine glMultiDrawElements().
 *
 *  You can render multiple sets of primitives in a single call:
 *    glMultiDrawArrays
 *    glMultiDrawElements
 */

//macro to introduce glMultiDraw*
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef GL_VERSION_1_3

void setupPointer(void) {
  // clang-format off
  static GLint vertices[] = {
    25, 25, 75,
    75, 100, 125,
    150, 75, 200,
    175, 250, 150,
    300, 125, 100,
    200, 150, 250,
    200, 225, 250,
    300, 300, 250
  };
  // clang-format on

  glEnableClientState(GL_VERTEX_ARRAY);
  glVertexPointer(2, GL_INT, 0, vertices);
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  setupPointer();
}

void display(void) {
  //use the same vertices to create two indices
  static GLubyte oneIndices[] = {0, 1, 2, 3, 4, 5, 6};
  static GLubyte twoIndices[] = {1, 7, 8, 9, 10, 11};
  static GLsizei count[] = {7, 6};
  static const GLvoid* indices[2] = {oneIndices, twoIndices};

  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glMultiDrawElements(GL_LINE_STRIP, count, GL_UNSIGNED_BYTE, indices, 2);
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
  glutInitWindowSize(350, 350);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
#else
int main(int argc, char** argv) {
  fprintf(stderr,
    "This program demonstrates a feature which is not in OpenGL Version "
    "1.0.\n");
  fprintf(stderr,
    "If your implementation of OpenGL Version 1.0 has the right extensions,\n");
  fprintf(stderr, "you may be able to modify this program to make it run.\n");
  return 0;
}
#endif
