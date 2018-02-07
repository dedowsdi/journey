/*  image.c
 *  this program demonstrates drawing pixels and shows the effect
 *  of glDrawPixels(), glCopyPixels(), and glPixelZoom().
 *  interaction: moving the mouse while pressing the mouse button
 *  will copy the image in the lower-left corner of the window
 *  to the mouse position, using the current pixel zoom factors.
 *  there is no attempt to prevent you from drawing over the original
 *  image.  if you press the 'r' key, the original image and zoom
 *  factors are reset.  if you press the 'z' or 'Z' keys, you change
 *  the zoom factors.
 *
 *  if you only want to draw subrect of a image, use glPixlelStore with
 *  GL_UNPACK_SKIP_ROWS, GL_UNPACK_SKIP_PIXELS to set up left bottom origin, use
 *  glPIxelStore with GL_UNPACK_ROW_LENGTH to set subrect width, set height in
 *  glDrawPixel, ROW_LENGTH should be the same as the width you use in
 *  glDrawPixel
 *
 */
#include "app.h"

namespace zxd {

/*	create checkerboard image	*/
#define IMAGE_WIDTH 64
#define IMAGE_HEIGHT 64
GLubyte check_image[IMAGE_HEIGHT][IMAGE_WIDTH][3];

GLdouble zoom_factor = 1.0;
GLint height;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "image";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 250;
    m_info.wnd_height = 250;
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
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(0, 0);
    glDrawPixels(
      IMAGE_WIDTH, IMAGE_HEIGHT, GL_RGB, GL_UNSIGNED_BYTE, check_image);
    glWindowPos2i(10, 230);
    const unsigned char info[] =
      "qQ : zoom factor\n"
      "r : reset zoom factor to 1\n"
      "drag : copy pixels";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    height = (GLint)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void motion(int x, int y) {
    static GLint screeny;

    screeny = height - (GLint)y - 1;
    glRasterPos2i(x, screeny);
    glPixelZoom(zoom_factor, zoom_factor);
    glCopyPixels(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_COLOR);
    glPixelZoom(1.0, 1.0);
    glFlush();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'r':
      case 'R':
        zoom_factor = 1.0;
        printf("zoom_factor reset to 1.0\n");
        break;
      case 'q':
        zoom_factor += 0.5;
        if (zoom_factor >= 3.0) zoom_factor = 3.0;
        printf("zoom_factor is now %4.1f\n", zoom_factor);
        break;
      case 'Q':
        zoom_factor -= 0.5;
        if (zoom_factor <= 0.5) zoom_factor = 0.5;
        printf("zoom_factor is now %4.1f\n", zoom_factor);
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
