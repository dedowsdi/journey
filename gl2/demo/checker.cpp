/*  checker.c
 *  this program texture maps a checkerboard image onto
 *  two rectangles.
 *
 *  if running this program on open_g_l 1.0, texture objects are
 *  not used.
 *
 *  it's not garenteed that the requested internal format will be delivered by
 *  opengl, opengl is only obliged to choose an internal representation that
 *  closly approximates what is requested.
 *
 *  image width should be 2^m or 2^m + 2 if it has border
 */
#include "app.h"

namespace zxd {
/*	create checkerboard texture	*/
#define check_image_width 64
#define check_image_height 64
static GLubyte check_image[check_image_height][check_image_width][4];

#ifdef GL_VERSION_1_1
static GLuint tex_name;
#endif

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "checker";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 250;
    m_info.wnd_height = 250;
  }

  void make_check_image(void) {
    int i, j, c;

    for (i = 0; i < check_image_height; i++) {
      for (j = 0; j < check_image_width; j++) {
        c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
        check_image[i][j][0] = (GLubyte)c;
        check_image[i][j][1] = (GLubyte)c;
        check_image[i][j][2] = (GLubyte)c;
        check_image[i][j][3] = (GLubyte)255;
      }
    }
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    make_check_image();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef GL_VERSION_1_1
    glGenTextures(1, &tex_name);
    glBindTexture(GL_TEXTURE_2D, tex_name);
#endif

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#ifdef GL_VERSION_1_1
    // it's not garenteed that the requested internal format will be delivered
    // by
    // opengl, opengl is only obliged to choose an internal representation that
    // closly  approximates what is requested.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, check_image_width,
      check_image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, check_image);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, 4, check_image_width, check_image_height, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, check_image);
#endif
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
#ifdef GL_VERSION_1_1
    glBindTexture(GL_TEXTURE_2D, tex_name);
#endif

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-2.0, -1.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-2.0, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.0, 1.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.0, -1.0, 0.0);

    glTexCoord2f(0.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(2.41421, 1.0, -1.41421);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(2.41421, -1.0, -1.41421);
    glEnd();
    glFlush();
    glDisable(GL_TEXTURE_2D);
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 30.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -3.6);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
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
