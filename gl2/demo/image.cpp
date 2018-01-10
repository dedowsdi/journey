/*  image.c
 *  This program demonstrates drawing pixels and shows the effect
 *  of glDrawPixels(), glCopyPixels(), and glPixelZoom().
 *  Interaction: moving the mouse while pressing the mouse button
 *  will copy the image in the lower-left corner of the window
 *  to the mouse position, using the current pixel zoom factors.
 *  There is no attempt to prevent you from drawing over the original
 *  image.  If you press the 'r' key, the original image and zoom
 *  factors are reset.  If you press the 'z' or 'Z' keys, you change
 *  the zoom factors.
 *  
 *  If you only want to draw subrect of a image, use glPixlelStore with
 *  GL_UNPACK_SKIP_ROWS, GL_UNPACK_SKIP_PIXELS to set up left bottom origin, use
 *  glPIxelStore with GL_UNPACK_ROW_LENGTH to set subrect width, set height in
 *  glDrawPixel, ROW_LENGTH should be the same as the width you use in
 *  glDrawPixel
 *
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>
#include "common.h"

/*	Create checkerboard image	*/
#define checkImageWidth 64
#define checkImageHeight 64
GLubyte checkImage[checkImageHeight][checkImageWidth][3];

static GLdouble zoomFactor = 1.0;
static GLint height;

void makeCheckImage(void) {
  int i, j, c;

  for (i = 0; i < checkImageHeight; i++) {
    for (j = 0; j < checkImageWidth; j++) {
      c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
      checkImage[i][j][0] = (GLubyte)c;
      checkImage[i][j][1] = (GLubyte)c;
      checkImage[i][j][2] = (GLubyte)c;
    }
  }
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  makeCheckImage();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(0, 0);
  glDrawPixels(
    checkImageWidth, checkImageHeight, GL_RGB, GL_UNSIGNED_BYTE, checkImage);
  glWindowPos2i(10, 230);
  const unsigned char info[] =
    "qQ : zoom factor\n"
    "r : reset zoom factor to 1\n"
    "drag : copy pixels";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  height = (GLint)h;
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void motion(int x, int y) {
  static GLint screeny;

  screeny = height - (GLint)y - 1;
  glRasterPos2i(x, screeny);
  glPixelZoom(zoomFactor, zoomFactor);
  glCopyPixels(0, 0, checkImageWidth, checkImageHeight, GL_COLOR);
  glPixelZoom(1.0, 1.0);
  glFlush();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'r':
    case 'R':
      zoomFactor = 1.0;
      glutPostRedisplay();
      printf("zoomFactor reset to 1.0\n");
      break;
    case 'q':
      zoomFactor += 0.5;
      if (zoomFactor >= 3.0) zoomFactor = 3.0;
      printf("zoomFactor is now %4.1f\n", zoomFactor);
      break;
    case 'Q':
      zoomFactor -= 0.5;
      if (zoomFactor <= 0.5) zoomFactor = 0.5;
      printf("zoomFactor is now %4.1f\n", zoomFactor);
      break;
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(250, 250);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadGL();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMotionFunc(motion);
  glutMainLoop();
  return 0;
}
