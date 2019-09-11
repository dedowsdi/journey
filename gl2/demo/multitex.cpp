/*  multitex.c
 *
 *  texture unit:
 *   	texture image
 *	 	filtering parameters
 *	 	environment application
 *	 	texture matrix stack
 *	 	automatic texture-coordinate generation
 *	 	vertex-array specification (if needed, through glClintActivateTexture)
 *
 */
#include <app.h>

namespace zxd {

GLubyte texels0[32][32][4];
GLubyte texels1[16][16][4];
GLuint tex_names[2];

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "multitex";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH;
    m_info.wnd_width = 250;
    m_info.wnd_height = 250;
  }

  void make_images(void) {
    int i, j;

    for (i = 0; i < 32; i++) {
      for (j = 0; j < 32; j++) {
        texels0[i][j][0] = (GLubyte)64;
        texels0[i][j][1] = (GLubyte)j * 5;
        texels0[i][j][2] = (GLubyte)(i * j) / 255;
        texels0[i][j][3] = (GLubyte)255;
      }
    }

    for (i = 0; i < 16; i++) {
      for (j = 0; j < 16; j++) {
        texels1[i][j][0] = (GLubyte)255;
        texels1[i][j][1] = (GLubyte)i * 10;
        texels1[i][j][2] = (GLubyte)j * 10;
        texels1[i][j][3] = (GLubyte)255;
      }
    }
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    make_images();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glEnable(GL_TEXTURE_2D);
    glGenTextures(2, tex_names);
    glBindTexture(GL_TEXTURE_2D, tex_names[0]);
    ZCGE(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 32, 32, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, texels0));

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindTexture(GL_TEXTURE_2D, tex_names[1]);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGBA, 16, 16, 0, GL_RGBA, GL_UNSIGNED_BYTE, texels1);
    gluBuild2DMipmaps(
      GL_TEXTURE_2D, GL_RGBA, 16, 16, GL_RGBA, GL_UNSIGNED_BYTE, texels1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /*  use the two texture objects to define two texture units
     *  for use in multitexturing  */
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_names[0]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glTranslatef(0.5f, 0.5f, 0.0f);
    glRotatef(45.0f, 0.0f, 0.0f, 1.0f);
    glTranslatef(-0.5f, -0.5f, 0.0f);
    glMatrixMode(GL_MODELVIEW);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, tex_names[1]);
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
    glMultiTexCoord2f(GL_TEXTURE1, 1.0, 0.0);
    glVertex2f(0.0, 0.0);
    glMultiTexCoord2f(GL_TEXTURE0, 0.5, 1.0);
    glMultiTexCoord2f(GL_TEXTURE1, 0.5, 0.0);
    glVertex2f(50.0, 100.0);
    glMultiTexCoord2f(GL_TEXTURE0, 1.0, 0.0);
    glMultiTexCoord2f(GL_TEXTURE1, 1.0, 1.0);
    glVertex2f(100.0, 0.0);
    glEnd();
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      gluOrtho2D(0.0, 100.0, 0.0, 100.0 * (GLdouble)h / (GLdouble)w);
    else
      gluOrtho2D(0.0, 100.0 * (GLdouble)w / (GLdouble)h, 0.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
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
