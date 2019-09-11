/*
 * robot.c
 * this program shows how to composite modeling transformations
 * to draw translated and rotated hierarchical models.
 * interaction:  pressing the s and e keys (shoulder and elbow)
 * alters the rotation of the robot arm.
 */
#include <app.h>

namespace zxd {

// rotation angles
static int shoulder = 0, elbow = 0, index_finger0 = 0, index_finger1 = 0,
           middle_finger0 = 0, middle_finger1 = 0, ring_finger0 = 0,
           ring_finger1 = 0, thumb0 = 0, thumb1;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "robot";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    // enable line smooth and blend to handle antialiasing
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1.5);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  }

  // draw a cube with left face centered at current node
  void draw_segment(int roll, double width, double height, double depth) {
    glRotatef((GLfloat)roll, 0.0, 0.0, 1.0);
    glTranslatef(width / 2, 0.0, 0.0);

    glPushMatrix();
    glScalef(width, height, depth);
    glutWireCube(1.0);
    glPopMatrix();
  }

  void draw_finger(
    int roll0, int roll1, double width, double height, double depth) {
    draw_segment(roll0, width, height, depth);
    glTranslatef(width * 0.5, 0.0f, 0.0f);
    draw_segment(roll1, width, height, depth);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();

    GLdouble sw = 2.0, sh = 0.4, sd = 0.5;  // shoulder width height depth
    GLdouble ew = 2.0, eh = 0.4, ed = 0.5;  // elbow width height depth
    GLdouble fw = 0.4, fh = 0.1, fd = 0.1;  // figner width height depth
    // half
    GLdouble hsw = 0.5 * sw, hew = 0.5 * ew, heh = 0.5 * eh, hed = 0.5 * ed,
             hfw = 0.5 * fw, hfh = 0.5 * fh, hfd = 0.5 * fd;

    // shoulder
    glTranslatef(-hsw, 0.0, 0.0);
    draw_segment(shoulder, sw, sh, sd);

    // elbow
    glTranslatef(hsw, 0.0, 0.0);
    draw_segment(elbow, ew, eh, ed);

    glTranslatef(hew, 0.0, 0.0);

    // index finger
    glPushMatrix();
    glTranslatef(0, heh - hfh, hed - hfd);
    draw_finger(index_finger0, index_finger1, fw, fh, fd);
    glPopMatrix();

    // middle finger
    glPushMatrix();
    glTranslatef(0, heh - hfh, 0);
    draw_finger(middle_finger0, middle_finger1, fw, fh, fd);
    glPopMatrix();

    // ring finger
    glPushMatrix();
    glTranslatef(0, heh - hfh, -hed + hfd);
    draw_finger(ring_finger0, ring_finger1, fw, fh, fd);
    glPopMatrix();

    // thumb
    glPushMatrix();
    glTranslatef(0, -heh + hfh, hed - hfd);
    draw_finger(thumb0, thumb1, fw, fh, fd);
    glPopMatrix();

    glPopMatrix();

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    const GLubyte info[] =
      "qQ : shoulder \n"
      "wW : albow \n"
      "yY : thumb0 \n"
      "hY : thumb1 \n"
      "uU : index0 \n"
      "jJ : index1 \n"
      "iI : middle0 \n"
      "kK : middle1 \n"
      "oO : ring0 \n"
      "lL : ring1 \n";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(65.0, (GLfloat)w / (GLfloat)h, 1.0, 20.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -7.0);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        shoulder = (shoulder + 5) % 360;
        break;
      case 'Q':
        shoulder = (shoulder - 5) % 360;
        break;
      case 'w':
        elbow = (elbow + 5) % 360;
        break;
      case 'W':
        elbow = (elbow - 5) % 360;
        break;
      case 'y':
        thumb0 = (thumb0 + 5) % 360;
        break;
      case 'Y':
        thumb0 = (thumb0 - 5) % 360;
        break;
      case 'h':
        thumb1 = (thumb1 + 5) % 360;
        break;
      case 'H':
        thumb1 = (thumb1 - 5) % 360;
        break;
      case 'u':
        index_finger0 = (index_finger0 + 5) % 360;
        break;
      case 'U':
        index_finger0 = (index_finger0 - 5) % 360;
        break;
      case 'j':
        index_finger1 = (index_finger1 + 5) % 360;
        break;
      case 'J':
        index_finger1 = (index_finger1 - 5) % 360;
        break;
      case 'i':
        middle_finger0 = (middle_finger0 + 5) % 360;
        break;
      case 'I':
        middle_finger0 = (middle_finger0 - 5) % 360;
        break;
      case 'k':
        middle_finger1 = (middle_finger1 + 5) % 360;
        break;
      case 'K':
        middle_finger1 = (middle_finger1 - 5) % 360;
        break;
      case 'o':
        ring_finger0 = (ring_finger0 + 5) % 360;
        break;
      case 'O':
        ring_finger0 = (ring_finger0 - 5) % 360;
        break;
      case 'l':
        ring_finger1 = (ring_finger1 + 5) % 360;
        break;
      case 'L':
        ring_finger1 = (ring_finger1 - 5) % 360;
        break;
      default:
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
