/*
 *  histogram.c
 *  compute the histogram of the image.  this program illustrates the
 *  use of the glHistogram() function.
 */

#include <app.h>
#include "readimage.cpp"

extern GLubyte* read_image(const char*, GLsizei*, GLsizei*);

namespace zxd {
#define HISTOGRAM_SIZE 256 /* must be a power of 2 */

GLubyte* pixels;
GLsizei width, height;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "histogram";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = width;
    m_info.wnd_height = height;
  }

  void create_scene(void) {
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glHistogram(GL_HISTOGRAM, HISTOGRAM_SIZE, GL_RGB, GL_FALSE);
    glEnable(GL_HISTOGRAM);
  }

  void display(void) {
    int i;
    GLushort values[HISTOGRAM_SIZE][3];

    glClear(GL_COLOR_BUFFER_BIT);
    glRasterPos2i(1, 1);
    glDrawPixels(width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    // get histogram, reset inner table. you can also use glResetHistogram to
    // reset
    // iner table
    glGetHistogram(GL_HISTOGRAM, GL_TRUE, GL_RGB, GL_UNSIGNED_SHORT, values);

    /* plot histogram */

    glBegin(GL_LINE_STRIP);
    glColor3f(1.0, 0.0, 0.0);
    for (i = 0; i < HISTOGRAM_SIZE; i++) glVertex2s(i, values[i][0]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 1.0, 0.0);
    for (i = 0; i < HISTOGRAM_SIZE; i++) glVertex2s(i, values[i][1]);
    glEnd();

    glBegin(GL_LINE_STRIP);
    glColor3f(0.0, 0.0, 1.0);
    for (i = 0; i < HISTOGRAM_SIZE; i++) glVertex2s(i, values[i][2]);
    glEnd();
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 256, 0, 10000, -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    static GLboolean sink = GL_FALSE;

    switch (key) {
      case 's':
        sink = !sink;
        glHistogram(GL_HISTOGRAM, HISTOGRAM_SIZE, GL_RGB, sink);
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
