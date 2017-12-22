/*
 *  list.c
 *  This program demonstrates how to make and execute a
 *  display list.  Note that attributes, such as current
 *  color and matrix, are changed.
 */
#include <GL/freeglut.h>
#include <stdlib.h>

GLuint listName;

static void init(void) {
  listName = glGenLists(1);

  glNewList(listName, GL_COMPILE);
  /*glPushAttrib(GL_CURRENT_BIT);*/  // you can use this to restore current
                                     // color 
  glColor3f(1.0, 0.0, 0.0); /*  current color red  */
  glBegin(GL_TRIANGLES);
  glVertex2f(0.0, 0.0);
  glVertex2f(1.0, 0.0);
  glVertex2f(0.0, 1.0);
  glEnd();
  glTranslatef(1.5, 0.0, 0.0); /*  move position  */
  /*glPopAttrib();*/
  glEndList();
  glShadeModel(GL_FLAT);
}

static void drawLine(void) {
  glBegin(GL_LINES);
  glVertex2f(0.0, 0.5);
  glVertex2f(15.0, 0.5);
  glEnd();
}

void display(void) {
  GLuint i;

  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(0.0, 1.0, 0.0); /*  current color green  */
  for (i = 0; i < 10; i++) /*  draw 10 triangles, also change current color   */
    glCallList(listName);
  /*glLoadIdentity();*/
  drawLine(); /*  is this line green?  NO!  */
              /*  where is the line drawn?  */
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    gluOrtho2D(
      0.0, 2.0, -0.5 * (GLfloat)h / (GLfloat)w, 1.5 * (GLfloat)h / (GLfloat)w);
  else
    gluOrtho2D(0.0, 2.0 * (GLfloat)w / (GLfloat)h, -0.5, 1.5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(800, 50);
  glutCreateWindow(argv[0]);
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
