/*
 *  pointp.c
 *  This program demonstrates point parameters and their effect
 *  on point primitives.
 *  250 points are randomly generated within a 10 by 10 by 40
 *  region, centered at the origin.  In some modes (including the
 *  default), points that are closer to the viewer will appear larger.
 *
 *
 *  You can not call glPointSize betwen glBegin and glEnd, you need to use point
 *  parameter to do the job. It scale and fade point based on attenuation.
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>
#include "common.h"

#ifdef GL_VERSION_1_4

static GLfloat psize = 7.0;
static GLfloat pmax[1];
static GLfloat constant[3] = {1.0, 0.0, 0.0};
static GLfloat linear[3] = {0.0, 0.12, 0.0};
static GLfloat quadratic[3] = {0.0, 0.0, 0.01};

void init(void) {
  int i;

  srand(12345);

  // compile list for 250 points in random location
  glNewList(1, GL_COMPILE);
  glBegin(GL_POINTS);
  for (i = 0; i < 250; i++) {
    glColor3f(
      1.0, ((rand() / (float)RAND_MAX) * 0.5) + 0.5, rand() / (float)RAND_MAX);
    /*  randomly generated vertices:
        -5 < x < 5;  -5 < y < 5;  -5 < z < -45  */
    glVertex3f(((rand() / (float)RAND_MAX) * 10.0) - 5.0,
      ((rand() / (float)RAND_MAX) * 10.0) - 5.0,
      ((rand() / (float)RAND_MAX) * 40.0) - 45.0);
  }
  glEnd();
  glEndList();

  glEnable(GL_DEPTH_TEST);
  glPointSize(psize);
  // enable point smooth to get round point
  glEnable(GL_POINT_SMOOTH);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glGetFloatv(GL_POINT_SIZE_MAX, pmax);

  glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, linear);
  glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, 2.0);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCallList(1);

  const unsigned char info[] =
    "q : set distance attenuation to pure constant \n"
    "w : set distance attenuation to pure linear \n"
    "e : set distance attenuation to pure quadratic \n"
    "rR : change point size\n"
    "u : toggle multisampling, cause small point to fade\n"
    "jJ : change camera distance\n";


  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(35.0, 1.0, 0.25, 200.0);
  glMatrixMode(GL_MODELVIEW);
  glTranslatef(0.0, 0.0, -10.0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'j':
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(0.0, 0.0, 0.5);
      glutPostRedisplay();
      break;
    case 'J':
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(0.0, 0.0, -0.5);
      glutPostRedisplay();
      break;
    case 'q':
      glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, constant);
      glutPostRedisplay();
      break;
    case 'w':
      glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, linear);
      glutPostRedisplay();
      break;
    case 'e':
      glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION, quadratic);
      glutPostRedisplay();
      break;
    case 'r':
      if (psize < (pmax[0] + 1.0)) psize = psize + 1.0;
      glPointSize(psize);
      glutPostRedisplay();
      break;
    case 'R':
      if (psize >= 2.0) psize = psize - 1.0;
      glPointSize(psize);
      glutPostRedisplay();
      break;
    case 'u':
      if (glIsEnabled(GL_MULTISAMPLE)) 
        glDisable(GL_MULTISAMPLE);
      else
        glEnable(GL_MULTISAMPLE);
      glutPostRedisplay();
      break;
    case 27:
      exit(0);
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadGL();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
#else
int main(int argc, char** argv) {
  fprintf(stderr,
    "This program demonstrates a feature which is not in OpenGL Version "
    "1.4.\n");
  fprintf(stderr,
    "If your implementation of OpenGL Version 1.4 has the right extensions,\n");
  fprintf(stderr, "you may be able to modify this program to make it run.\n");
  return 0;
}
#endif
