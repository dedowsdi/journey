/*
 *  convolution.c
 *  use various 2D convolutions filters to find edges in an image.
 *
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
    m_info.title = "convolution";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGBA;
    m_info.wnd_width = width;
    m_info.wnd_height = height;
  }

  // clang-format off

GLfloat horizontal[3][3] = {
  {0, -1, 0}, 
  {0, 1, 0},
  {0, 0, 0}
};

GLfloat vertical[3][3] = {
  {0, 0, 0},
  {-1, 1, 0}, 
  {0, 0, 0}
};

GLfloat laplacian[3][3] = {
  {-0.125, -0.125, -0.125},
  {-0.125, 1.0, -0.125},
  {-0.125, -0.125, -0.125},
};

  // clang-format on

  void create_scene(void) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glClearColor(0.0, 0.0, 0.0, 0.0);

    printf("using the horizontal filter\n");
    glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, 3, 3, GL_LUMINANCE,
      GL_FLOAT, horizontal);
    glEnable(GL_CONVOLUTION_2D);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(1, 1);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    const unsigned char info[] =
      "q : horizontal filter\n"
      "w : vertical filter\n"
      "e : laplacian filter";
    glWindowPos2i(10, height - 20);
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

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
    switch (key) {
      case 'q':
        printf("using a horizontal filter\n");
        glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, 3, 3,
          GL_LUMINANCE, GL_FLOAT, horizontal);
        break;

      case 'w':
        printf("using the vertical filter\n");
        glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, 3, 3,
          GL_LUMINANCE, GL_FLOAT, vertical);
        break;

      case 'e':
        printf("using the laplacian filter\n");
        glConvolutionFilter2D(GL_CONVOLUTION_2D, GL_LUMINANCE, 3, 3,
          GL_LUMINANCE, GL_FLOAT, laplacian);
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
  zxd::pixels = read_image("leeds.bin", &zxd::width, &zxd::height);
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
