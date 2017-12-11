/*
 *  stencilcap.c This program demonstrates use of the stencil cap for solid
 *  convex object
 *
 *  If you slice a convex object by a plane, you will see through the cut, we
 *  need stencil buffer to cover the cut.
 *
 *    create stencil :
 *
 *      enable clip plane
 *      disable cull face
 *      clear stencil to 0.
 *      call glStencilFunc(GL_ALWAYS, 0x1, 0x1)
 *      call glStencilOp(GL_KEEP, GL_REVERT, GL_REVERT)
 *      draw convex object.
 *
 *      Any fragments that pass stencil test will get it's stencil reverted, the
 *      fragments that can be seen through the cut will be reverted once(0x1),
 * others
 *      will be reverted twice(0x0).
 *
 *   use stencil:
 *
 *      disable stencil, draw convex object.
 *      enable stencil call glStencilFunc(GL_EQUAL, 0x1, 0x1)
 *      disable clip plane!!!! (otherwise rect will jitter like z fight)
 *      draw a rect aligned to the clip plane, it should be least large enough
 *      to cover the cuts.
 *
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <stdlib.h>
#include "common.h"
#include <GL/freeglut_ext.h>

#define BLUEMAT 1
#define CAP 2

GLfloat matClip[16];
GLint yaw = 0;  // yaw clip plane

void init(void) {
  GLfloat yellow_diffuse[] = {0.7, 0.7, 0.0, 1.0};
  GLfloat yellow_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat blue_diffuse[] = {0.1, 0.1, 0.7, 1.0};
  GLfloat blue_specular[] = {0.1, 1.0, 1.0, 1.0};

  GLfloat position_one[] = {0.0, 1.0, 1.0, 0.0};

  glNewList(BLUEMAT, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, blue_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, blue_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, 45.0);
  glEndList();

  glNewList(CAP, GL_COMPILE);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, yellow_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, 45.0);

  // draw rect aligned with clip plane, make sure it covers entire screen
  glNormal3f(0, 0, 1);  // reverse normal of clip plane
  glColor3f(1.0f, 1.0f, 1.0f);
  glRectd(-1.5, -1.5, 1.5, 1.5);
  glEndList();

  glLightfv(GL_LIGHT0, GL_POSITION, position_one);

  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_DEPTH_TEST);

  glClearStencil(0x0);
  glClearDepth(1);
  glEnable(GL_STENCIL_TEST);
}

void drawObject() {
  glCallList(BLUEMAT);
  glPushMatrix();
  glRotatef(45, 0, 1, 0);
  glutSolidTorus(0.2, 1, 10, 20);
  glPopMatrix();
}

void display(void) {
  // don't clear stencil
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw objects, which is cliped by the clip plane
  drawObject();

  // draw clap aligned to clip plane
  glEnable(GL_STENCIL_TEST);
  glDisable(GL_CLIP_PLANE0);
  glPushMatrix();
  glLoadMatrixf(matClip);
  glStencilFunc(GL_EQUAL, 0x01, 0x01);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glCallList(CAP);
  glPopMatrix();
  glEnable(GL_CLIP_PLANE0);
  glDisable(GL_STENCIL_TEST);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 380);
  const GLubyte info[] = "qQ : yaw clip plane \n";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);

  glFlush();

  ZCGEA
}

void resetStencil() {
  glPushMatrix();
  glRotatef(yaw, 0, 1, 0);
  glGetFloatv(GL_MODELVIEW_MATRIX, matClip);
  GLdouble eqn[4] = {0, 0, -1, 0};
  glClipPlane(GL_CLIP_PLANE0, eqn);
  glEnable(GL_CLIP_PLANE0);
  glPopMatrix();

  // set bitplane where clipplane cut object to 1
  glClear(GL_STENCIL_BUFFER_BIT);

  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  glStencilOp(GL_KEEP, GL_INVERT, GL_INVERT);
  glColorMask(0, 0, 0, 0);
  glDisable(GL_DEPTH_TEST);
  drawObject();
  glEnable(GL_DEPTH_TEST);
  glColorMask(1, 1, 1, 1);
  glDisable(GL_STENCIL_TEST);
}

/*  Whenever the window is reshaped, redefine the
 *  coordinate system and redraw the stencil area.
 */
void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 3.0, 7.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -5.0);

  resetStencil();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      yaw += 5;
      resetStencil();
      glutPostRedisplay();
      break;
    case 'Q':
      yaw -= 5;
      resetStencil();
      glutPostRedisplay();
      break;
    default:
      break;
  }
}

/* Main Loop
 * Be certain to request stencil bits.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
