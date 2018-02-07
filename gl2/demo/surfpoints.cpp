/*
 *  surfpoints.c
 *  this program is a modification of the earlier surface.c
 *  program.  the vertex data are not directly rendered,
 *  but are instead passed to the callback function.
 *  the values of the tessellated vertices are printed
 *  out there.
 *
 *  this program draws a NURBS surface in the shape of a
 *  symmetrical hill.  the 'c' keyboard key allows you to
 *  toggle the visibility of the control points themselves.
 *  note that some of the control points are hidden by the
 *  surface itself.
 *
 *  you can control the tesslate process like this:
 *      gluNurbsProperty(the_nurb, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);
 *      gluNurbsCallback(the_nurb, GLU_NURBS_BEGIN, begin_callback);
 *      gluNurbsCallback(the_nurb, GLU_NURBS_VERTEX, vertex_callback);
 *      gluNurbsCallback(the_nurb, GLU_NURBS_NORMAL, normal_callback);
 *      gluNurbsCallback(the_nurb, GLU_NURBS_END, end_callback);
 */
#include "app.h"

namespace zxd {

#ifndef CALLBACK
#define CALLBACK
#endif

void CALLBACK nurbs_error(GLenum error_code) {
  const GLubyte *estring;

  estring = gluErrorString(error_code);
  fprintf(stderr, "nurbs error: %s\n", estring);
  exit(0);
}

void CALLBACK begin_callback(GLenum which_type) {
  glBegin(which_type); /*  resubmit rendering directive  */
  printf("glBegin(");
  switch (which_type) { /*  print diagnostic message  */
    case GL_LINES:
      printf("GL_LINES)\n");
      break;
    case GL_LINE_LOOP:
      printf("GL_LINE_LOOP)\n");
      break;
    case GL_LINE_STRIP:
      printf("GL_LINE_STRIP)\n");
      break;
    case GL_TRIANGLES:
      printf("GL_TRIANGLES)\n");
      break;
    case GL_TRIANGLE_STRIP:
      printf("GL_TRIANGLE_STRIP)\n");
      break;
    case GL_TRIANGLE_FAN:
      printf("GL_TRIANGLE_FAN)\n");
      break;
    case GL_QUADS:
      printf("GL_QUADS)\n");
      break;
    case GL_QUAD_STRIP:
      printf("GL_QUAD_STRIP)\n");
      break;
    case GL_POLYGON:
      printf("GL_POLYGON)\n");
      break;
    default:
      break;
  }
}

void CALLBACK end_callback() {
  glEnd(); /*  resubmit rendering directive  */
  printf("glEnd()\n");
}

void CALLBACK vertex_callback(GLfloat *vertex) {
  glVertex3fv(vertex); /*  resubmit rendering directive  */
  printf("glVertex3f (%5.3f, %5.3f, %5.3f)\n", vertex[0], vertex[1], vertex[2]);
}

void CALLBACK normal_callback(GLfloat *normal) {
  glNormal3fv(normal); /*  resubmit rendering directive  */
  printf("glNormal3f (%5.3f, %5.3f, %5.3f)\n", normal[0], normal[1], normal[2]);
}

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "surfpoints";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  GLfloat ctlpoints[4][4][3];
  int show_points = 0;

  GLUnurbsObj *the_nurb;

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
    gluNurbsProperty(the_nurb, GLU_NURBS_MODE, GLU_NURBS_TESSELLATOR);
    gluNurbsProperty(the_nurb, GLU_SAMPLING_TOLERANCE, 25.0);
    gluNurbsProperty(the_nurb, GLU_DISPLAY_MODE, GLU_FILL);
    gluNurbsCallback(
      the_nurb, GLU_ERROR, reinterpret_cast<_GLUfuncptr>(nurbs_error));
    gluNurbsCallback(
      the_nurb, GLU_NURBS_BEGIN, reinterpret_cast<_GLUfuncptr>(begin_callback));
    gluNurbsCallback(the_nurb, GLU_NURBS_VERTEX,
      reinterpret_cast<_GLUfuncptr>(vertex_callback));
    gluNurbsCallback(the_nurb, GLU_NURBS_NORMAL,
      reinterpret_cast<_GLUfuncptr>(normal_callback));
    gluNurbsCallback(the_nurb, GLU_NURBS_END, end_callback);
  }

  void display(void) {
    GLfloat knots[8] = {0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0};
    int i, j;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef(330.0, 1., 0., 0.);
    glScalef(0.5, 0.5, 0.5);

    gluBeginSurface(the_nurb);
    gluNurbsSurface(the_nurb, 8, knots, 8, knots, 4 * 3, 3, &ctlpoints[0][0][0],
      4, 4, GL_MAP2_VERTEX_3);
    gluEndSurface(the_nurb);

    if (show_points) {
      glPointSize(5.0);
      glDisable(GL_LIGHTING);
      glColor3f(1.0, 1.0, 0.0);
      glBegin(GL_POINTS);
      for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
          glVertex3f(
            ctlpoints[i][j][0], ctlpoints[i][j][1], ctlpoints[i][j][2]);
        }
      }
      glEnd();
      glEnable(GL_LIGHTING);
    }
    glPopMatrix();
    glFlush();
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
    switch (key) {
      case 'c':
      case 'C':
        show_points = !show_points;
        break;
      default:
        break;
    }
  }
};
}
int main(int argc, char **argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
