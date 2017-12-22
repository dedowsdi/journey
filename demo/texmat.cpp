/*
 *  sprite.c
 *  This program demonstrates tex matrix
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/freeglut.h>
#include <stdlib.h>
#include <GL/freeglut_ext.h>
#include "common.h"

#define imgWidth 32
#define imgHeight 32

GLubyte image[imgWidth][imgHeight][4];
GLuint tex;
GLdouble roll = 0;

void makeCheckImage() {
  GLubyte c;
  for (int i = 0; i < imgHeight; ++i) {
    for (int j = 0; j < imgWidth; ++j) {
      c = (((i & 8) == 0) ^ ((j & 8) == 0)) * 255;
      image[i][j][0] = c;
      image[i][j][1] = c;
      image[i][j][2] = c;
      image[i][j][3] = 255;
    }
  }
}

void init(void) {
  glClearColor(0.0, 0.5, 1.0, 0.0);
  glShadeModel(GL_FLAT);

  makeCheckImage();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // create texture
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);

  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imgWidth, imgHeight, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, image);

  /*glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);*/
  glEnable(GL_TEXTURE_2D);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5, 0.5, 0.0);
  glRotatef(roll, 0, 0, 1);
  glTranslatef(-0.5, -0.5, 0.0);

  glBegin(GL_QUADS);
  glTexCoord2f(0, 0);
  glVertex2f(-0.5, -0.5);
  glTexCoord2f(1, 0);
  glVertex2f(0.5, -0.5);
  glTexCoord2f(1, 1);
  glVertex2f(0.5, 0.5);
  glTexCoord2f(0, 1);
  glVertex2f(-0.5, 0.5);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  const GLubyte info[] = " q : rotate along unit z at center";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);
  glEnable(GL_TEXTURE_2D);

  glFlush();

  ZCGEA
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h) {
    gluOrtho2D(-1, 1, -1 * (double)h / w, 1 * (double)h / w);
  } else {
    gluOrtho2D(-1 * (double)w / h, 1 * (double)w / h, -1, 1);
  }
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      roll += 30;
      glutPostRedisplay();
      break;
    case 'Q':
      roll -= 30;
      glutPostRedisplay();
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA);
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
