/*
 *  tess.c
 *  this program demonstrates polygon tessellation.
 *  two tesselated objects are drawn.  the first is a
 *  rectangle with a triangular hole.  the second is a
 *  smooth shaded, self-intersecting star.
 *
 *  note the exterior rectangle is drawn with its vertices
 *  in counter-clockwise order, but its interior clockwise.
 *  note the combine_callback is needed for the self-intersecting
 *  star.  also note that removing the tess_property for the
 *  star will make the interior unshaded (WINDING_ODD).
 */
#include <app.h>

// empty CALLBACK makesure callback func works on both linux and windows
#ifndef CALLBACK
#define CALLBACK
#endif

GLuint start_list;

namespace zxd {

void CALLBACK begin_callback(GLenum which) { glBegin(which); }

void CALLBACK error_callback(GLenum error_code) {
  const GLubyte *estring;

  estring = gluErrorString(error_code);
  fprintf(stderr, "tessellation error: %s\n", estring);
  exit(0);
}

void CALLBACK end_callback(void) { glEnd(); }

void CALLBACK vertex_callback(GLvoid *vertex) {
  const GLdouble *pointer;

  pointer = (GLdouble *)vertex;
  glColor3dv(pointer + 3);
  glVertex3dv(pointer);
}

/*  combine_callback is used to create a new vertex when edges
 *  intersect.
 *
 *  coords: new vertex coordinate
 *  vertex_data: 4 verteices of two intersecting edges
 *  weight: vertex weight, can be used to calculate color, normal, texure..
 *  data_out: output data
 */
void CALLBACK combine_callback(GLdouble coords[3], GLdouble *vertex_data[4],
  GLfloat weight[4], GLdouble **data_out) {
  GLdouble *vertex;
  int i;

  // x, y, z, r, g, b
  vertex = (GLdouble *)malloc(6 * sizeof(GLdouble));

  vertex[0] = coords[0];
  vertex[1] = coords[1];
  vertex[2] = coords[2];
  for (i = 3; i < 6; i++)
    vertex[i] = weight[0] * vertex_data[0][i] + weight[1] * vertex_data[1][i] +
                weight[2] * vertex_data[2][i] + weight[3] * vertex_data[3][i];
  *data_out = vertex;
}

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "tess";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glCallList(start_list);
    glCallList(start_list + 1);
    glFlush();
  }

  void create_scene(void) {
    GLUtesselator *tobj;
    // clang-format off
   GLdouble rect[4][3] = {50.0, 50.0, 0.0,
                          200.0, 50.0, 0.0,
                          200.0, 200.0, 0.0,
                          50.0, 200.0, 0.0};
   //cw inner triangle
   GLdouble tri[3][3] = {75.0, 75.0, 0.0,
                         125.0, 175.0, 0.0,
                         175.0, 75.0, 0.0};
   //self intersect star
   GLdouble star[5][6] = {250.0, 50.0, 0.0, 1.0, 0.0, 1.0,
                          325.0, 200.0, 0.0, 1.0, 1.0, 0.0,
                          400.0, 50.0, 0.0, 0.0, 1.0, 1.0,
                          250.0, 150.0, 0.0, 1.0, 0.0, 0.0,
                          400.0, 150.0, 0.0, 0.0, 1.0, 0.0};
    // clang-format on

    glClearColor(0.0, 0.0, 0.0, 0.0);

    start_list = glGenLists(2);

    tobj = gluNewTess();
    gluTessCallback(
      tobj, GLU_TESS_VERTEX, reinterpret_cast<_GLUfuncptr>(glVertex3dv));
    gluTessCallback(
      tobj, GLU_TESS_BEGIN, reinterpret_cast<_GLUfuncptr>(begin_callback));
    gluTessCallback(
      tobj, GLU_TESS_END, reinterpret_cast<_GLUfuncptr>(end_callback));
    gluTessCallback(
      tobj, GLU_TESS_ERROR, reinterpret_cast<_GLUfuncptr>(error_callback));

    /*  rectangle with triangular hole inside  */
    glNewList(start_list, GL_COMPILE);  // use list to cache tessllator result
    glShadeModel(GL_FLAT);
    gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_ODD);
    /*gluTessProperty(tobj, GLU_TESS_BOUNDARY_ONLY, GL_TRUE);*/
    gluTessBeginPolygon(tobj, NULL);
    gluTessBeginContour(tobj);  // outer rect
    gluTessVertex(tobj, rect[0], rect[0]);
    gluTessVertex(tobj, rect[1], rect[1]);
    gluTessVertex(tobj, rect[2], rect[2]);
    gluTessVertex(tobj, rect[3], rect[3]);
    gluTessEndContour(tobj);
    gluTessBeginContour(tobj);  // inner triangle
    gluTessVertex(tobj, tri[0], tri[0]);
    gluTessVertex(tobj, tri[1], tri[1]);
    gluTessVertex(tobj, tri[2], tri[2]);
    gluTessEndContour(tobj);
    gluTessEndPolygon(tobj);
    glEndList();

    gluTessCallback(
      tobj, GLU_TESS_VERTEX, reinterpret_cast<_GLUfuncptr>(vertex_callback));
    gluTessCallback(
      tobj, GLU_TESS_BEGIN, reinterpret_cast<_GLUfuncptr>(begin_callback));
    gluTessCallback(
      tobj, GLU_TESS_END, reinterpret_cast<_GLUfuncptr>(end_callback));
    gluTessCallback(
      tobj, GLU_TESS_ERROR, reinterpret_cast<_GLUfuncptr>(error_callback));
    gluTessCallback(
      tobj, GLU_TESS_COMBINE, reinterpret_cast<_GLUfuncptr>(combine_callback));

    /*  smooth shaded, self-intersecting star  */
    glNewList(start_list + 1, GL_COMPILE);
    glShadeModel(GL_SMOOTH);
    gluTessProperty(tobj, GLU_TESS_WINDING_RULE, GLU_TESS_WINDING_POSITIVE);
    /*gluTessProperty(tobj, GLU_TESS_BOUNDARY_ONLY, GL_TRUE);*/
    gluTessBeginPolygon(tobj, NULL);
    gluTessBeginContour(tobj);
    gluTessVertex(tobj, star[0], star[0]);
    gluTessVertex(tobj, star[1], star[1]);
    gluTessVertex(tobj, star[2], star[2]);
    gluTessVertex(tobj, star[3], star[3]);
    gluTessVertex(tobj, star[4], star[4]);
    gluTessEndContour(tobj);
    gluTessEndPolygon(tobj);
    glEndList();
    gluDeleteTess(tobj);
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }
};
}
int main(int argc, char **argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
