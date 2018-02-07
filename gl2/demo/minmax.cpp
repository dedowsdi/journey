/*
 *  minmax.c
 *  determine the minimum and maximum values of a group of pixels.
 *  this demonstrates use of the glMinmax() call.
 */
#include "app.h"
#include "readimage.cpp"
extern GLubyte* read_image(const char*, GLsizei*, GLsizei*);

namespace zxd {

GLubyte* pixels;
GLsizei width, height;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "minmax";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = width;
    m_info.wnd_height = height;
  }

  void create_scene(void) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glMinmax(GL_MINMAX, GL_RGB, GL_FALSE);
    glEnable(GL_MINMAX);
  }

  void display(void) {
    GLubyte values[6];

    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(1, 1);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glFlush();

    glGetMinmax(GL_MINMAX, GL_TRUE, GL_RGB, GL_UNSIGNED_BYTE, values);
    printf(" red   : min = %d   max = %d\n", values[0], values[3]);
    printf(" green : min = %d   max = %d\n", values[1], values[4]);
    printf(" blue  : min = %d   max = %d\n", values[2], values[5]);
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
  }

  void keyboard(unsigned char key, int x, int y) { app::keyboard(key, x, y); }

  /*  main loop
   *  open window with initial window size, title bar,
   *  RGBA display mode, and handle input events.
   */
};
}
int main(int argc, char** argv) {
  zxd::pixels = read_image("data/leeds.bin", &zxd::width, &zxd::height);
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
