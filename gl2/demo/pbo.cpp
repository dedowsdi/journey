/*
 *  pboimage.c
 *  drawing, copying and zooming pixel data stored in a buffer object
 */
#include <app.h>

namespace zxd {

/*	create checkerboard image	*/
#define IMAGE_WIDTH 64
#define IMAGE_HEIGHT 64
GLubyte check_image[IMAGE_HEIGHT][IMAGE_WIDTH][3];

GLuint pixel_buffer;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "pboimage";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 300;
    m_info.wnd_height = 300;
  }

  void make_check_image(void) {
    int i, j, c;

    for (i = 0; i < IMAGE_HEIGHT; i++) {
      for (j = 0; j < IMAGE_WIDTH; j++) {
        c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
        check_image[i][j][0] = (GLubyte)c;
        check_image[i][j][1] = (GLubyte)c;
        check_image[i][j][2] = (GLubyte)c;
      }
    }
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    make_check_image();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenBuffers(1, &pixel_buffer);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixel_buffer);
    glBufferData(GL_PIXEL_UNPACK_BUFFER, 3 * IMAGE_WIDTH * IMAGE_HEIGHT,
      check_image, GL_STATIC_DRAW);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, 0);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pixel_buffer);
    // draw pixels from current pixel unpack buffer
    glDrawPixels(
      IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
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
