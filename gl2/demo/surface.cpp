/*
 *  surface.c
 *  this program draws a NURBS surface in the shape of a
 *  symmetrical hill.  the 'c' keyboard key allows you to
 *  toggle the visibility of the control points themselves.
 *  note that some of the control points are hidden by the
 *  surface itself.
 *
 *  change u step will change the surface slightly even the sample mothed is not
 *  domain distance.
 */
#include "glad/glad.h"
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include "glEnumString.h"
#include <GL/freeglut_ext.h>
#include "common.h"

#ifndef CALLBACK
#define CALLBACK
#endif

GLfloat ctlpoints[4][4][3];
int show_points = 0;

GLUnurbsObj *the_nurb;

GLenum sample_methods[] = {GLU_OBJECT_PARAMETRIC_ERROR, GLU_OBJECT_PATH_LENGTH,
  GLU_PATH_LENGTH, GLU_PARAMETRIC_ERROR, GLU_DOMAIN_DISTANCE};

/*
 *  initializes the control points of the surface to a small hill.
 *  the control points range from -3 to +3 in x, y, and z
 */
void init_surface(void) {
  int u, v;
  for (u = 0; u < 4; u++) {
    for (v = 0; v < 4; v++) {
      ctlpoints[u][v][0] = 2.0 * ((GLfloat)u - 1.5);
      ctlpoints[u][v][1] = 2.0 * ((GLfloat)v - 1.5);

      if ((u == 1 || u == 2) && (v == 1 || v == 2))
        ctlpoints[u][v][2] = 3.0;
      else
        ctlpoints[u][v][2] = -3.0;
    }
  }
}

void CALLBACK nurbs_error(GLenum error_code) {
  const GLubyte *estring;

  estring = gluErrorString(error_code);
  fprintf(stderr, "nurbs error: %s\n", estring);
  exit(0);
}

/*  initialize material property and depth buffer.
 */
void init(void) {
  GLfloat mat_diffuse[] = {0.7, 0.7, 0.7, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_shininess[] = {100.0};

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);

  init_surface();

  the_nurb = gluNewNurbsRenderer();

  // control tessellate granularity
  gluNurbsProperty(the_nurb, GLU_SAMPLING_METHOD, GLU_PATH_LENGTH);  // default
  gluNurbsProperty(the_nurb, GLU_SAMPLING_TOLERANCE, 25.0);

  gluNurbsProperty(the_nurb, GLU_DISPLAY_MODE, GLU_FILL);
  gluNurbsCallback(the_nurb, GLU_ERROR, reinterpret_cast<_GLUfuncptr>(nurbs_error));
}

void display(void) {
  // degrade to bezier surface
  GLfloat knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
  int i, j;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  glRotatef(330.0, 1., 0., 0.);
  glScalef(0.5, 0.5, 0.5);

  gluBeginSurface(the_nurb);
  gluNurbsSurface(the_nurb, 8, knots, 8, knots, 4 * 3, 3, &ctlpoints[0][0][0], 4,
    4, GL_MAP2_VERTEX_3); //non rational
  gluEndSurface(the_nurb);

  if (show_points) {
    glPointSize(5.0);
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 0.0);
    glBegin(GL_POINTS);
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        glVertex3f(ctlpoints[i][j][0], ctlpoints[i][j][1], ctlpoints[i][j][2]);
      }
    }
    glEnd();
    glEnable(GL_LIGHTING);
  }
  glPopMatrix();

  glColor3f(1.0f,1.0f,1.0f);
  glWindowPos2i(10, 480);
  char info[512];
  GLfloat sample_method, sample_tolerance, parametric_tolerance, ustep, vstep;

  gluGetNurbsProperty(the_nurb, GLU_SAMPLING_METHOD, &sample_method);
  gluGetNurbsProperty(the_nurb, GLU_SAMPLING_TOLERANCE, &sample_tolerance);
  gluGetNurbsProperty(the_nurb, GLU_PARAMETRIC_TOLERANCE, &parametric_tolerance);
  gluGetNurbsProperty(the_nurb, GLU_U_STEP, &ustep);
  gluGetNurbsProperty(the_nurb, GLU_V_STEP, &vstep);

  sprintf(info,
    "q : sample_method : %s \n"
    "wW : sample_tolerance : %.2f \n"
    "eE : parametric_tolerance : %.2f \n"
    "uU : u step : %.0f \n"
    "iI : v step : %.0f \n"
    "o : show points \n",
    glu_nurb_sample_method_to_string(sample_method), (GLdouble)sample_tolerance,
    (GLdouble)parametric_tolerance, ustep, vstep);

  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte *)info);
  glEnable(GL_TEXTURE_2D);
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 3.0, 8.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -5.0);
}

void keyboard(unsigned char key, int x, int y) {
  // just for convinence, get all of the properties here.
  GLfloat sample_method, sample_tolerance, parametric_tolerance, ustep, vstep;

  gluGetNurbsProperty(the_nurb, GLU_SAMPLING_METHOD, &sample_method);
  gluGetNurbsProperty(the_nurb, GLU_SAMPLING_TOLERANCE, &sample_tolerance);
  gluGetNurbsProperty(the_nurb, GLU_PARAMETRIC_TOLERANCE, &parametric_tolerance);
  gluGetNurbsProperty(the_nurb, GLU_U_STEP, &ustep);
  gluGetNurbsProperty(the_nurb, GLU_V_STEP, &vstep);

  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
    case 'Q': {
      GLint i = 0;
      while (sample_methods[i] != (GLenum)sample_method) ++i;
      gluNurbsProperty(the_nurb, GLU_SAMPLING_METHOD, sample_methods[++i % 5]);
      glutPostRedisplay();
    } break;
    case 'w':
      sample_tolerance += 5;
      gluNurbsProperty(the_nurb, GLU_SAMPLING_TOLERANCE, sample_tolerance);
      glutPostRedisplay();
      break;
    case 'W':
      if (sample_tolerance > 10) {
        sample_tolerance -= 5;
        gluNurbsProperty(the_nurb, GLU_SAMPLING_TOLERANCE, sample_tolerance);
        glutPostRedisplay();
      }
      break;
    case 'e':
      parametric_tolerance += 0.05;
      gluNurbsProperty(the_nurb, GLU_PARAMETRIC_TOLERANCE, parametric_tolerance);
      glutPostRedisplay();
      break;
    case 'E':
      if (parametric_tolerance > 0.1) {
        parametric_tolerance -= 0.05;
        gluNurbsProperty(
          the_nurb, GLU_PARAMETRIC_TOLERANCE, parametric_tolerance);
        glutPostRedisplay();
      }
      break;
    case 'u':
      ustep += 1;
      gluNurbsProperty(the_nurb, GLU_U_STEP, ustep);
      glutPostRedisplay();
      break;
    case 'U':
      if (ustep > 1) {
        ustep -= 1;
        gluNurbsProperty(the_nurb, GLU_U_STEP, ustep);
        glutPostRedisplay();
      }
      break;
    case 'i':
      vstep += 1;
      gluNurbsProperty(the_nurb, GLU_V_STEP, vstep);
      glutPostRedisplay();
      break;
    case 'I':
      if (vstep > 1) {
        vstep -= 1;
        gluNurbsProperty(the_nurb, GLU_V_STEP, vstep);
        glutPostRedisplay();
      }
      break;
    case 'o':
      show_points = !show_points;
      glutPostRedisplay();
      break;
    default:
      break;
  }
}

int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
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
