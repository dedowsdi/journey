/*
 *  pboimage.c
 *  drawing, copying and zooming pixel data stored in a buffer object
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include "common.h"

#define BUFFER_OFFSET(bytes) ((GLubyte*)NULL + (bytes))

/*	create checkerboard image	*/
#define check_image_width 64
#define check_image_height 64
GLubyte check_image[check_image_height][check_image_width][3];

GLuint pixel_buffer;

void make_check_image(void) {
  int i, j, c;

  for (i = 0; i < check_image_height; i++) {
    for (j = 0; j < check_image_width; j++) {
      c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
      check_image[i][j][0] = (GLubyte)c;
      check_image[i][j][1] = (GLubyte)c;
      check_image[i][j][2] = (GLubyte)c;
    }
  }
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  make_check_image();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenBuffers(1, &pixel_buffer);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixel_buffer);
  glBufferData(GL_PIXEL_UNPACK_BUFFER, 3 * check_image_width * check_image_height,
    check_image, GL_STATIC_DRAW);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(0, 0);
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixel_buffer);
  //draw pixels from current pixel unpack buffer
  glDrawPixels(check_image_width, check_image_height, GL_RGB, GL_UNSIGNED_BYTE,
    BUFFER_OFFSET(0));

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
  glutInitWindowSize(300, 300);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  zxd::loadgl();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
