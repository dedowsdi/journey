/*
 *  model.c
 *  This program demonstrates modeling transformations
 *
 *  Model and view is conmbined together in opengl, it's called modelView,
 *  modewView = T0T1T2T3T4...Tn, push T at back works like transorm object in
 *  local space,  push T at front works like transform the enire scene in
 *  viewspace.
 *
 *  It can be seperated like this:
 *    view = T0T1..
 *    world = T1T2T3T4..
 *    mordelview = view * world.
 *
 *  It's hard to imagin where the camera is if you look directly at T0T1, as
 *  p_v = T0T1 * p_w,  p_w = (T1^t)(T0^t) p_v, which means if you align camera
 *  frame with world frame, then rotate in world frame by (T1^t)(T0^t), you get
 *  the camera frame in world space.
 *
 *  ModelView transform works like a node tree. It's root is eye, everytime you
 *  call glRotate or stuff like that, you create a new child node with transform
 *  T, T is the new intrinsic transform node in the end of the transform tree,
 *  T can also be used to transform local frame into parent node frame.
 *
 */
#include <GL/freeglut.h>
#include <stdlib.h>

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void draw_triangle(void) {
  glBegin(GL_LINE_LOOP);
  glVertex2f(0.0, 25.0);
  glVertex2f(25.0, -25.0);
  glVertex2f(-25.0, -25.0);
  glEnd();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);

  glLoadIdentity();
  glColor3f(1.0, 1.0, 1.0);
  draw_triangle();

  glEnable(GL_LINE_STIPPLE);
  glLineStipple(1, 0xF0F0);
  glLoadIdentity();
  glTranslatef(-20.0, 0.0, 0.0);
  draw_triangle();

  glLineStipple(1, 0xF00F);
  glLoadIdentity();
  glScalef(1.5, 0.5, 1.0);
  draw_triangle();

  glLineStipple(1, 0x8888);
  glLoadIdentity();
  glRotatef(90.0, 0.0, 0.0, 1.0);
  draw_triangle();
  glDisable(GL_LINE_STIPPLE);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  //this trick is used to place a*a area into viewport as much as possible.
  //it scales the view port so the narrow edge is the same as a, just like find
  //a rectangle with specific aspect ratio for a square.
  if (w <= h)
    glOrtho(-50.0, 50.0, -50.0 * (GLfloat)h / (GLfloat)w,
      50.0 * (GLfloat)h / (GLfloat)w, -1.0, 1.0);
  else
    glOrtho(-50.0 * (GLfloat)w / (GLfloat)h, 50.0 * (GLfloat)w / (GLfloat)h,
      -50.0, 50.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
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
