/*
 *  colormatix.c
 *  this program uses the color matrix to exchange the color channels of
 *  an image.
 *
 *    red   -> green
 *    green -> blue
 *    blue  -> red
 *
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include "read_image.cpp"
#include "common.h"

extern GLubyte* read_image(const char*, GLsizei*, GLsizei*);

GLubyte* pixels;
GLsizei width, height;

void init(void) {
  // clang-format off
   GLfloat  m[16] = {
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      1.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 1.0
   };
  // clang-format on

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glMatrixMode(GL_COLOR);
  glLoadMatrixf(m);
  glMatrixMode(GL_MODELVIEW);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(1, 1);
  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
  glFlush();
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

/*  main loop
 *  open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv) {
  pixels = read_image("data/leeds.bin", &width, &height);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadgl();
  init();
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
