/*  texbind.c
 *  this program demonstrates using glBindTexture() by
 *  creating and managing two textures.
 */
#include "app.h"

namespace zxd {

/*	create checkerboard texture	*/
#define IMAGE_WIDTH 64
#define IMAGE_HEIGHT 64
GLubyte check_image[IMAGE_HEIGHT][IMAGE_WIDTH][4];
GLubyte other_image[IMAGE_HEIGHT][IMAGE_WIDTH][4];

GLuint tex_name[2];

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "texbind";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 250;
    m_info.wnd_height = 250;
  }

  void make_check_images(void) {
    int i, j, c;

    for (i = 0; i < IMAGE_HEIGHT; i++) {
      for (j = 0; j < IMAGE_WIDTH; j++) {
        c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
        check_image[i][j][0] = (GLubyte)c;
        check_image[i][j][1] = (GLubyte)c;
        check_image[i][j][2] = (GLubyte)c;
        check_image[i][j][3] = (GLubyte)255;
        c = ((((i & 0x10) == 0) ^ ((j & 0x10) == 0))) * 255;
        other_image[i][j][0] = (GLubyte)c;
        other_image[i][j][1] = (GLubyte)0;
        other_image[i][j][2] = (GLubyte)0;
        other_image[i][j][3] = (GLubyte)255;
      }
    }
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    make_check_images();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(2, tex_name);
    glBindTexture(GL_TEXTURE_2D, tex_name[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, check_image);

    glBindTexture(GL_TEXTURE_2D, tex_name[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, other_image);
    glEnable(GL_TEXTURE_2D);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBindTexture(GL_TEXTURE_2D, tex_name[0]);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-2.0, -1.0, 0.0);
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-2.0, 1.0, 0.0);
    glTexCoord2f(1.0, 1.0);
    glVertex3f(0.0, 1.0, 0.0);
    glTexCoord2f(1.0, 0.0);
    glVertex3f(0.0, -1.0, 0.0);
    glEnd();
    glBindTexture(GL_TEXTURE_2D, tex_name[1]);
    glBegin(GL_QUADS);
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
    switch (key) {}
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
