/*
 *  drawf.c
 *  draws the bitmapped letter F on the screen (several times).
 *  this demonstrates use of the glBitmap() call.
 *
 *  bitmap data is always stored in chunks that are multiples of 8 bits, but the
 *  width of the actual bitmap doesn't have to be a multiple of 8. just make
 *  sure width is large enough to include the valid bits.
 *
 *  you can't rotate bitmap fonts because the bitmap is always drawn aligned to
 *  the x and y framebuffer axes. additionally, bitmaps can't be zoomed.
 *
 *  bitmap works on pixels, it doesn't concern model_view matrix.
 *
 *  glRastePos works in model coordinates, it will be transfromed by geometric
 *  processing line. glWindowPos works in window coordinates directly.
 *
 */
#include "app.h"

namespace zxd {

// clang-format off

// bitmap mask of f

//     * * * * * * * * * * 0 0 0 0 0 0
//     * * * * * * * * * * 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//     * * * * * * * * 0 0 0 0 0 0 0 0
//     * * * * * * * * 0 0 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0
//     * * 0 0 0 0 0 0 0 0 0 0 0 0 0 0

// bitmap is created from bottom to top. be careful about this.
GLubyte rasters[24] = {
  0xc0, 0x00,  // bottom
  0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xc0, 0x00, 0xff, 0x00, 0xff, 0x00, 0xc0,
  0x00, 0xc0, 0x00, 0xc0, 0x00, 0xff, 0xc0, 0xff, 0xc0  // top
};
// clang-format on

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "drawf";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 100;
    m_info.wnd_height = 100;
  }

  void create_scene(void) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 0.0);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glRasterPos2i(20, 20);
    /*glWindowPos2i(20, 20);*/
    glBitmap(10, 12, 0.0, 0.0, 11.0, 0.0, rasters);
    glBitmap(10, 12, 0.0, 0.0, 11.0, 0.0, rasters);
    glBitmap(10, 12, 0.0, 0.0, 11.0, 0.0, rasters);
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

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
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
