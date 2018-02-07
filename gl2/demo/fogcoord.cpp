/*
 *  fogcoord.c
 *
 *  this program demonstrates the use of explicit fog
 *  coordinates.  you can press the keyboard and change
 *  the fog coordinate value at any vertex.  you can
 *  also switch between using explicit fog coordinates
 *  and the default fog generation mode.
 *
 *  pressing the 'f' and 'b' keys move the viewer forward
 *  and backwards.
 *  pressing 'c' initiates the default fog generation.
 *  pressing capital 'C' restores explicit fog coordinates.
 *  pressing '1', '2', '3', '8', '9', and '0' add or
 *  subtract from the fog coordinate values at one of the
 *  three vertices of the triangle.
 */
#include "app.h"

namespace zxd{

GLfloat f1, f2, f3;

class app0 : public app{
void init_info(){
  app::init_info();
  m_info.title = "fogcoord";
  m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
  m_info.wnd_width = 500;
  m_info.wnd_height =  500;
}


/*  initialize fog
 */
void create_scene(void) {
  GLfloat fog_color[4] = {0.0, 0.25, 0.25, 1.0};
  f1 = 1.0f;
  f2 = 5.0f;
  f3 = 10.0f;

  glEnable(GL_FOG);
  glFogi(GL_FOG_MODE, GL_EXP);
  glFogfv(GL_FOG_COLOR, fog_color);
  glFogf(GL_FOG_DENSITY, 0.25);
  glHint(GL_FOG_HINT, GL_DONT_CARE);
  glFogi(GL_FOG_COORDINATE_SOURCE, GL_FOG_COORDINATE);
  glClearColor(0.0, 0.25, 0.25, 1.0); /* fog color */
}

/* display() draws a triangle at an angle.
 */
void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glColor3f(1.0f, 0.75f, 0.0f);
  glBegin(GL_TRIANGLES);
  glFogCoordf(f1);
  glVertex3f(2.0f, -2.0f, 0.0f);
  glFogCoordf(f2);
  glVertex3f(-2.0f, 0.0f, -5.0f);
  glFogCoordf(f3);
  glVertex3f(0.0f, 2.0f, -10.0f);
  glEnd();

  glDisable(GL_FOG);
  glColor3f(1.0,1.0,1.0);
  const unsigned char info[] =
    "q : toggle GL_FOG_COORDINATE_SOURCE between\n"
    "    fragment_depth and coordinate_ext\n"
    "wW : change fog coordinate of vertex 0\n"
    "eE : change fog coordinate of vertex 1\n"
    "rR : change fog coordinate of vertex 2\n"
    "jJ : change camera distance\n";
  glWindowPos2i(10, 480);
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);
  glEnable(GL_FOG);

  glutSwapBuffers();
}

void reshape(int w, int h) {
  app::reshape(w, h);
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45.0, 1.0, 0.25, 25.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0.0, 0.0, -5.0);
}

void keyboard(unsigned char key, int x, int y) {
  app::keyboard(key, x, y);
  switch (key) {
    case 'q': {
      int i;
      glGetIntegerv(GL_FOG_COORD_SRC, &i);
      if (i == GL_FRAGMENT_DEPTH)
        glFogi(GL_FOG_COORDINATE_SOURCE, GL_FOG_COORDINATE);
      else
        glFogi(GL_FOG_COORDINATE_SOURCE, GL_FRAGMENT_DEPTH);
    }
      break;
    case 'w':
      f1 = f1 + 0.25;
      break;
    case 'e':
      f2 = f2 + 0.25;
      break;
    case 'r':
      f3 = f3 + 0.25;
      break;
    case 'W':
      if (f1 > 0.25) {
        f1 = f1 - 0.25;
      }
      break;
    case 'E':
      if (f2 > 0.25) {
        f2 = f2 - 0.25;
      }
      break;
    case 'R':
      if (f3 > 0.25) {
        f3 = f3 - 0.25;
      }
      break;
    case 'j':
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(0.0, 0.0, -0.25);
      break;
    case 'J':
      glMatrixMode(GL_MODELVIEW);
      glTranslatef(0.0, 0.0, 0.25);
      break;
    default:
      break;
  }
}

/*  main loop
 *  open window with initial window size, title bar,
 *  RGBA display mode, depth buffer, and handle input events.
 */
};
}
int main(int argc, char** argv) {
zxd::app0 _app0;
_app0.run(argc, argv);
return 0;
}
