/*
 *  convolution.c
 *  Use various 2D convolutions filters to find edges in an image.
 *
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include "readImage.cpp"
#include <GL/freeglut_ext.h>

extern GLubyte* readImage(const char*, GLsizei*, GLsizei*);

GLubyte* pixels;
GLsizei width, height;

// clang-format off

GLfloat horizontal[3][3] = {
  {0, -1, 0}, 
  {0, 1, 0},
  {0, 0, 0}
};

GLfloat vertical[3][3] = {
  {0, 0, 0},
  {-1, 1, 0}, 
  {0, 0, 0}
};

GLfloat laplacian[3][3] = {
  {-0.125, -0.125, -0.125},
  {-0.125, 1.0, -0.125},
  {-0.125, -0.125, -0.125},
};

// clang-format on

void init(void) {
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glClearColor(0.0, 0.0, 0.0, 0.0);

  printf("Using the horizontal filter\n");
  glConvolutionFilter2D(
    GL_CONVOLUTION_2D, GL_LUMINANCE, 3, 3, GL_LUMINANCE, GL_FLOAT, horizontal);
  glEnable(GL_CONVOLUTION_2D);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glRasterPos2i(1, 1);
  glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

  const unsigned char info[] = 
    "q : horizontal filter\n"
    "w : vertical filter\n"
    "e : laplacian filter";
  glWindowPos2i(10, height -20);
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);

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
    case 'q':
      printf("Using a horizontal filter\n");
      glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, 3, 3, GL_LUMINANCE,
        GL_FLOAT, horizontal);
      break;

    case 'w':
      printf("Using the vertical filter\n");
      glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, 3, 3, GL_LUMINANCE,
        GL_FLOAT, vertical);
      break;

    case 'e':
      printf("Using the laplacian filter\n");
      glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, 3, 3, GL_LUMINANCE,
        GL_FLOAT, laplacian);
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
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
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
