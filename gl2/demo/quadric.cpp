/*
 *  quadric.c
 *  this program demonstrates the use of some of the gluQuadric*
 *  routines. quadric objects are created with some quadric
 *  properties and the callback routine to handle errors.
 *  note that the cylinder has no top or bottom and the circle
 *  has a hole in it.
 */
#include <app.h>

namespace zxd {

#ifndef CALLBACK
#define CALLBACK
#endif

GLuint start_list;

void CALLBACK error_callback(GLenum error_code) {
  const GLubyte *estring;

  estring = gluErrorString(error_code);
  fprintf(stderr, "quadric error: %s\n", estring);
  exit(0);
}

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "quadric";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    GLUquadricObj *qobj;
    GLfloat mat_ambient[] = {0.5, 0.5, 0.5, 1.0};
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {50.0};
    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};
    GLfloat model_ambient[] = {0.5, 0.5, 0.5, 1.0};

    glClearColor(0.0, 0.0, 0.0, 0.0);

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, model_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);

    /*  create 4 display lists, each with a different quadric object.
     *  different drawing styles and surface normal specifications
     *  are demonstrated.
     */
    start_list = glGenLists(4);
    qobj = gluNewQuadric();
    gluQuadricCallback(
      qobj, GLU_ERROR, reinterpret_cast<_GLUfuncptr>(error_callback));

    gluQuadricDrawStyle(qobj, GLU_FILL);  /* smooth shaded */
    gluQuadricNormals(qobj, GLU_SMOOTH);  // one normal per vertex
    glNewList(start_list, GL_COMPILE);
    gluSphere(qobj, 0.75, 15, 10);
    glEndList();

    gluQuadricDrawStyle(qobj, GLU_FILL); /* flat shaded */
    gluQuadricNormals(qobj, GLU_FLAT);   // one normal per face
    glNewList(start_list + 1, GL_COMPILE);
    gluCylinder(qobj, 0.5, 0.3, 1.0, 15, 5);
    glEndList();

    gluQuadricDrawStyle(qobj, GLU_LINE); /* all polygons wireframe */
    gluQuadricNormals(qobj, GLU_NONE);   // no normal
    glNewList(start_list + 2, GL_COMPILE);
    gluDisk(qobj, 0.25, 1.0, 20, 4);
    glEndList();

    gluQuadricDrawStyle(qobj, GLU_SILHOUETTE); /* boundary only  */
    gluQuadricNormals(qobj, GLU_NONE);
    glNewList(start_list + 3, GL_COMPILE);
    gluPartialDisk(qobj, 0.0, 1.0, 20, 4, 0.0, 225.0);
    glEndList();
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    glTranslatef(-1.0, -1.0, 0.0);
    glCallList(start_list);

    glShadeModel(GL_FLAT);
    glTranslatef(0.0, 2.0, 0.0);
    glPushMatrix();
    glRotatef(300.0, 1.0, 0.0, 0.0);
    glCallList(start_list + 1);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(0.0, 1.0, 1.0);
    glTranslatef(2.0, -2.0, 0.0);
    glCallList(start_list + 2);

    glColor3f(1.0, 1.0, 0.0);
    glTranslatef(0.0, 2.0, 0.0);
    glCallList(start_list + 3);

    glPopMatrix();
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      glOrtho(-2.5, 2.5, -2.5 * (GLfloat)h / (GLfloat)w,
        2.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
      glOrtho(-2.5 * (GLfloat)w / (GLfloat)h, 2.5 * (GLfloat)w / (GLfloat)h,
        -2.5, 2.5, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
