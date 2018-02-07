/*
 *  blendeqn.c
 *  demonstrate the different blending functions available with the
 *  open_g_l imaging subset.  this program demonstrates use of the
 *  glBlendEquation() call.
 *
 *  blend_result = blend_equasion(src * src_factor, dest * dest_factor)
 *  set blend_equasion with glBlendEquasion
 *  set src_factor and dest_factor with glBlendFunc
 *  you can seperate color and alpha if you want.
 */

#include "app.h"

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "blendeqn";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }

  void create_scene(void) {
    glClearColor(1.0, 1.0, 0.0, 0.0);

    glBlendFunc(GL_ONE, GL_ONE);
    glEnable(GL_BLEND);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0, 0.0, 1.0);
    glRectf(-0.5, -0.5, 0.5, 0.5);

    glDisable(GL_BLEND);
    glColor3f(0.0f, 0.0f, 0.0f);
    glWindowPos2i(10, 492);
    const GLubyte info[] =
      "blend func : one one \n\n"
      "a : GL_FUNC_ADD \n"
      "s : GL_FUNC_SUBTRACT \n"
      "r : GL_FUNC_REVERSE_SUBTRACT \n"
      "m : GL_FUNC_MIN \n"
      "x: GL_FUNC_MAX \n";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);
    glEnable(GL_BLEND);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    GLdouble aspect = (GLdouble)w / h;

    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (aspect < 1.0) {
      aspect = 1.0 / aspect;
      glOrtho(-aspect, aspect, -1.0, 1.0, -1.0, 1.0);
    } else
      glOrtho(-1.0, 1.0, -aspect, aspect, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'a':
      case 'A':
        /* colors are added as: (1, 1, 0) + (0, 0, 1) = (1, 1, 1)
         *  which will produce a white square on a yellow background.
         */
        glBlendEquation(GL_FUNC_ADD);
        break;

      case 's':
      case 'S':
        /* colors are subtracted as: (0, 0, 1) - (1, 1, 0) = (-1, -1, 1)
	  *  which is clamped to (0, 0, 1), producing a blue square on a
	  *  yellow background
	  */ glBlendEquation(
          GL_FUNC_SUBTRACT);
        break;

      case 'r':
      case 'R':
        /* colors are subtracted as: (1, 1, 0) - (0, 0, 1) = (1, 1, -1)
         *  which is clamed to (1, 1, 0).  this produces yellow for both
         *  the square and the background.
         */
        glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
        break;

      case 'm':
      case 'M':
        /* the minimum of each component is computed, as
         *  [min(1, 0), min(1, 0), min(0, 1)] which equates to (0, 0, 0).
         *  this will produce a black square on the yellow background.
         */
        glBlendEquation(GL_MIN);
        break;

      case 'x':
      case 'X':
        /* the minimum of each component is computed, as
         *  [max(1, 0), max(1, 0), max(0, 1)] which equates to (1, 1, 1)
         *  this will produce a white square on the yellow background.
         */
        glBlendEquation(GL_MAX);
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
