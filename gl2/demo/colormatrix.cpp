/*
 *  colormatix.c
 *  this program uses the color matrix to exchange the color channels of
 *  an image.
 *
 *    red   -> green
 *    green -> blue
 *    blue  -> red
 *
 */
#include "app.h"
#include "readimage.cpp"

extern GLubyte* read_image(const char*, GLsizei*, GLsizei*);
namespace zxd {

GLubyte* pixels;
GLsizei width, height = 256;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "colormatrix";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = width;
    m_info.wnd_height = height;
  }

  void create_scene(void) {
    // clang-format off
   GLfloat  m[16] = {
      0.0, 1.0, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      1.0, 0.0, 0.0, 0.0,
      0.0, 0.0, 0.0, 1.0
   };
    // clang-format on

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glMatrixMode(GL_COLOR);
    glLoadMatrixf(m);
    glMatrixMode(GL_MODELVIEW);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(1, 1);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
    glFinish();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }

  /*  main loop
   *  open window with initial window size, title bar,
   *  RGBA display mode, and handle input events.
   */
};
}
int main(int argc, char** argv) {
  zxd::pixels = read_image("leeds.bin", &zxd::width, &zxd::height);
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
