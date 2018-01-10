/*
 *  drawf.c
 *  Draws the bitmapped letter F on the screen (several times).
 *  This demonstrates use of the glBitmap() call.
 *
 *  Bitmap data is always stored in chunks that are multiples of 8 bits, but the
 *  width of the actual bitmap doesn't have to be a multiple of 8. Just make
 *  sure width is large enough to include the valid bits.
 *
 *  You can't rotate bitmap fonts because the bitmap is always drawn aligned to
 *  the x and y framebuffer axes. Additionally, bitmaps can't be zoomed.
 *
 *  Bitmap works on pixels, it doesn't concern modelView matrix. 
 *
 *  glRastePos works in model coordinates, it will be transfromed by geometric
 *  processing line. glWindowPos works in window coordinates directly.
 *
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include "common.h"

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

//bitmap is created from bottom to top. be careful about this.
GLubyte rasters[24] = {
  0xc0, 0x00,   //bottom
  0xc0, 0x00,
  0xc0, 0x00,
  0xc0, 0x00,
  0xc0, 0x00,
  0xff, 0x00,
  0xff, 0x00,
  0xc0, 0x00,
  0xc0, 0x00,
  0xc0, 0x00,
  0xff, 0xc0,
  0xff, 0xc0    //top
};
// clang-format on

void init(void) {
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
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0, w, 0, h, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
  }
}

/*  Main Loop
 *  Open window with initial window size, title bar,
 *  RGBA display mode, and handle input events.
 */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(100, 100);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadGL();
  init();
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutDisplayFunc(display);
  glutMainLoop();
  return 0;
}
