/*
 *  sprite.c
 *  This program demonstrates point sprite
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <stdlib.h>
#include <GL/freeglut_ext.h>
#include "common.h"

#define imgWidth 32
#define imgHeight 32

GLubyte image[imgWidth][imgHeight][4];
GLuint tex;

// blue bottom
void makeImage() {
  GLubyte c;
  for (int i = 0; i < imgHeight; ++i) {
    for (int j = 0; j < imgWidth; ++j) {
      c = i * ((double)256 / imgHeight);
      image[i][j][0] = c;
      image[i][j][1] = 0;
      image[i][j][2] = 255;
      image[i][j][3] = c;
    }
  }
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);

  makeImage();
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

  // enable point sprite
  glPointSize(30);
  glTexEnvf(GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);  // enable point sprite
  glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
  glEnable(GL_POINT_SPRITE);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glBegin(GL_POINTS);
  glColor3f(1.0f, 1.0f, 1.0f);
  glVertex3f(0.0f, 0.0f, 0.0f);
  glTexCoord2f(0, 0);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  const GLubyte info[] = " q : toggle GL_POINT_COORD_ORIGIN";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);
  glEnable(GL_TEXTURE_2D);

  glFlush();

  ZCGEA
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(-w / 2, w / 2, -h / 2, h / 2);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q': {
      int i;
      glGetIntegerv(GL_POINT_SPRITE_COORD_ORIGIN, &i);
      if (i == GL_LOWER_LEFT) {
        glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
        printf("set origin to GL_UPPER_LEFT\n");
      } else {
        glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
        printf("set origin to GL_LOWER_LEFT\n");
      }
      glutPostRedisplay();
    } break;
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
