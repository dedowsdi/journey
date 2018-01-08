/*
 *  minmax.c
 *  Determine the minimum and maximum values of a group of pixels.
 *  This demonstrates use of the glMinmax() call.
 */
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include "readImage.cpp"

extern GLubyte* readImage(const char*, GLsizei*, GLsizei*);

GLubyte* pixels;
GLsizei width, height;

void init(void) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glMinmax(GL_MINMAX, GL_RGB, GL_FALSE);
  glEnable(GL_MINMAX);
}

void display(void) {
  GLubyte values[6];

  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(1, 1);
  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  glFlush();

  glGetMinmax(GL_MINMAX, GL_TRUE, GL_RGB, GL_UNSIGNED_BYTE, values);
  printf(" Red   : min = %d   max = %d\n", values[0], values[3]);
  printf(" Green : min = %d   max = %d\n", values[1], values[4]);
  printf(" Blue  : min = %d   max = %d\n", values[2], values[5]);
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
  }
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv) {
  pixels = readImage("Data/leeds.bin", &width, &height);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
