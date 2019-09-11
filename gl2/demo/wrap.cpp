/*  wrap.c
 *  this program texture maps a checkerboard image onto
 *  two rectangles.  this program demonstrates the wrapping
 *  modes, if the texture coordinates fall outside 0.0 and 1.0.
 *  interaction: pressing the 's' and 'S' keys switch the
 *  wrapping between clamping and repeating for the s parameter.
 *  the 't' and 'T' keys control the wrapping for the t parameter.
 *
 *  if running this program on open_g_l 1.0, texture objects are
 *  not used.
 */
#include <app.h>

namespace zxd {

/*	create checkerboard texture	*/
#define IMAGE_WIDTH 64
#define IMAGE_HEIGHT 64
GLubyte check_image[IMAGE_HEIGHT][IMAGE_WIDTH][4];

GLuint tex_name;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "wrap";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, check_image);
#else
    glTexImage2D(GL_TEXTURE_2D, 0, 4, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, check_image);
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
    glTexCoord2f(0.0, 3.0);
    glVertex3f(-2.0, 1.0, 0.0);
    glTexCoord2f(3.0, 3.0);
    glVertex3f(0.0, 1.0, 0.0);
    glTexCoord2f(3.0, 0.0);
    glVertex3f(0.0, -1.0, 0.0);

    glTexCoord2f(0.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);
    glTexCoord2f(0.0, 3.0);
    glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(3.0, 3.0);
    glVertex3f(2.41421, 1.0, -1.41421);
    glTexCoord2f(3.0, 0.0);
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
      case 's':
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
        break;
      case 'S':
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        break;
      case 't':
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        break;
      case 'T':
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
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
