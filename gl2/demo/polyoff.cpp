/*
 *  polyoff.c
 *  this program demonstrates polygon offset to draw a shaded
 *  polygon and its wireframe counterpart without ugly visual
 *  artifacts ("stitching").
 *
 *  polygon offset only works for polygon. you can render polygon in fill, line,
 *  or point polygon mode, and enable GL_POLYGON_OFFSET_FILL,
 *  GL_POLYGON_OFFSET_LINE or GL_POLYGON_OFFSET_POINT
 */
#include "app.h"

namespace zxd {

GLuint list;
GLint spinx = 0;
GLint spiny = 0;
GLfloat tdist = 0.0;
GLfloat polyfactor = 1.0;
GLfloat polyunits = 1.0;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "polyoff";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  /*  display() draws two spheres, one with a gray, diffuse material, pushed
   * away
   *  from camera by polygon offset, the other sphere with a magenta material
   * with
   *  a specular highlight.
   */
  void display(void) {
    GLfloat gray[] = {0.8, 0.8, 0.8, 1.0};
    GLfloat black[] = {0.0, 0.0, 0.0, 1.0};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(0.0, 0.0, tdist);
    glRotatef((GLfloat)spinx, 1.0, 0.0, 0.0);
    glRotatef((GLfloat)spiny, 0.0, 1.0, 0.0);

    // push away the solid one from camera by polyogn offset
    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, gray);
    glMaterialfv(GL_FRONT, GL_SPECULAR, black);
    glMaterialf(GL_FRONT, GL_SHININESS, 0.0);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(polyfactor, polyunits);
    glCallList(list);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LIGHTING);
    glDisable(GL_LIGHT0);
    glColor3f(1.0, 1.0, 1.0);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glCallList(list);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glPopMatrix();

    const unsigned char info[] =
      "q : spin along world x \n"
      "w : spin along local y  \n"
      "eE : chagne object z\n"
      "jJ : change polygon_offset factor\n"
      "kK : change chagne polygon_offset unit\n";

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

    glMatrixMode(GL_MODELVIEW);

    glFlush();
  }

  /*  specify initial properties
   *  create display list with sphere
   *  initialize lighting and depth buffer
   */
  void create_scene(void) {
    GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

    GLfloat global_ambient[] = {0.2, 0.2, 0.2, 1.0};

    glClearColor(0.0, 0.0, 0.0, 1.0);

    list = glGenLists(1);
    glNewList(list, GL_COMPILE);
    glutSolidSphere(1.0, 20, 12);
    glEndList();

    glEnable(GL_DEPTH_TEST);

    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_ambient);
  }

  /*  call when window is resized  */
  void reshape(int width, int height) {
    app::reshape(width, height);
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLdouble)width / (GLdouble)height, 1.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
  }

  /*  call when mouse button is pressed  */
  void mouse(int button, int state, int x, int y) {
    switch (button) {
      default:
        break;
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        spinx = (spinx + 5) % 360;
        break;
      case 'w':
        spiny = (spiny + 5) % 360;
        break;
      case 'e':
        if (tdist < 4.0) {
          tdist = (tdist + 0.5);
        }
        break;
      case 'E':
        if (tdist > -5.0) {
          tdist = (tdist - 0.5);
        }
        break;
      case 'j':
        polyfactor = polyfactor + 0.1;
        printf("polyfactor is %f\n", polyfactor);
        break;
      case 'J':
        polyfactor = polyfactor - 0.1;
        printf("polyfactor is %f\n", polyfactor);
        break;
      case 'k':
        polyunits = polyunits + 1.0;
        printf("polyunits is %f\n", polyunits);
        break;
      case 'K':
        polyunits = polyunits - 1.0;
        printf("polyunits is %f\n", polyunits);
        break;
      default:
        break;
    }
  }

  /*  main loop
   *  open window with initial window size, title bar,
   *  RGBA display mode, and handle input events.
   */
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
