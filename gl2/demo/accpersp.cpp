/*  accpersp.c
 *  use the accumulation buffer to do full-scene antialiasing
 *  on a scene with perspective projection, using the special
 *  routines acc_frustum() and acc_perspective().
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include <math.h>
#include "jitter.h"
#include <stdio.h>
#include "common.h"

#define PI_ 3.14159265358979323846

GLuint acsize = 16;

/* acc_frustum()
 * the first 6 arguments are identical to the glFrustum() call.
 *
 * pixdx and pixdy are anti-alias jitter in pixels.
 * set both equal to 0.0 for no anti-alias jitter.
 * eyedx and eyedy are depth-of field jitter in pixels.
 * set both equal to 0.0 for no depth of field effects.
 *
 * focus is distance from eye to plane in focus.
 * focus must be greater than, but not equal to 0.0.
 *
 * note that acc_frustum() calls glTranslatef().  you will
 * probably want to insure that your model_view matrix has been
 * initialized to identity before calling acc_frustum().
 */
void acc_frustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top,
  GLdouble z_near, GLdouble z_far, GLdouble pixdx, GLdouble pixdy, GLdouble eyedx,
  GLdouble eyedy, GLdouble focus) {
  GLdouble xwsize, ywsize;
  GLdouble dx, dy;
  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);

  xwsize = right - left;
  ywsize = top - bottom;

  dx = -(pixdx * xwsize / (GLdouble)viewport[2] + eyedx * z_near / focus);
  dy = -(pixdy * ywsize / (GLdouble)viewport[3] + eyedy * z_near / focus);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glFrustum(left + dx, right + dx, bottom + dy, top + dy, z_near, z_far);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(-eyedx, -eyedy, 0.0);
}

/* acc_perspective()
 *
 * the first 4 arguments are identical to the gluPerspective() call.
 * pixdx and pixdy are anti-alias jitter in pixels.
 * set both equal to 0.0 for no anti-alias jitter.
 * eyedx and eyedy are depth-of field jitter in pixels.
 * set both equal to 0.0 for no depth of field effects.
 *
 * focus is distance from eye to plane in focus.
 * focus must be greater than, but not equal to 0.0.
 *
 * note that acc_perspective() calls acc_frustum().
 */
void acc_perspective(GLdouble fovy, GLdouble aspect, GLdouble z_near,
  GLdouble z_far, GLdouble pixdx, GLdouble pixdy, GLdouble eyedx, GLdouble eyedy,
  GLdouble focus) {
  GLdouble fov2, left, right, bottom, top;

  fov2 = ((fovy * PI_) / 180.0) / 2.0;

  top = z_near / (cos(fov2) / sin(fov2));
  bottom = -top;

  right = top * aspect;
  left = -right;

  acc_frustum(
    left, right, bottom, top, z_near, z_far, pixdx, pixdy, eyedx, eyedy, focus);
}

/*  initialize lighting and other values.
 */
void init(void) {
  GLfloat mat_ambient[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_position[] = {0.0, 0.0, 10.0, 1.0};
  GLfloat lm_ambient[] = {0.2, 0.2, 0.2, 1.0};

  glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialf(GL_FRONT, GL_SHININESS, 50.0);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position);
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lm_ambient);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glClearAccum(0.0, 0.0, 0.0, 0.0);
}

void display_objects(void) {
  GLfloat torus_diffuse[] = {0.7, 0.7, 0.0, 1.0};
  GLfloat cube_diffuse[] = {0.0, 0.7, 0.7, 1.0};
  GLfloat sphere_diffuse[] = {0.7, 0.0, 0.7, 1.0};
  GLfloat octa_diffuse[] = {0.7, 0.4, 0.4, 1.0};

  glPushMatrix();
  glTranslatef(0.0, 0.0, -5.0);
  glRotatef(30.0, 1.0, 0.0, 0.0);

  glPushMatrix();
  glTranslatef(-0.80, 0.35, 0.0);
  glRotatef(100.0, 1.0, 0.0, 0.0);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, torus_diffuse);
  glutSolidTorus(0.275, 0.85, 16, 16);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-0.75, -0.50, 0.0);
  glRotatef(45.0, 0.0, 0.0, 1.0);
  glRotatef(45.0, 1.0, 0.0, 0.0);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, cube_diffuse);
  glutSolidCube(1.5);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.75, 0.60, 0.0);
  glRotatef(30.0, 1.0, 0.0, 0.0);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, sphere_diffuse);
  glutSolidSphere(1.0, 16, 16);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0.70, -0.90, 0.25);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, octa_diffuse);
  glutSolidOctahedron();
  glPopMatrix();

  glPopMatrix();
}

void display(void) {
  GLint viewport[4];
  int jitter;

  glGetIntegerv(GL_VIEWPORT, viewport);

  glClear(GL_ACCUM_BUFFER_BIT);
  for (jitter = 0; jitter < acsize; jitter++) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    acc_perspective(50.0, (GLdouble)viewport[2] / (GLdouble)viewport[3], 1.0,
      15.0, j8[jitter].x, j8[jitter].y, 0.0, 0.0, 1.0);
    display_objects();
    glAccum(GL_ACCUM, 1.0 / acsize);
  }
  glAccum(GL_RETURN, 1.0);

  glWindowPos2i(20, 230);
  const GLubyte info[] = "q : accumulation size";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);

  glFlush();
}

void reshape(int w, int h) { glViewport(0, 0, (GLsizei)w, (GLsizei)h); }

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      ++acsize;
      printf("accumulation size : %d\n", acsize);
      glutPostRedisplay();
      break;
    case 'Q':
      if (acsize > 1) --acsize;
      printf("accumulation size : %d\n", acsize);
      glutPostRedisplay();
      break;
      break;
    default:
      break;
  }
}

/*  main loop
 *  be certain you request an accumulation buffer.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_ACCUM | GLUT_DEPTH);
  glutInitWindowSize(250, 250);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadgl();
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
