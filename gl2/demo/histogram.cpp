/*
 *  histogram.c
 *  Compute the histogram of the image.  This program illustrates the
 *  use of the glHistogram() function.
 */

#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include "readImage.cpp"
#include "common.h"

#define HISTOGRAM_SIZE 256 /* Must be a power of 2 */

extern GLubyte* readImage(const char*, GLsizei*, GLsizei*);

GLubyte* pixels;
GLsizei width, height;

void init(void) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glHistogram(GL_HISTOGRAM, HISTOGRAM_SIZE, GL_RGB, GL_FALSE);
  glEnable(GL_HISTOGRAM);
}

void display(void) {
  int i;
  GLushort values[HISTOGRAM_SIZE][3];

  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(1, 1);
  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  //get histogram, reset inner table. You can also use glResetHistogram to reset
  //iner table
  glGetHistogram(GL_HISTOGRAM, GL_TRUE, GL_RGB, GL_UNSIGNED_SHORT, values);

  /* Plot histogram */

  glBegin(GL_LINE_STRIP);
  glColor3f(1.0, 0.0, 0.0);
  for (i = 0; i < HISTOGRAM_SIZE; i++) glVertex2s(i, values[i][0]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glColor3f(0.0, 1.0, 0.0);
  for (i = 0; i < HISTOGRAM_SIZE; i++) glVertex2s(i, values[i][1]);
  glEnd();

  glBegin(GL_LINE_STRIP);
  glColor3f(0.0, 0.0, 1.0);
  for (i = 0; i < HISTOGRAM_SIZE; i++) glVertex2s(i, values[i][2]);
  glEnd();
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, 256, 0, 10000, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
  static GLboolean sink = GL_FALSE;

  switch (key) {
    case 's':
      sink = !sink;
      glHistogram(GL_HISTOGRAM, HISTOGRAM_SIZE, GL_RGB, sink);
      break;

    case 27:
      exit(0);
  }
  glutPostRedisplay();
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
  loadGL();
  init();
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
