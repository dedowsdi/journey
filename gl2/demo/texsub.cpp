/*  texsub.c
 *  this program texture maps a checkerboard image onto
 *  two rectangles.  this program clamps the texture, if
 *  the texture coordinates fall outside 0.0 and 1.0.
 */
#include "app.h"

namespace zxd {

/*  create checkerboard textures  */
#define check_image_width 64
#define check_image_height 64
#define sub_image_width 16
#define sub_image_height 16
GLubyte check_image[check_image_height][check_image_width][4];
GLubyte sub_image[sub_image_height][sub_image_width][4];

GLuint tex_name;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "texsub";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 250;
    m_info.wnd_height = 250;
  }

  void make_check_images(void) {
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
    for (i = 0; i < sub_image_height; i++) {
      for (j = 0; j < sub_image_width; j++) {
        c = ((((i & 0x4) == 0) ^ ((j & 0x4) == 0))) * 255;
        sub_image[i][j][0] = (GLubyte)c;
        sub_image[i][j][1] = (GLubyte)0;
        sub_image[i][j][2] = (GLubyte)0;
        sub_image[i][j][3] = (GLubyte)255;
      }
    }
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    make_check_images();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &tex_name);
    glBindTexture(GL_TEXTURE_2D, tex_name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, check_image_width,
      check_image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, check_image);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glBindTexture(GL_TEXTURE_2D, tex_name);

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

    glDisable(GL_TEXTURE_2D);

    glWindowPos2i(10, 230);
    const unsigned char info[] =
      "q : change sub image\n"
      "w : reset";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

    glFlush();
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
      case 'q':
      case 'Q':
        // change sub image
        glBindTexture(GL_TEXTURE_2D, tex_name);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 12, 44, sub_image_width,
          sub_image_height, GL_RGBA, GL_UNSIGNED_BYTE, sub_image);
        break;
      case 'w':
      case 'W':
        // restore entire image
        glBindTexture(GL_TEXTURE_2D, tex_name);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, check_image_width,
          check_image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, check_image);
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
