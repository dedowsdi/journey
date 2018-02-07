/*
 * material.c
 * this program demonstrates the use of the GL lighting model.
 * several objects are drawn using different material characteristics.
 * A single light source illuminates the objects.
 */
#include "app.h"

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "material";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 600;
    m_info.wnd_height = 450;
  }

  /*  initialize z-buffer, projection matrix, light source,
   *  and lighting model.  do not specify a material property here.
   */
  void create_scene(void) {
    GLfloat ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat position[] = {0.0, 3.0, 2.0, 0.0};
    GLfloat lmodel_ambient[] = {0.4, 0.4, 0.4, 1.0};
    GLfloat local_view[] = {0.0};

    glClearColor(0.0, 0.1, 0.1, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
  }

  /*  draw twelve spheres in 3 rows with 4 columns.
   *  the spheres in the first row have materials with no ambient reflection.
   *  the second row has materials with significant ambient reflection.
   *  the third row has materials with colored ambient reflection.
   *
   *  the first column has materials with blue, diffuse reflection only.
   *  the second column has blue diffuse reflection, as well as specular
   *  reflection with a low shininess exponent.
   *  the third column has blue diffuse reflection, as well as specular
   *  reflection with a high shininess exponent (a more concentrated highlight).
   *  the fourth column has materials which also include an emissive component.
   *
   *  glTranslatef() is used to move spheres to their appropriate locations.
   *
   *  glMaterial has a performance cost, following code could be rewritten to
   *  minimize material-proterty chagne.
   */

  void display(void) {
    GLfloat no_mat[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat mat_ambient[] = {0.7, 0.7, 0.7, 1.0};
    GLfloat mat_ambient_color[] = {0.8, 0.8, 0.2, 1.0};
    GLfloat mat_diffuse[] = {0.1, 0.5, 0.8, 1.0};
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat no_shininess[] = {0.0};
    GLfloat low_shininess[] = {5.0};
    GLfloat high_shininess[] = {100.0};
    GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /*  draw sphere in first row, first column
     *  diffuse reflection only; no ambient or specular
     */
    glPushMatrix();
    glTranslatef(-3.75, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in first row, second column
     *  diffuse and specular reflection; low shininess; no ambient
     */
    glPushMatrix();
    glTranslatef(-1.25, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in first row, third column
     *  diffuse and specular reflection; high shininess; no ambient
     */
    glPushMatrix();
    glTranslatef(1.25, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in first row, fourth column
     *  diffuse reflection; emission; no ambient or specular reflection
     */
    glPushMatrix();
    glTranslatef(3.75, 3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, no_mat);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in second row, first column
     *  ambient and diffuse reflection; no specular
     */
    glPushMatrix();
    glTranslatef(-3.75, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in second row, second column
     *  ambient, diffuse and specular reflection; low shininess
     */
    glPushMatrix();
    glTranslatef(-1.25, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in second row, third column
     *  ambient, diffuse and specular reflection; high shininess
     */
    glPushMatrix();
    glTranslatef(1.25, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in second row, fourth column
     *  ambient and diffuse reflection; emission; no specular
     */
    glPushMatrix();
    glTranslatef(3.75, 0.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in third row, first column
     *  colored ambient and diffuse reflection; no specular
     */
    glPushMatrix();
    glTranslatef(-3.75, -3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in third row, second column
     *  colored ambient, diffuse and specular reflection; low shininess
     */
    glPushMatrix();
    glTranslatef(-1.25, -3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, low_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in third row, third column
     *  colored ambient, diffuse and specular reflection; high shininess
     */
    glPushMatrix();
    glTranslatef(1.25, -3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, high_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, no_mat);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    /*  draw sphere in third row, fourth column
     *  colored ambient and diffuse reflection; emission; no specular
     */
    glPushMatrix();
    glTranslatef(3.75, -3.0, 0.0);
    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
    glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glutSolidSphere(1.0, 16, 16);
    glPopMatrix();

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= (h * 2))
      glOrtho(-6.0, 6.0, -3.0 * ((GLfloat)h * 2) / (GLfloat)w,
        3.0 * ((GLfloat)h * 2) / (GLfloat)w, -10.0, 10.0);
    else
      glOrtho(-6.0 * (GLfloat)w / ((GLfloat)h * 2),
        6.0 * (GLfloat)w / ((GLfloat)h * 2), -3.0, 3.0, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) { app::keyboard(key, x, y); }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
