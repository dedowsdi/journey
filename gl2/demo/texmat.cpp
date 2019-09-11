/*
 *  sprite.c
 *  this program demonstrates tex matrix
 */
#include <app.h>

namespace zxd {

#define img_width 32
#define img_height 32

GLubyte image[img_width][img_height][4];
GLuint tex;
GLdouble roll = 0;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "texmat";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGBA;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void make_check_image() {
    GLubyte c;
    for (int i = 0; i < img_height; ++i) {
      for (int j = 0; j < img_width; ++j) {
        c = (((i & 8) == 0) ^ ((j & 8) == 0)) * 255;
        image[i][j][0] = c;
        image[i][j][1] = c;
        image[i][j][2] = c;
        image[i][j][3] = 255;
      }
    }
  }

  void create_scene(void) {
    glClearColor(0.0, 0.5, 1.0, 0.0);
    glShadeModel(GL_FLAT);

    make_check_image();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // create texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img_width, img_height, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, image);

    /*glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);*/
    glEnable(GL_TEXTURE_2D);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5, 0.5, 0.0);
    glRotatef(roll, 0, 0, 1);
    glTranslatef(-0.5, -0.5, 0.0);

    glBegin(GL_QUADS);
    glTexCoord2f(0, 0);
    glVertex2f(-0.5, -0.5);
    glTexCoord2f(1, 0);
    glVertex2f(0.5, -0.5);
    glTexCoord2f(1, 1);
    glVertex2f(0.5, 0.5);
    glTexCoord2f(0, 1);
    glVertex2f(-0.5, 0.5);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    const GLubyte info[] = " q : rotate along unit z at center";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);
    glEnable(GL_TEXTURE_2D);

    glFlush();

    ZCGEA
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h) {
      gluOrtho2D(-1, 1, -1 * (double)h / w, 1 * (double)h / w);
    } else {
      gluOrtho2D(-1 * (double)w / h, 1 * (double)w / h, -1, 1);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        roll += 30;
        break;
      case 'Q':
        roll -= 30;
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
