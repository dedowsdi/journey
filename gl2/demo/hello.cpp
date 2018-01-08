#include <GL/freeglut.h>
#include <stdlib.h>

void display(void) {
  /* clear all pixels  */
  glClear(GL_COLOR_BUFFER_BIT);

  /* draw white polygon (rectangle) with corners at
   * (0.25, 0.25, 0.0) and (0.75, 0.75, 0.0)
   */
  glColor3f(1.0, 1.0, 1.0);
  glBegin(GL_POLYGON);
  glVertex3f(0.25, 0.25, 0.0);
  glVertex3f(0.75, 0.25, 0.0);
  glVertex3f(0.75, 0.75, 0.0);
  glVertex3f(0.25, 0.75, 0.0);
  glEnd();

  /* don't wait!
   * start processing buffered OpenGL routines
   * Force previously issued opengl commands to begin execution
   */
  glFlush();
}

void init(void) {
  /* select clearing color 	*/
  glClearColor(0.0, 0.0, 0.0, 0.0);

  /* initialize viewing values  */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

/*
 * Declare initial window size, position, and display mode
 * (single buffer and RGBA).  Open window with "hello"
 * in its title bar.  Call initialization routines.
 * Register callback function to display graphics.
 * Enter main loop and process events.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(250, 250);
  glutInitWindowPosition(100, 100);
  glutCreateWindow("hello");
  init();
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0; /* ANSI C requires main to return int. */
}
