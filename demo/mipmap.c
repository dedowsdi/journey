/*  mipmap.c
 *  This program demonstrates using mipmaps for texture maps.
 *  To overtly show the effect of mipmaps, each mipmap reduction
 *  level has a solidly colored, contrasting texture image.
 *  Thus, the quadrilateral which is drawn is drawn with several
 *  different colors.
 *
 *  If you are useing mipmap, you must create texture from base_level until
 *  max_level(or 1*1), otherwise you are screwed!!!.
 *
 *  LOD:
 *    ρ = max(texel_width/pixel_width, texel_height/pixel_height)
 *    γ = log2(2, ρ) + lod_bias
 *
 *    just thought lod is float type level.
 *
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>
#include "common.h"

GLubyte mipmapImage32[32][32][4];
GLubyte mipmapImage16[16][16][4];
GLubyte mipmapImage8[8][8][4];
GLubyte mipmapImage4[4][4][4];
GLubyte mipmapImage2[2][2][4];
GLubyte mipmapImage1[1][1][4];

GLuint baseLevel = 0;
GLuint maxLevel = 5;
GLdouble minLod = 0;
GLdouble maxLod = 5;
GLdouble lodBias = 0;

#ifdef GL_VERSION_1_1
static GLuint texName;
#endif

void makeImages(void) {
  int i, j;

  for (i = 0; i < 32; i++) {
    for (j = 0; j < 32; j++) {
      mipmapImage32[i][j][0] = 255;
      mipmapImage32[i][j][1] = 255;
      mipmapImage32[i][j][2] = 0;
      mipmapImage32[i][j][3] = 255;
    }
  }
  for (i = 0; i < 16; i++) {
    for (j = 0; j < 16; j++) {
      mipmapImage16[i][j][0] = 255;
      mipmapImage16[i][j][1] = 0;
      mipmapImage16[i][j][2] = 255;
      mipmapImage16[i][j][3] = 255;
    }
  }
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      mipmapImage8[i][j][0] = 255;
      mipmapImage8[i][j][1] = 0;
      mipmapImage8[i][j][2] = 0;
      mipmapImage8[i][j][3] = 255;
    }
  }
  for (i = 0; i < 4; i++) {
    for (j = 0; j < 4; j++) {
      mipmapImage4[i][j][0] = 0;
      mipmapImage4[i][j][1] = 255;
      mipmapImage4[i][j][2] = 0;
      mipmapImage4[i][j][3] = 255;
    }
  }
  for (i = 0; i < 2; i++) {
    for (j = 0; j < 2; j++) {
      mipmapImage2[i][j][0] = 0;
      mipmapImage2[i][j][1] = 0;
      mipmapImage2[i][j][2] = 255;
      mipmapImage2[i][j][3] = 255;
    }
  }
  mipmapImage1[0][0][0] = 255;
  mipmapImage1[0][0][1] = 255;
  mipmapImage1[0][0][2] = 255;
  mipmapImage1[0][0][3] = 255;
}

void init(void) {
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);

  glTranslatef(0.0, 0.0, -3.6);
  makeImages();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef GL_VERSION_1_1
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);
#endif
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(
    GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    mipmapImage32);
  glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    mipmapImage16);
  glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    mipmapImage8);
  glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    mipmapImage4);
  glTexImage2D(GL_TEXTURE_2D, 4, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    mipmapImage2);
  glTexImage2D(GL_TEXTURE_2D, 5, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
    mipmapImage1);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glEnable(GL_TEXTURE_2D);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef GL_VERSION_1_1
  glBindTexture(GL_TEXTURE_2D, texName);
#endif
  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-2.0, -1.0, 0.0);
  glTexCoord2f(0.0, 8.0);
  glVertex3f(-2.0, 1.0, 0.0);
  glTexCoord2f(8.0, 8.0);
  glVertex3f(2000.0, 1.0, -6000.0);
  glTexCoord2f(8.0, 0.0);
  glVertex3f(2000.0, -1.0, -6000.0);
  glEnd();

  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(180, 620);

  char info[512];

  sprintf(info,
    "mipmap works on pixels, \n"
    "face size in 3d space doesn't matter. \n"
    "a single face might use multiple levels of texture\n"
    "qQ : mip base level : %u\n"
    "wW : mip max level : %u\n"
    "eE : mip min lod : %3.2f\n"
    "rR : mip max lod : %3.2f\n"
    "uU : lod bias : %3.2f\n",
    baseLevel, maxLevel, minLod, maxLod, lodBias);

  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
  glEnable(GL_TEXTURE_2D);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 30000.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      ++baseLevel;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, baseLevel);
      glutPostRedisplay();
      break;
    case 'Q':
      if (baseLevel > 0) {
        --baseLevel;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, baseLevel);
        glutPostRedisplay();
      }
      break;
    case 'w':
      ++maxLevel;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel);
      glutPostRedisplay();
      break;
    case 'W':
      if (maxLevel > 0) {
        --maxLevel;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel);
        glutPostRedisplay();
      }
      break;
    case 'e':
      minLod += 0.1;
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, minLod);
      glutPostRedisplay();
      break;
    case 'E':
      if (minLod > 0) {
        minLod -= 0.1;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, minLod);
        glutPostRedisplay();
      }
      break;
    case 'r':
      maxLod += 0.1;
      glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, maxLod);
      glutPostRedisplay();
      break;
    case 'R':
      if (maxLod >= 0.1) {
        maxLod -= 0.1;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, maxLod);
        glutPostRedisplay();
      }
      break;
    case 'u':
      lodBias += 0.1;
      glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, lodBias);
      glutPostRedisplay();
      ZCGEA;
      break;
    case 'U':
      lodBias -= 0.1;
      glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, lodBias);
      glutPostRedisplay();
      break;
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(640, 640);
  glutInitWindowPosition(50, 50);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
