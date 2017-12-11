/*  texture3d.c
 *  This program demonstrates using a three-dimensional texture.
 *  It creates a 3D texture and then renders two rectangles
 *  with different texture coordinates to obtain different
 *  "slices" of the 3D texture.
 *
 *  You don't need to use the same r for 3d tex coord of primitives.
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>

#ifdef GL_VERSION_1_2
GLuint x0 = 0, y0 = 0, width = 16, height = 16, depth = 16;

static GLubyte image[16][16][16][3];
static GLuint texName;

/*  Create a 16x16x16x3 array with different color values in
 *  each array element [r, g, b].  Values range from 0 to 255.
 */

void makeImage(void) {
  int s, t, r;

  for (s = 0; s < 16; s++)
    for (t = 0; t < 16; t++)
      for (r = 0; r < 16; r++) {
        image[r][t][s][0] = (GLubyte)(s * 17);
        image[r][t][s][1] = (GLubyte)(t * 17);
        image[r][t][s][2] = (GLubyte)(r * 17);
      }
}

void genTex() {
  glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB,
    GL_UNSIGNED_BYTE, image);
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  makeImage();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
  glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, height);

  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_3D, texName);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glEnable(GL_TEXTURE_3D);

  genTex();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glBegin(GL_QUADS);
  glColor3f(1.0f, 1.0f, 1.0f);
  // you don't need to specify the same r for quad
  glTexCoord3f(0.0, 0.0, 0.0);
  glVertex3f(-2.25, -1.0, 0.0);
  glTexCoord3f(0.0, 1.0, 0.0);
  glVertex3f(-2.25, 1.0, 0.0);
  glTexCoord3f(1.0, 1.0, 1.0);
  glVertex3f(-0.25, 1.0, 0.0);
  glTexCoord3f(1.0, 0.0, 1.0);
  glVertex3f(-0.25, -1.0, 0.0);

  glTexCoord3f(0.0, 0.0, 1.0);
  glVertex3f(0.25, -1.0, 0.0);
  glTexCoord3f(0.0, 1.0, 1.0);
  glVertex3f(0.25, 1.0, 0.0);
  glTexCoord3f(1.0, 1.0, 0.0);
  glVertex3f(2.25, 1.0, 0.0);
  glTexCoord3f(1.0, 0.0, 0.0);
  glVertex3f(2.25, -1.0, 0.0);
  glEnd();

  glDisable(GL_TEXTURE_3D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  char info[512];

  sprintf(info,
    "qQ : pixel x0 : %u\n"
    "wW : pixel y0 : %u\n"
    "eE : tex width : %u\n"
    "rR : tex height : %u\n"
    "uR : skip tex images : %u\n",
    x0, y0, width, height, 16 - depth);

  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
  glEnable(GL_TEXTURE_3D);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -4.0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      if (x0 < 16) {
        x0 += 1;
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, x0);
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'Q':
      if (x0 > 0) {
        x0 -= 1;
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, x0);
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'w':
      if (y0 < 16) {
        y0 += 1;
        glPixelStorei(GL_UNPACK_SKIP_ROWS, y0);
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'W':
      if (y0 > 0) {
        y0 -= 1;
        glPixelStorei(GL_UNPACK_SKIP_ROWS, y0);
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'e':
      if (width < 16) {
        width += 1;
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'E':
      if (width > 0) {
        width -= 1;
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'r':
      if (height < 12) {
        height += 1;
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'R':
      if (height > 0) {
        height -= 1;
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'u':
      //skip image
      if (depth > 0) {
        --depth;
        glPixelStorei(GL_UNPACK_SKIP_IMAGES, 16 - depth);
        genTex();
        glutPostRedisplay();
      }
      break;
    case 'U':
      if (depth < 16) {
        ++depth;
        glPixelStorei(GL_UNPACK_SKIP_IMAGES, 16 - depth);
        genTex();
        glutPostRedisplay();
      }
      break;
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
  fprintf(
    stderr, "If your implementation of OpenGL has the right extensions,\n");
  fprintf(stderr, "you may be able to modify this program to make it run.\n");
  return 0;
}
#endif
