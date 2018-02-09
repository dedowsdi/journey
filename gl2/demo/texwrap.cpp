/* texwrap.c
 *
 *  you can use GL_TEXTURE_BORDER_COLOR to set border color, or you can put
 *  border in your texture, the later case will make the width and height 2
 *  pixels larger.
 *
 *
 *  repeat : s = fract(s)
 *  mirror repeat : s = int(s)&1 ? 1 - fract(s) : fract(s)
 *  clamp : s = clamp(s, 0, 1)
 *  clam_to_edge :
 *  clamp_to_border : color = border in texture if it exists, otherwise
 *                    gl_texture_border_corlor
 *
 *  in GL_NEAREST:
 *     only clamp_to_border use border if tex is out of 1
 *
 *  in GL_LINEAR edge:
 *    repeat :2*2 array wraps to opposite edge of the texture, border ignored.
 *    mirror repeat : 2*2 array use the same edge of the texture twice.
 *    clamp : 2*2 array use border and texels at the edge.
 *    clamp_to_edge : border ignored.
 *    clamp_to_border : 2*2 array use border and texels at the edge.
 *
 */
#include "app.h"
#include "glenumstring.h"

namespace zxd {
/*	create checkerboard texture	*/
#define check_image_width 64
#define check_image_height 64

GLuint tex_name;

GLfloat borde_color[4] = {0.0f, 0.0f, 1.0f, 1.0f};

GLenum wrap_mode[] = {GL_CLAMP, GL_CLAMP_TO_BORDER, GL_CLAMP_TO_EDGE,
  GL_MIRRORED_REPEAT, GL_REPEAT};

GLenum filters[] = {
  GL_NEAREST,                 // point
  GL_LINEAR,                  // bilinear
  GL_NEAREST_MIPMAP_NEAREST,  //
  GL_LINEAR_MIPMAP_NEAREST,   //
  GL_NEAREST_MIPMAP_LINEAR,   //
  GL_LINEAR_MIPMAP_LINEAR     // trilinear
};

GLubyte check_image[check_image_height][check_image_width][4];

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "texwrap";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
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

    glGenTextures(1, &tex_name);
    glBindTexture(GL_TEXTURE_2D, tex_name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borde_color);

    /*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, check_image_width,
     * check_image_height,
     * 1,*/
    /*GL_RGBA, GL_UNSIGNED_BYTE, check_image);*/
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, check_image_width,
      check_image_height, GL_RGBA, GL_UNSIGNED_BYTE, check_image);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_name);

    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-1.0, -1.0, 0.0);
    glTexCoord2f(0.0, 2.0);
    glVertex3f(-1.0, 1.0, 0.0);
    glTexCoord2f(2.0, 2.0);
    glVertex3f(1.0, 1.0, 0.0);
    glTexCoord2f(2.0, 0.0);
    glVertex3f(1.0, -1.0, 0.0);

    glEnd();

    glDisable(GL_TEXTURE_2D);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    char info[512];

    GLint modeS, modeT, min_filter, mag_filter;
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &modeS);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &modeT);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &min_filter);
    glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &mag_filter);
    sprintf(info,
      "q : wrap s : %s \n"
      "w : wrap t : %s \n"
      "e : min filter : %s \n"
      "r : mag filter : %s \n"
      "border color : 0 0 1 1 \n",
      gl_tex_wrap_to_string(modeS), gl_tex_wrap_to_string(modeT),
      gl_tex_filter_to_string(min_filter), gl_tex_filter_to_string(mag_filter));

    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
    glEnable(GL_TEXTURE_2D);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      glOrtho(-2.25, 2.25, -2.25 * h / w, 2.25 * h / w, -10.0, 10.0);
    else
      glOrtho(-2.25 * w / h, 2.25 * w / h, -2.25, 2.25, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q': {
        GLint mode;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, &mode);
        GLint i = 0;
        while (wrap_mode[i] != mode) ++i;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_mode[(++i) % 5]);
      } break;
      case 'w': {
        GLint mode;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, &mode);
        GLint i = 0;
        while (wrap_mode[i] != mode) ++i;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_mode[(++i) % 5]);
      } break;
      case 'e': {
        GLint filter;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, &filter);
        GLint i = 0;
        while (filters[i] != filter) ++i;
        glTexParameteri(
          GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filters[(++i) % 6]);
      } break;
      case 'r': {
        GLint filter;
        glGetTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, &filter);
        GLint i = 0;
        while (filters[i] != filter) ++i;
        glTexParameteri(
          GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filters[(++i) % 2]);
      } break;
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
