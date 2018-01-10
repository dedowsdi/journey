/*  multitex.c
 *
 *  texture unit:
 *   	Texture image
 *	 	Filtering parameters
 *	 	Environment application
 *	 	Texture matrix stack
 *	 	Automatic texture-coordinate generation
 *	 	Vertex-array specification (if needed, through glClintActivateTexture)
 *
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#ifdef GL_VERSION_1_2

static GLubyte texels0[32][32][4];
static GLubyte texels1[16][16][4];
GLuint texNames[2];

void makeImages(void) {
  int i, j;

  for (i = 0; i < 32; i++) {
    for (j = 0; j < 32; j++) {
      texels0[i][j][0] = (GLubyte)64;
      texels0[i][j][1] = (GLubyte)j * 5;
      texels0[i][j][2] = (GLubyte)(i * j) / 255;
      texels0[i][j][3] = (GLubyte)255;
    }
  }

  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      texels1[i][j][0] = (GLubyte)255;
      texels1[i][j][1] = (GLubyte)i * 10;
      texels1[i][j][2] = (GLubyte)j * 10;
      texels1[i][j][3] = (GLubyte)255;
    }
  }
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  makeImages();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glEnable(GL_TEXTURE_2D);
  glGenTextures(2, texNames);
  glBindTexture(GL_TEXTURE_2D, texNames[0]);
  ZCGE(glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels0));

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glBindTexture(GL_TEXTURE_2D, texNames[1]);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels1);
  gluBuild2DMipmaps(
    GL_TEXTURE_2D, GL_RGBA, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, texels1);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  /*  Use the two texture objects to define two texture units
   *  for use in multitexturing  */
  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texNames[0]);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5f, 0.5f, 0.0f);
  glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
  glTranslatef(-0.5f, -0.5f, 0.0f);
  glMatrixMode(GL_MODELVIEW);

  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texNames[1]);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBegin(GL_TRIANGLES);
  glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
  glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
  glVertex2f(0.0, 0.0);
  glMultiTexCoord2f(GL_TEXTURE0, 0.5, 1.0);
  glMultiTexCoord2f(GL_TEXTURE1, 0.5, 0.0);
  glVertex2f(50.0, 100.0);
  glMultiTexCoord2f(GL_TEXTURE0, 1.0, 0.0);
  glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
  glVertex2f(100.0, 0.0);
  glEnd();
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    gluOrtho2D(0.0, 100.0, 0.0, 100.0 * (GLdouble)h / (GLdouble)w);
  else
    gluOrtho2D(0.0, 100.0 * (GLdouble)w / (GLdouble)h, 0.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
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
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
  glutInitWindowSize(250, 250);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadGL();
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
#else
int main(int argc, char** argv) {
  fprintf(stderr,
    "This program demonstrates a feature which is not in OpenGL Version 1.0 or "
    "1.1.\n");
  fprintf(stderr,
    "If your implementation of OpenGL Version 1.0 has the right extensions,\n");
  fprintf(stderr, "you may be able to modify this program to make it run.\n");
  return 0;
}
#endif
