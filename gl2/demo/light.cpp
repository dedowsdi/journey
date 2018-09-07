/*
 *  light.c
 *  this program demonstrates the use of the opengl lighting
 *  model.  A sphere is drawn using a grey material characteristic.
 *  A single light source illuminates the object.
 */
#include "app.h"

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "light";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  /*  initialize material property, light source, lighting model,
   *  and depth buffer.
   */
  void create_scene(void) {
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {50.0};

    GLfloat light_position0[] = {1.0, 0.0, 0.0, 0.0};   // 0 w means dir light
    GLfloat light_position1[] = {-2.0, 0.0, 0.0, 1.0};  // point
    GLfloat light_position2[] = {0.0, 2.0, 0.0, 1.0};   // spot
    GLfloat light_diffuse0[] = {1.0, 0.0, 0.0, 1.0};
    GLfloat light_diffuse1[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat light_diffuse2[] = {0.0, 0.0, 1.0, 1.0};
    GLfloat light_specular0[] = {1.0, 0.0, 0.0, 1.0};
    GLfloat light_specular1[] = {0.0, 1.0, 0.0, 1.0};
    GLfloat light_specular2[] = {0.0, 0.0, 1.0, 1.0};
    GLfloat light_dir2[] = {0, -1, 0};
    GLfloat light_cutoff2 = 20;

    GLfloat lmodel_ambient[] = {0.1, 0.1, 0.1, 1.0};  // global ambient

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
    glLightfv(GL_LIGHT2, GL_POSITION, light_position2);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, light_diffuse2);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
    glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
    glLightfv(GL_LIGHT2, GL_SPECULAR, light_specular2);
    glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, light_dir2);
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, light_cutoff2);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_LIGHT2);
    glEnable(GL_DEPTH_TEST);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glutSolidSphere(1.0, 64, 32);
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      glOrtho(-1.5, 1.5, -1.5 * (GLfloat)h / (GLfloat)w,
        1.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
      glOrtho(-1.5 * (GLfloat)w / (GLfloat)h, 1.5 * (GLfloat)w / (GLfloat)h,
        -1.5, 1.5, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
