/*  mipmap.c
 *  this program demonstrates using mipmaps for texture maps.
 *  to overtly show the effect of mipmaps, each mipmap reduction
 *  level has a solidly colored, contrasting texture image.
 *  thus, the quadrilateral which is drawn is drawn with several
 *  different colors.
 *
 *  if you are useing mipmap, you must create texture from base_level until
 *  max_level(or 1*1), otherwise you are screwed!!!.
 *
 *  LOD:
 *    ρ = max(texel_width/pixel_width, texel_height/pixel_height)
 *    γ = log2(2, ρ) + lod_bias
 *
 *    just thought lod is float type level.
 *
 */
#include "app.h"

namespace zxd {

GLubyte mipmap_image32[32][32][4];
GLubyte mipmap_image16[16][16][4];
GLubyte mipmap_image8[8][8][4];
GLubyte mipmap_image4[4][4][4];
GLubyte mipmap_image2[2][2][4];
GLubyte mipmap_image1[1][1][4];

GLuint base_level = 0;
GLuint max_level = 5;
GLdouble min_lod = 0;
GLdouble max_lod = 5;
GLdouble lod_bias = 0;

GLuint tex_name;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "mipmap";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 640;
    m_info.wnd_height = 640;
  }

  void make_images(void) {
    int i, j;

    for (i = 0; i < 32; i++) {
      for (j = 0; j < 32; j++) {
        mipmap_image32[i][j][0] = 255;
        mipmap_image32[i][j][1] = 255;
        mipmap_image32[i][j][2] = 0;
        mipmap_image32[i][j][3] = 255;
      }
    }
    for (i = 0; i < 16; i++) {
      for (j = 0; j < 16; j++) {
        mipmap_image16[i][j][0] = 255;
        mipmap_image16[i][j][1] = 0;
        mipmap_image16[i][j][2] = 255;
        mipmap_image16[i][j][3] = 255;
      }
    }
    for (i = 0; i < 8; i++) {
      for (j = 0; j < 8; j++) {
        mipmap_image8[i][j][0] = 255;
        mipmap_image8[i][j][1] = 0;
        mipmap_image8[i][j][2] = 0;
        mipmap_image8[i][j][3] = 255;
      }
    }
    for (i = 0; i < 4; i++) {
      for (j = 0; j < 4; j++) {
        mipmap_image4[i][j][0] = 0;
        mipmap_image4[i][j][1] = 255;
        mipmap_image4[i][j][2] = 0;
        mipmap_image4[i][j][3] = 255;
      }
    }
    for (i = 0; i < 2; i++) {
      for (j = 0; j < 2; j++) {
        mipmap_image2[i][j][0] = 0;
        mipmap_image2[i][j][1] = 0;
        mipmap_image2[i][j][2] = 255;
        mipmap_image2[i][j][3] = 255;
      }
    }
    mipmap_image1[0][0][0] = 255;
    mipmap_image1[0][0][1] = 255;
    mipmap_image1[0][0][2] = 255;
    mipmap_image1[0][0][3] = 255;
  }

  void create_scene(void) {
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);

    glTranslatef(0.0, 0.0, -3.6);
    make_images();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef GL_VERSION_1_1
    glGenTextures(1, &tex_name);
    glBindTexture(GL_TEXTURE_2D, tex_name);
#endif
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, mipmap_image32);
    glTexImage2D(GL_TEXTURE_2D, 1, GL_RGBA, 16, 16, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, mipmap_image16);
    glTexImage2D(GL_TEXTURE_2D, 2, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE,
      mipmap_image8);
    glTexImage2D(GL_TEXTURE_2D, 3, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE,
      mipmap_image4);
    glTexImage2D(GL_TEXTURE_2D, 4, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE,
      mipmap_image2);
    glTexImage2D(GL_TEXTURE_2D, 5, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE,
      mipmap_image1);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
    glEnable(GL_TEXTURE_2D);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
#ifdef GL_VERSION_1_1
    glBindTexture(GL_TEXTURE_2D, tex_name);
#endif
    glBegin(GL_QUADS);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-2.0, -1.0, 0.0);
    glTexCoord2f(0.0, 8.0);
    glVertex3f(-2.0, 1.0, 0.0);
    glTexCoord2f(8.0, 8.0);
    glVertex3f(2000.0, 1.0, -6000.0);
    glTexCoord2f(8.0, 0.0);
    glVertex3f(2000.0, -1.0, -6000.0);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(180, 620);

    char info[512];

    sprintf(info,
      "mipmap works on pixels, \n"
      "face size in 3d space doesn't matter. \n"
      "a single face might use multiple levels of texture\n"
      "qQ : mip base level : %u\n"
      "wW : mip max level : %u\n"
      "eE : mip min lod : %3.2f\n"
      "rR : mip max lod : %3.2f\n"
      "uU : lod bias : %3.2f\n",
      base_level, max_level, min_lod, max_lod, lod_bias);

    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
    glEnable(GL_TEXTURE_2D);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 1.0, 30000.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        ++base_level;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, base_level);
        break;
      case 'Q':
        if (base_level > 0) {
          --base_level;
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, base_level);
        }
        break;
      case 'w':
        ++max_level;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_level);
        break;
      case 'W':
        if (max_level > 0) {
          --max_level;
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, max_level);
        }
        break;
      case 'e':
        min_lod += 0.1;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, min_lod);
        break;
      case 'E':
        if (min_lod > 0) {
          min_lod -= 0.1;
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, min_lod);
        }
        break;
      case 'r':
        max_lod += 0.1;
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, max_lod);
        break;
      case 'R':
        if (max_lod >= 0.1) {
          max_lod -= 0.1;
          glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_LOD, max_lod);
        }
        break;
      case 'u':
        lod_bias += 0.1;
        glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, lod_bias);
        ZCGEA;
        break;
      case 'U':
        lod_bias -= 0.1;
        glTexEnvf(GL_TEXTURE_FILTER_CONTROL, GL_TEXTURE_LOD_BIAS, lod_bias);
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
