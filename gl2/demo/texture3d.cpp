/*  texture3d.c
 *  this program demonstrates using a three-dimensional texture.
 *  it creates a 3D texture and then renders two rectangles
 *  with different texture coordinates to obtain different
 *  "slices" of the 3D texture.
 *
 *  you don't need to use the same r for 3d tex coord of primitives.
 */
#include "app.h"

namespace zxd {

GLuint _x0 = 0, _y0 = 0, width = 16, height = 16, depth = 16;

GLubyte image[16][16][16][3];
GLuint tex_name;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "texture3d";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  /*  create a 16x16x16x3 array with different color values in
   *  each array element [r, g, b].  values range from 0 to 255.
   */

  void make_image(void) {
    int s, t, r;

    for (s = 0; s < 16; s++)
      for (t = 0; t < 16; t++)
        for (r = 0; r < 16; r++) {
          image[r][t][s][0] = (GLubyte)(s * 17);
          image[r][t][s][1] = (GLubyte)(t * 17);
          image[r][t][s][2] = (GLubyte)(r * 17);
        }
  }

  void gen_tex() {
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB,
      GL_UNSIGNED_BYTE, image);
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    make_image();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, width);
    glPixelStorei(GL_UNPACK_IMAGE_HEIGHT, height);

    glGenTextures(1, &tex_name);
    glBindTexture(GL_TEXTURE_3D, tex_name);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glEnable(GL_TEXTURE_3D);

    gen_tex();
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);
    // you don't need to specify the same r for quad
    glTexCoord3f(0.0, 0.0, 0.0);
    glVertex3f(-2.25, -1.0, 0.0);
    glTexCoord3f(0.0, 1.0, 0.0);
    glVertex3f(-2.25, 1.0, 0.0);
    glTexCoord3f(1.0, 1.0, 1.0);
    glVertex3f(-0.25, 1.0, 0.0);
    glTexCoord3f(1.0, 0.0, 1.0);
    glVertex3f(-0.25, -1.0, 0.0);

    glTexCoord3f(0.0, 0.0, 1.0);
    glVertex3f(0.25, -1.0, 0.0);
    glTexCoord3f(0.0, 1.0, 1.0);
    glVertex3f(0.25, 1.0, 0.0);
    glTexCoord3f(1.0, 1.0, 0.0);
    glVertex3f(2.25, 1.0, 0.0);
    glTexCoord3f(1.0, 0.0, 0.0);
    glVertex3f(2.25, -1.0, 0.0);
    glEnd();

    glDisable(GL_TEXTURE_3D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    char info[512];

    sprintf(info,
      "qQ : pixel _x0 : %u\n"
      "wW : pixel _y0 : %u\n"
      "eE : tex width : %u\n"
      "rR : tex height : %u\n"
      "uR : skip tex images : %u\n",
      _x0, _y0, width, height, 16 - depth);

    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
    glEnable(GL_TEXTURE_3D);

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
    glTranslatef(0.0, 0.0, -4.0);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        if (_x0 < 16) {
          _x0 += 1;
          glPixelStorei(GL_UNPACK_SKIP_PIXELS, _x0);
          gen_tex();
        }
        break;
      case 'Q':
        if (_x0 > 0) {
          _x0 -= 1;
          glPixelStorei(GL_UNPACK_SKIP_PIXELS, _x0);
          gen_tex();
        }
        break;
      case 'w':
        if (_y0 < 16) {
          _y0 += 1;
          glPixelStorei(GL_UNPACK_SKIP_ROWS, _y0);
          gen_tex();
        }
        break;
      case 'W':
        if (_y0 > 0) {
          _y0 -= 1;
          glPixelStorei(GL_UNPACK_SKIP_ROWS, _y0);
          gen_tex();
        }
        break;
      case 'e':
        if (width < 16) {
          width += 1;
          gen_tex();
        }
        break;
      case 'E':
        if (width > 0) {
          width -= 1;
          gen_tex();
        }
        break;
      case 'r':
        if (height < 12) {
          height += 1;
          gen_tex();
        }
        break;
      case 'R':
        if (height > 0) {
          height -= 1;
          gen_tex();
        }
        break;
      case 'u':
        // skip image
        if (depth > 0) {
          --depth;
          glPixelStorei(GL_UNPACK_SKIP_IMAGES, 16 - depth);
          gen_tex();
        }
        break;
      case 'U':
        if (depth < 16) {
          ++depth;
          glPixelStorei(GL_UNPACK_SKIP_IMAGES, 16 - depth);
          gen_tex();
        }
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
