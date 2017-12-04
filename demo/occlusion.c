/*
 * occlusion.c
 * This program demonstrates occlusion query
 *
 * occlusion test works like bounding box test,
 * First you draw your "bounding box", check if any fragment can be drawn, if
 * nothing drawn, you can throw away your renderable.
 *
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <stdlib.h>
#include <GL/freeglut_ext.h>
#include "common.h"

GLuint queries[2];

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  glGenQueries(2, queries);
  glEnable(GL_DEPTH_TEST);
}

void printQuery(GLuint id) {}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  GLint numSamples, available;

  // disable depth and color write when query
  glDepthMask(GL_FALSE);
  glColorMask(0, 0, 0, 0);

  glBeginQuery(GL_SAMPLES_PASSED, queries[0]);
  glutSolidCube(5);
  glEndQuery(GL_SAMPLES_PASSED);

  glDepthMask(GL_TRUE);
  glColorMask(1, 1, 1, 1);

  glGetQueryObjectiv(queries[0], GL_QUERY_RESULT, &numSamples);
  glGetQueryObjectiv(queries[0], GL_QUERY_RESULT_AVAILABLE, &available);
  printf("numSamples : %d, available : %s \n", numSamples,
    available ? "true" : "false");

  // if querry successed draw our renderable
  if (available) {
    glColor3f(0, 0, 1);
    glutSolidCube(4);
  }

  // try to draw a cube inside the 1st cube
  glDepthMask(GL_FALSE);
  glColorMask(0, 0, 0, 0);

  glBeginQuery(GL_SAMPLES_PASSED, queries[1]);
  glutSolidCube(3);
  glEndQuery(GL_SAMPLES_PASSED);

  glDepthMask(GL_TRUE);
  glColorMask(1, 1, 1, 1);

  glGetQueryObjectiv(queries[1], GL_QUERY_RESULT, &numSamples);
  glGetQueryObjectiv(queries[1], GL_QUERY_RESULT_AVAILABLE, &available);
  printf("numSamples : %d, available : %s \n", numSamples,
    available ? "true" : "false");

  // if querry successed draw our renderable
  if (available) {
    glColor3f(0, 0, 1);
    glutSolidCube(2);
  }

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  const GLubyte info[] = "qQ : toggle multisample \n";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);

  /*GLint box[4];*/
  /*glGetIntegerv(GL_SCISSOR_BOX, box);*/
  /*printf("%d, %d, %d, %d\n", box[0],box[1],box[2],box[3]);*/

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    glOrtho(-15, 15, -15, 15 * (GLfloat)h / (GLfloat)w, -100, 100);
  else
    glOrtho(-15, 15 * (GLfloat)w / (GLfloat)h, -15, 15, -100, 100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
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
