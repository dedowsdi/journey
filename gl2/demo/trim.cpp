/*
 *  trim.c
 *  this program draws a NURBS surface in the shape of a
 *  symmetrical hill, using both a NURBS curve and pwl
 *  (piecewise linear) curve to trim part of the surface.
 *
 *  you can trim nurb surface by piece wise linear curve or nurb curve.
 *  everything on left side of the curve will be consided inside, everything on
 *  right side of the curve will be discarded.
 *
 */
#include <app.h>

namespace zxd {

#ifndef CALLBACK
#define CALLBACK
#endif

GLfloat ctlpoints[4][4][3];

GLUnurbsObj *the_nurb;

void CALLBACK nurbs_error(GLenum error_code) {
  const GLubyte *estring;

  estring = gluErrorString(error_code);
  fprintf(stderr, "nurbs error: %s\n", estring);
  exit(0);
}

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "trim";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

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

  /*  initialize material property and depth buffer.
   */
  void create_scene(void) {
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
    gluNurbsProperty(the_nurb, GLU_SAMPLING_TOLERANCE, 25.0);
    gluNurbsProperty(the_nurb, GLU_DISPLAY_MODE, GLU_FILL);
    gluNurbsCallback(
      the_nurb, GLU_ERROR, reinterpret_cast<_GLUfuncptr>(nurbs_error));
  }

  void display(void) {
    GLfloat knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    GLfloat edge_pt[5][2] = /* counter clockwise. include entire region */
      {{0.0, 0.0}, {1.0, 0.0}, {1.0, 1.0}, {0.0, 1.0}, {0.0, 0.0}};

    // m = 7, n = 3 p = 3 for this nurb curve. glu will close the curve for you.
    GLfloat curve_pt[4][2] = /* clockwise */
      {{0.25, 0.5}, {0.25, 0.75}, {0.75, 0.75}, {0.75, 0.5}};
    GLfloat curve_knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};

    GLfloat pwl_pt[3][2] = /* clockwise */
      {{0.75, 0.5}, {0.5, 0.25}, {0.25, 0.5}};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glRotatef(330.0, 1., 0., 0.);
    glScalef(0.5, 0.5, 0.5);

    gluBeginSurface(the_nurb);
    gluNurbsSurface(the_nurb, 8, knots, 8, knots, 4 * 3, 3, &ctlpoints[0][0][0],
      4, 4, GL_MAP2_VERTEX_3);

    // include the engire region
    gluBeginTrim(the_nurb);
    gluPwlCurve(the_nurb, 5, &edge_pt[0][0], 2, GLU_MAP1_TRIM_2);
    gluEndTrim(the_nurb);

    gluBeginTrim(the_nurb);
    gluNurbsCurve(
      the_nurb, 8, curve_knots, 2, &curve_pt[0][0], 4, GLU_MAP1_TRIM_2);
    gluPwlCurve(the_nurb, 3, &pwl_pt[0][0], 2, GLU_MAP1_TRIM_2);
    gluEndTrim(the_nurb);

    gluEndSurface(the_nurb);

    glPopMatrix();
    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble)w / (GLdouble)h, 3.0, 8.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }

  /*  main loop
   */
};
}
int main(int argc, char **argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
