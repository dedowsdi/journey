/*
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <math.h>
#include <GL/freeglut_ext.h>

#define X .525731112119133606
#define Z .850650808352039932
#define random() (double)rand() / INT_MAX
long depth = 1;  // subdivide depth
double aspectRatio = 1.0f;
double vpWidth, vpHeight;

// clang-format off
static GLfloat vdata[12][3] = {
  {-X, 0.0, Z}, {X, 0.0, Z}, {-X, 0.0, -Z}, {X, 0.0, -Z},
  {0.0, Z, X}, {0.0, Z, -X}, {0.0, -Z, X}, {0.0, -Z, -X}, 
  {Z, X, 0.0}, {-Z, X, 0.0}, {Z, -X, 0.0}, {-Z, -X, 0.0}
};

static GLuint tindices[20][3] = {
  {1, 4, 0}, {4, 9, 0}, {4, 5, 9}, {8, 5, 4},
  {1, 8, 4}, {1, 10, 8}, {10, 3, 8}, {8, 3, 5},
  {3, 2, 5}, {3, 7, 2}, {3, 10, 7}, {10, 6, 7},
  {6, 11, 7}, {6, 0, 11}, {6, 1, 0}, {10, 1, 6},
  {11, 0, 9}, {2, 11, 9}, {5, 2, 9}, {11, 2, 7}
};
// clang-format on

void normalize(float v[3]) {
  GLfloat d = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
  if (d == 0.0) {
    printf("zero length vector");
    return;
  }

  v[0] /= d;
  v[1] /= d;
  v[2] /= d;
}

void normcrossprod(float v1[3], float v2[3], float out[3]) {
  out[0] = v1[1] * v2[2] - v1[2] * v2[1];
  out[1] = v1[2] * v2[0] - v1[0] * v2[2];
  out[2] = v1[0] * v2[1] - v1[1] * v2[0];
  normalize(out);
}
//
void drawtriangle(float *v1, float *v2, float *v3) {
  glBegin(GL_TRIANGLES);
  glColor3f(random(), random(), random());
  glNormal3fv(v1);
  glVertex3fv(v1);
  glNormal3fv(v2);
  glVertex3fv(v2);
  glNormal3fv(v3);
  glVertex3fv(v3);
  glEnd();
}

void subdivide(float *v1, float *v2, float *v3, long depth) {
  if (depth == 0) {
    drawtriangle(v1, v2, v3);
    return;
  }

  GLfloat v12[3], v23[3], v31[3];
  GLint i;

  for (i = 0; i < 3; i++) {
    v12[i] = (v1[i] + v2[i]) / 2.0;
    v23[i] = (v2[i] + v3[i]) / 2.0;
    v31[i] = (v3[i] + v1[i]) / 2.0;
  }
  normalize(v12);
  normalize(v23);
  normalize(v31);
  subdivide(v1, v12, v31, depth - 1);
  subdivide(v2, v23, v12, depth - 1);
  subdivide(v3, v31, v23, depth - 1);
  subdivide(v12, v23, v31, depth - 1);
}

void init(void) {
  glClearColor(0.0, 0.5, 1.0, 1.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_CULL_FACE);

  // TODO enable light
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  int i;
  for (i = 0; i < 20; i++) {
    subdivide(&vdata[tindices[i][0]][0], &vdata[tindices[i][1]][0],
      &vdata[tindices[i][2]][0], depth);
  }

  // hud camera view

  char info[100];
  sprintf(info, "depth : %ld", depth);
  glColor3f(0.0f, 0.0f, 0.0f);
  glWindowPos2i(10, 780);  // must be called after glColor
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char *)info);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  aspectRatio = (double)w / h;
  vpWidth = w;
  vpHeight = h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (double)w / h, 0.1, 100);

  glMatrixMode(GL_MODELVIEW);
  gluLookAt(0, 0, 4, 0, 0, 0, 0, 1, 0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

void specialKeyPress(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_UP:
      ++depth;
      glutPostRedisplay();
      break;
    case GLUT_KEY_DOWN:
      depth = fmax(depth - 1, 0);
      glutPostRedisplay();
      break;
  }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(800, 800);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutSpecialFunc(specialKeyPress);
  glutMainLoop();
  return 0;
}
