/* texwrap.c
 *
 *  You can use GL_TEXTURE_BORDER_COLOR to set border color, or you can put
 *  border in your texture, the later case will make the width and height 2
 *  pixels larger.
 *
 *
 *  repeat : s = fract(s)
 *  mirror repeat : s = int(s)&1 ? 1 - fract(s) : fract(s)
 *  clamp : s = clamp(s, 0, 1)
 *  clam_to_edge :
 *  clamp_to_border : color = border in texture if it exists, otherwise
 *                    gl_texture_border_corlor
 *
 *  in GL_NEAREST:
 *     only clamp_to_border use border if tex is out of 1
 *
 *  in GL_LINEAR edge:
 *    repeat :2*2 array wraps to opposite edge of the texture, border ignored.
 *    mirror repeat : 2*2 array use the same edge of the texture twice.
 *    clamp : 2*2 array use border and texels at the edge.
 *    clamp_to_edge : border ignored.
 *    clamp_to_border : 2*2 array use border and texels at the edge.
 *
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>
#include "common.h"
#include "glEnumString.h"

/*	Create checkerboard texture	*/
#define checkImageWidth 64
#define checkImageHeight 64
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];
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

static GLuint texName;

GLfloat bordeColor[4] = {0.0f, 0.0f, 1.0f, 1.0f};

GLenum wrapMode[] = {GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE,
  GL_MIRRORED_REPEAT, GL_REPEAT};

GLenum filters[] = {
  GL_NEAREST,                 // point
  GL_LINEAR,                  // bilinear
  GL_NEAREST_MIPMAP_NEAREST,  //
  GL_LINEAR_MIPMAP_NEAREST,   //
  GL_NEAREST_MIPMAP_LINEAR,   //
  GL_LINEAR_MIPMAP_LINEAR     // trilinear
};

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
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, bordeColor);

  /*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight,
   * 1,*/
  /*GL_RGBA, GL_UNSIGNED_BYTE, checkImage);*/
  gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, checkImageWidth, checkImageHeight,
    GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texName);

  glBegin(GL_QUADS);
  glTexCoord2f(0.0, 0.0);
  glVertex3f(-1.0, -1.0, 0.0);
  glTexCoord2f(0.0, 2.0);
  glVertex3f(-1.0, 1.0, 0.0);
  glTexCoord2f(2.0, 2.0);
  glVertex3f(1.0, 1.0, 0.0);
  glTexCoord2f(2.0, 0.0);
  glVertex3f(1.0, -1.0, 0.0);

  glEnd();

  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  char info[512];

  GLint modeS, modeT, minFilter, magFilter;
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &modeS);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &modeT);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &minFilter);
  glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &magFilter);
  sprintf(info,
    "q : wrap s : %s \n"
    "w : wrap t : %s \n"
    "e : min filter : %s \n"
    "r : mag filter : %s \n"
    "border color : 0 0 1 1 \n",
    glTexWrapToString(modeS), glTexWrapToString(modeT),
    glTexFilterToString(minFilter), glTexFilterToString(magFilter));

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
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &mode);
      GLint i = 0;
      while (wrapMode[i] != mode) ++i;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode[(++i) % 5]);
      glutPostRedisplay();
    } break;
    case 'w': {
      GLint mode;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &mode);
      GLint i = 0;
      while (wrapMode[i] != mode) ++i;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode[(++i) % 5]);
      glutPostRedisplay();
    } break;
    case 'e': {
      GLint filter;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &filter);
      GLint i = 0;
      while (filters[i] != filter) ++i;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filters[(++i) % 6]);
      glutPostRedisplay();
    } break;
    case 'r': {
      GLint filter;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &filter);
      GLint i = 0;
      while (filters[i] != filter) ++i;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filters[(++i) % 2]);
      glutPostRedisplay();
    } break;
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
