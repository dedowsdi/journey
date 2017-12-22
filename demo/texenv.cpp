/*  texenv.c
 *
 *  demo of GL_TEXTURE_ENV_MODE
 *
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>
#include "common.h"
#include "glEnumString.h"

/*	Create checkerboard texture	*/
#define checkImageWidth 64
#define checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint texName;

GLenum texFunctions[] = {
  GL_REPLACE, GL_MODULATE, GL_DECAL, GL_BLEND, GL_ADD, GL_COMBINE};

void makeCheckImage(void) {
  int i, j, c;

  for (i = 0; i < checkImageHeight; i++) {
    for (j = 0; j < checkImageWidth; j++) {
      c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
      checkImage[i][j][0] = (GLubyte)c;
      checkImage[i][j][1] = (GLubyte)c;
      checkImage[i][j][2] = (GLubyte)c;
      checkImage[i][j][3] = (GLubyte)255;
    }
  }
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  makeCheckImage();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texName);

  glColor3f(1.0f, 0.0f, 0.0f);
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-1.0, -1.0, 0.0);
  glTexCoord2f(0.0, 1.0);
  glVertex3f(-1.0, 1.0, 0.0);
  glTexCoord2f(1.0, 1.0);
  glVertex3f(1.0, 1.0, 0.0);
  glTexCoord2f(1.0, 0.0);
  glVertex3f(1.0, -1.0, 0.0);

  glEnd();

  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 1.0f, 1.0f);

  glWindowPos2i(10, 480);
  char info[512];

  GLint mode;
  glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &mode);
  sprintf(info,
    "red quad with a chess texture\n"
    "q : rotate texture_env_mode : %s \n",
    glTexEnvModeToString(mode));

  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
  glEnable(GL_TEXTURE_2D);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    glOrtho(-2.25, 2.25, -2.25 * h / w, 2.25 * h / w, -10.0, 10.0);
  else
    glOrtho(-2.25 * w / h, 2.25 * w / h, -2.25, 2.25, -10.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q': {
      GLint mode;
      glGetTexEnviv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &mode);
      GLint i = 0;
      while (texFunctions[i] != mode) ++i;
      glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, texFunctions[(++i) % 6]);
      glutPostRedisplay();
      break;
    }
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
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
