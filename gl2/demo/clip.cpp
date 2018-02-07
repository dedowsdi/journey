/*
 *  clip.c
 *  this program demonstrates arbitrary clipping planes.
 *
 *  plane is constructed in model space, it will be transformed by inverse
 *  transpose of model_view and stored in eye space.
 *
 *  any vertex in negative side of plane will be out
 *
 *    tP*v >= 0  in
 *    tP*v < 0   out
 *
 */
#include "app.h"

namespace zxd {

double yaw = 0.0f;
double pitch = 0.0f;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "clip";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
  }

  void display(void) {
    GLdouble eqn[4] = {0.0, 1.0, 0.0, 0.0};   // clip out down part
    GLdouble eqn2[4] = {1.0, 0.0, 0.0, 0.0};  // clip out left part

    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();

    // in this demo we create clip planes under the node of the sphere, so the
    // planes passes center of the sphere.

    glRotatef(pitch, 1, 0, 0);
    glClipPlane(GL_CLIP_PLANE0, eqn);
    glEnable(GL_CLIP_PLANE0);

    glPopMatrix();  // cancel rotation
    glPushMatrix();

    glRotatef(yaw, 0, 1, 0);
    glClipPlane(GL_CLIP_PLANE1, eqn2);
    glEnable(GL_CLIP_PLANE1);

    glPopMatrix();  // cancel rotation

    // draw scene
    glColor3f(1.0, 1.0, 1.0);
    glutWireSphere(1.0, 20, 16);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -5.0);
    glRotatef(90.0, 1.0, 0.0, 0.0);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
  }

  void special(int key, int x, int y) {
    switch (key) {
      case GLUT_KEY_UP:
        pitch += 10;
        break;
      case GLUT_KEY_DOWN:
        pitch -= 10;
        break;
      case GLUT_KEY_LEFT:
        yaw -= 10;
        break;
      case GLUT_KEY_RIGHT:
        yaw += 10;
        break;
    }
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
