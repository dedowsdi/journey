/*
 *  unproject.c
 *  When the left mouse button is pressed, this program
 *  reads the mouse position and determines two 3D points
 *  from which it was transformed.  Very little is displayed.
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glFlush();
}

/* Change these values for a different transformation  */
void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

//x y in left hand frame, orgin at left top
void mouse(int button, int state, int x, int y) {
  GLint viewport[4];
  GLdouble mvmatrix[16], projmatrix[16];
  GLint realy;         /*  OpenGL y coordinate position  */
  GLdouble wx, wy, wz; /*  returned world x, y, z coords  */

  switch (button) {
    case GLUT_LEFT_BUTTON:
      if (state == GLUT_DOWN) {
        glGetIntegerv(GL_VIEWPORT, viewport);
        glGetDoublev(GL_MODELVIEW_MATRIX, mvmatrix);
        glGetDoublev(GL_PROJECTION_MATRIX, projmatrix);
        /*  note viewport[3] is height of window in pixels, that why -1 is
         *  needed, only works if viewport height is the same as window height,
         *  otherwise you need window height to get realy  */
        realy = viewport[3] - (GLint)y - 1;
        printf("Coordinates at cursor are (%4d, %4d)\n", x, realy);
        gluUnProject((GLdouble)x, (GLdouble)realy, 0.0, mvmatrix, projmatrix,
          viewport, &wx, &wy, &wz);
        printf("World coords at z=0.0 are (%f, %f, %f)\n", wx, wy, wz);
        gluUnProject((GLdouble)x, (GLdouble)realy, 1.0, mvmatrix, projmatrix,
          viewport, &wx, &wy, &wz);
        printf("World coords at z=1.0 are (%f, %f, %f)\n", wx, wy, wz);
      }
      break;
    case GLUT_RIGHT_BUTTON:
      if (state == GLUT_DOWN) exit(0);
      break;
    default:
      break;
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

/*
 *  Open window, register input callback functions
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutMainLoop();
  return 0;
}
