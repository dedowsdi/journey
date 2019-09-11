/*
 *  dof.c
 *  this program demonstrates use of the accumulation buffer to
 *  create an out-of-focus depth-of-field effect.  the teapots
 *  are drawn several times into the accumulation buffer.  the
 *  viewing volume is jittered, except at the focal point, where
 *  the viewing volume is at the same position, each time.  in
 *  this case, the gold teapot remains in focus.
 */
#include <app.h>
#include "jitter.h"

namespace zxd {

#define PI_ 3.14159265358979323846

GLuint teapot_list;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "dof";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_ACCUM | GLUT_DEPTH;
    m_info.wnd_width = 400;
    m_info.wnd_height = 400;
  }

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
    GLdouble z_near, GLdouble z_far, GLdouble pixdx, GLdouble pixdy,
    GLdouble eyedx, GLdouble eyedy, GLdouble focus) {
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

  /*  acc_perspective()
   *
   *  the first 4 arguments are identical to the gluPerspective() call.
   *  pixdx and pixdy are anti-alias jitter in pixels.
   *  set both equal to 0.0 for no anti-alias jitter.
   *  eyedx and eyedy are depth-of field jitter in pixels.
   *  set both equal to 0.0 for no depth of field effects.
   *
   *  focus is distance from eye to plane in focus.
   *  focus must be greater than, but not equal to 0.0.
   *
   *  note that acc_perspective() calls acc_frustum().
   */
  void acc_perspective(GLdouble fovy, GLdouble aspect, GLdouble z_near,
    GLdouble z_far, GLdouble pixdx, GLdouble pixdy, GLdouble eyedx,
    GLdouble eyedy, GLdouble focus) {
    GLdouble fov2, left, right, bottom, top;

    fov2 = ((fovy * PI_) / 180.0) / 2.0;

    top = z_near / (cos(fov2) / sin(fov2));
    bottom = -top;

    right = top * aspect;
    left = -right;

    acc_frustum(left, right, bottom, top, z_near, z_far, pixdx, pixdy, eyedx,
      eyedy, focus);
  }

  void create_scene(void) {
    GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat position[] = {0.0, 3.0, 3.0, 0.0};

    GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat local_view[] = {0.0};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

    glFrontFace(GL_CW);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearAccum(0.0, 0.0, 0.0, 0.0);
    /*  make teapot display list */
    teapot_list = glGenLists(1);
    glNewList(teapot_list, GL_COMPILE);
    glutSolidTeapot(0.5);
    glEndList();
  }

  void render_teapot(GLfloat x, GLfloat y, GLfloat z, GLfloat ambr,
    GLfloat ambg, GLfloat ambb, GLfloat difr, GLfloat difg, GLfloat difb,
    GLfloat specr, GLfloat specg, GLfloat specb, GLfloat shine) {
    GLfloat mat[4];

    glPushMatrix();
    glTranslatef(x, y, z);
    mat[0] = ambr;
    mat[1] = ambg;
    mat[2] = ambb;
    mat[3] = 1.0;
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
    mat[0] = difr;
    mat[1] = difg;
    mat[2] = difb;
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
    mat[0] = specr;
    mat[1] = specg;
    mat[2] = specb;
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
    glMaterialf(GL_FRONT, GL_SHININESS, shine * 128.0);
    glCallList(teapot_list);
    glPopMatrix();
  }

  /*  display() draws 5 teapots into the accumulation buffer
   *  several times; each time with a jittered perspective.
   *  the focal point is at z = 5.0, so the gold teapot will
   *  stay in focus.  the amount of jitter is adjusted by the
   *  magnitude of the acc_perspective() jitter; in this example, 0.33.
   *  in this example, the teapots are drawn 8 times.  see jitter.h
   */
  void display(void) {
    int jitter;
    GLint viewport[4];

    glGetIntegerv(GL_VIEWPORT, viewport);
    glClear(GL_ACCUM_BUFFER_BIT);

    for (jitter = 0; jitter < 8; jitter++) {
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      acc_perspective(45.0, (GLdouble)viewport[2] / (GLdouble)viewport[3], 1.0,
        15.0, 0.0, 0.0, 0.33 * j8[jitter].x, 0.33 * j8[jitter].y, 5.0);

      /*	ruby, gold, silver, emerald, and cyan teapots	*/
      render_teapot(-1.1, -0.5, -4.5, 0.1745, 0.01175, 0.01175, 0.61424,
        0.04136, 0.04136, 0.727811, 0.626959, 0.626959, 0.6);
      render_teapot(-0.5, -0.5, -5.0, 0.24725, 0.1995, 0.0745, 0.75164, 0.60648,
        0.22648, 0.628281, 0.555802, 0.366065, 0.4);
      render_teapot(0.2, -0.5, -5.5, 0.19225, 0.19225, 0.19225, 0.50754,
        0.50754, 0.50754, 0.508273, 0.508273, 0.508273, 0.4);
      render_teapot(1.0, -0.5, -6.0, 0.0215, 0.1745, 0.0215, 0.07568, 0.61424,
        0.07568, 0.633, 0.727811, 0.633, 0.6);
      render_teapot(1.8, -0.5, -6.5, 0.0, 0.1, 0.06, 0.0, 0.50980392,
        0.50980392, 0.50196078, 0.50196078, 0.50196078, .25);
      glAccum(GL_ACCUM, 0.125);
    }
    glAccum(GL_RETURN, 1.0);
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }

  /*  main loop
   *  be certain you request an accumulation buffer.
   */
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
