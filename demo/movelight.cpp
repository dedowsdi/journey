/*
 *  movelight.c
 *  This program demonstrates when to issue lighting and
 *  transformation commands to render a model with a light
 *  which is moved by a modeling transformation (rotate or
 *  translate).  The light position is reset after the modeling
 *  transformation is called.  The eye position does not change.
 *
 *  A sphere is drawn using a grey material characteristic.
 *  A single light source illuminates the object.
 *
 *  Interaction:  q or Q to rotate, w or W to translate.
 *
 *  light position will be transformed by modelview matrix.
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <GL/freeglut_ext.h>

static int spin = 0;
static float translation = 1.5f;

/*  Initialize material property, light source, lighting model,
 *  and depth buffer.
 */
void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);

  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.5);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.125);
}

/*  Here is where the light position is reset after the modeling
 *  transformation (glRotated) is called.  This places the
 *  light at a new position in world coordinates.  The cube
 *  represents the position of the light.
 */
void display(void) {
  GLfloat position[] = {0.0, 0.0, translation, 1.0};

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // update light position
  glPushMatrix();

  glRotated((GLdouble)spin, 1.0, 0.0, 0.0);
  glLightfv(GL_LIGHT0, GL_POSITION, position);

  // draw a wired cube at light position
  glTranslated(position[0], position[1], position[2]);
  glDisable(GL_LIGHTING);
  glColor3f(0.0, 1.0, 1.0);
  glutWireCube(0.1);
  glEnable(GL_LIGHTING);

  glPopMatrix();

  // draw a torus at origin
  glutSolidTorus(0.275, 0.85, 16, 30);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  const GLubyte info[] =
    "qQ : spin\n"
    "wW : translate \n";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  // setup camera
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      spin += 15;
      glutPostRedisplay();
      break;
    case 'Q':
      spin -= 15;
      glutPostRedisplay();
      break;
    case 'w':
      translation += 0.1;
      glutPostRedisplay();
      break;
    case 'W':
      translation -= 0.1;
      glutPostRedisplay();
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
