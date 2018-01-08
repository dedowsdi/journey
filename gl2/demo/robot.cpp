/*
 * robot.c
 * This program shows how to composite modeling transformations
 * to draw translated and rotated hierarchical models.
 * Interaction:  pressing the s and e keys (shoulder and elbow)
 * alters the rotation of the robot arm.
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <GL/freeglut_ext.h>

// rotation angles
static int shoulder = 0, elbow = 0, indexFinger0 = 0, indexFinger1 = 0,
           middleFinger0 = 0, middleFinger1 = 0, ringFinger0 = 0,
           ringFinger1 = 0, thumb0 = 0, thumb1;

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);

  // enable line smooth and blend to handle antialiasing
  glEnable(GL_LINE_SMOOTH);
  glLineWidth(1.5);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

//draw a cube with left face centered at current node
void drawSegment(int roll, double width, double height, double depth) {
  glRotatef((GLfloat)roll, 0.0, 0.0, 1.0);
  glTranslatef(width / 2, 0.0, 0.0);

  glPushMatrix();
  glScalef(width, height, depth);
  glutWireCube(1.0);
  glPopMatrix();
}

void drawFinger(
  int roll0, int roll1, double width, double height, double depth) {
  drawSegment(roll0, width, height, depth);
  glTranslatef(width * 0.5, 0.0f, 0.0f);
  drawSegment(roll1, width, height, depth);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glPushMatrix();

  GLdouble sw = 2.0, sh = 0.4, sd = 0.5;  // shoulder width height depth
  GLdouble ew = 2.0, eh = 0.4, ed = 0.5;  // elbow width height depth
  GLdouble fw = 0.4, fh = 0.1, fd = 0.1;  // figner width height depth
  // half
  GLdouble hsw = 0.5 * sw, hew = 0.5 * ew, heh = 0.5 * eh, hed = 0.5 * ed,
           hfw = 0.5 * fw, hfh = 0.5 * fh, hfd = 0.5 * fd;

  // shoulder
  glTranslatef(-hsw, 0.0, 0.0);
  drawSegment(shoulder, sw, sh, sd);

  // elbow
  glTranslatef(hsw, 0.0, 0.0);
  drawSegment(elbow, ew, eh, ed);

  glTranslatef(hew, 0.0, 0.0);

  // index finger
  glPushMatrix();
  glTranslatef(0, heh - hfh, hed - hfd);
  drawFinger(indexFinger0, indexFinger1, fw, fh, fd);
  glPopMatrix();

  // middle finger
  glPushMatrix();
  glTranslatef(0, heh - hfh, 0);
  drawFinger(middleFinger0, middleFinger1, fw, fh, fd);
  glPopMatrix();

  // ring finger
  glPushMatrix();
  glTranslatef(0, heh - hfh, -hed + hfd);
  drawFinger(ringFinger0, ringFinger1, fw, fh, fd);
  glPopMatrix();

  // thumb
  glPushMatrix();
  glTranslatef(0, -heh + hfh, hed - hfd);
  drawFinger(thumb0, thumb1, fw, fh, fd);
  glPopMatrix();

  glPopMatrix();

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  const GLubyte info[] =
    "qQ : shoulder \n"
    "wW : albow \n"
    "yY : thumb0 \n"
    "hY : thumb1 \n"
    "uU : index0 \n"
    "jJ : index1 \n"
    "iI : middle0 \n"
    "kK : middle1 \n"
    "oO : ring0 \n"
    "lL : ring1 \n";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -7.0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q':
      shoulder = (shoulder + 5) % 360;
      glutPostRedisplay();
      break;
    case 'Q':
      shoulder = (shoulder - 5) % 360;
      glutPostRedisplay();
      break;
    case 'w':
      elbow = (elbow + 5) % 360;
      glutPostRedisplay();
      break;
    case 'W':
      elbow = (elbow - 5) % 360;
      glutPostRedisplay();
      break;
    case 'y':
      thumb0 = (thumb0 + 5) % 360;
      glutPostRedisplay();
      break;
    case 'Y':
      thumb0 = (thumb0 - 5) % 360;
      glutPostRedisplay();
      break;
    case 'h':
      thumb1 = (thumb1 + 5) % 360;
      glutPostRedisplay();
      break;
    case 'H':
      thumb1 = (thumb1 - 5) % 360;
      glutPostRedisplay();
      break;
    case 'u':
      indexFinger0 = (indexFinger0 + 5) % 360;
      glutPostRedisplay();
      break;
    case 'U':
      indexFinger0 = (indexFinger0 - 5) % 360;
      glutPostRedisplay();
      break;
    case 'j':
      indexFinger1 = (indexFinger1 + 5) % 360;
      glutPostRedisplay();
      break;
    case 'J':
      indexFinger1 = (indexFinger1 - 5) % 360;
      glutPostRedisplay();
      break;
    case 'i':
      middleFinger0 = (middleFinger0 + 5) % 360;
      glutPostRedisplay();
      break;
    case 'I':
      middleFinger0 = (middleFinger0 - 5) % 360;
      glutPostRedisplay();
      break;
    case 'k':
      middleFinger1 = (middleFinger1 + 5) % 360;
      glutPostRedisplay();
      break;
    case 'K':
      middleFinger1 = (middleFinger1 - 5) % 360;
      glutPostRedisplay();
      break;
    case 'o':
      ringFinger0 = (ringFinger0 + 5) % 360;
      glutPostRedisplay();
      break;
    case 'O':
      ringFinger0 = (ringFinger0 - 5) % 360;
      glutPostRedisplay();
      break;
    case 'l':
      ringFinger1 = (ringFinger1 + 5) % 360;
      glutPostRedisplay();
      break;
    case 'L':
      ringFinger1 = (ringFinger1 - 5) % 360;
      glutPostRedisplay();
      break;
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
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
