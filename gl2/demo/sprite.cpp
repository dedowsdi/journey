/*
 *  sprite.c
 *  this program demonstrates point sprite
 */
#include <app.h>

namespace zxd {

#define IMG_WIDTH 32
#define IMG_HEIGHT 32

GLubyte image[IMG_WIDTH][IMG_HEIGHT][4];
GLuint tex;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "sprite";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGBA;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  // blue bottom
  void make_image() {
    GLubyte c;
    for (int i = 0; i < IMG_HEIGHT; ++i) {
      for (int j = 0; j < IMG_WIDTH; ++j) {
        c = i * ((double)256 / IMG_HEIGHT);
        image[i][j][0] = c;
        image[i][j][1] = 0;
        image[i][j][2] = 255;
        image[i][j][3] = c;
      }
    }
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);

    make_image();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // create texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMG_WIDTH, IMG_HEIGHT, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, image);

    /*glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);*/
    glEnable(GL_TEXTURE_2D);

    // enable point sprite
    glPointSize(100);
    glTexEnvf(
      GL_POINT_SPRITE, GL_COORD_REPLACE, GL_TRUE);  // enable point sprite
    glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
    glEnable(GL_POINT_SPRITE);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_POINTS);
    glColor3f(1.0f, 1.0f, 1.0f);
    glVertex3f(0.0f, 0.0f, 0.0f); // sprite center
    glTexCoord2f(0, 0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    const GLubyte info[] = " q : toggle GL_POINT_COORD_ORIGIN";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);
    glEnable(GL_TEXTURE_2D);

    glutSwapBuffers();

    ZCGEA
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-w / 2, w / 2, -h / 2, h / 2);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q': {
        int i;
        glGetIntegerv(GL_POINT_SPRITE_COORD_ORIGIN, &i);
        if (i == GL_LOWER_LEFT) {
          glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_UPPER_LEFT);
          printf("set origin to GL_UPPER_LEFT\n");
        } else {
          glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, GL_LOWER_LEFT);
          printf("set origin to GL_LOWER_LEFT\n");
        }
      } break;
    }
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
