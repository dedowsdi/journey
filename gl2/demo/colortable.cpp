/*
 *  colortable.c
 *  invert a passed block of pixels.  this program illustrates the
 *  use of the glColorTable() function.
 *
 */
#include "app.h"
#include "read_image.cpp"

extern GLubyte* read_image(const char*, GLsizei*, GLsizei*);

namespace zxd {

GLubyte* pixels;
GLsizei width, height;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "colortable";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = width;
    m_info.wnd_height = height;
  }

  void create_scene(void) {
    int i;
    GLubyte color_table[256][3];

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    /* set up an inverse color table for each channel */
    for (i = 0; i < 256; ++i) {
      color_table[i][0] = 255 - i;
      color_table[i][1] = 255 - i;
      color_table[i][2] = 255 - i;
    }

    /*glPixelTransferi(GL_MAP_COLOR, 1);*/

    // width be exponent of 2
    glColorTable(
      GL_COLOR_TABLE, GL_RGB, 256, GL_RGB, GL_UNSIGNED_BYTE, color_table);
    glEnable(GL_COLOR_TABLE);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(1, 1);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glFlush();
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
