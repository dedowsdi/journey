/*
 * fragtest.c
 * This program demonstrates scissor and alpha test
 *
 * Don't change alpha or scissor statie directly in keyboard function, it won't
 * work.
 *
 */
#include "glad/glad.h"
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <GL/freeglut_ext.h>
#include "common.h"

GLboolean enableScissor = GL_FALSE, enableAlpha = GL_FALSE;

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
}

void triangle(void) {
  glBegin(GL_TRIANGLES);
  glColor4f(1.0, 0.0, 0.0, 1.0f);
  glVertex2f(5.0, 5.0);
  glColor4f(0.0, 1.0, 0.0, 0.0f);
  glVertex2f(25.0, 5.0);
  glColor4f(0.0, 0.0, 1.0, 1.0f);
  glVertex2f(5.0, 25.0);
  glEnd();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  if (enableScissor) {
    glEnable(GL_SCISSOR_TEST);
    glScissor(200, 200, 100, 100);
  } else {
    glDisable(GL_SCISSOR_TEST);
  }

  if (enableAlpha) {
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
  } else {
    glDisable(GL_ALPHA_TEST);
  }

  triangle();

  glDisable(GL_SCISSOR_TEST);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  const GLubyte info[] =
    "qQ : toggle scissor test \n"
    "wW : toggle alpha test \n";
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
    gluOrtho2D(0.0, 30.0, 0.0, 30.0 * (GLfloat)h / (GLfloat)w);
  else
    gluOrtho2D(0.0, 30.0 * (GLfloat)w / (GLfloat)h, 0.0, 30.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;

    case 'q':
    case 'Q': {
      enableScissor = !enableScissor;

      glutPostRedisplay();
    } break;

    case 'w':
    case 'W':
      enableAlpha = !enableAlpha;
      glutPostRedisplay();
      break;

    case 'f':
    case 'F':
      glutPostRedisplay();
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
