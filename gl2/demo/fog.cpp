/*
 *  fog.c
 *  this program draws 5 red spheres, each at a different
 *  z distance from the eye, in different types of fog.
 *  pressing the f key chooses between 3 types of
 *  fog:  exponential, exponential squared, and linear.
 *  in this program, there is a fixed density value, as well
 *  as fixed start and end values for the linear fog.
 */
#include <app.h>

namespace zxd {

GLint fog_mode;
GLdouble density = 0.35;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "fog";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  /*  initialize depth buffer, fog, light source,
   *  material property, and lighting model.
   */
  void create_scene(void) {
    GLfloat position[] = {0.5, 0.5, 3.0, 0.0};

    glEnable(GL_DEPTH_TEST);

    glLightfv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    {
      GLfloat mat[3] = {0.1745, 0.01175, 0.01175};
      glMaterialfv(GL_FRONT, GL_AMBIENT, mat);
      mat[0] = 0.61424;
      mat[1] = 0.04136;
      mat[2] = 0.04136;
      glMaterialfv(GL_FRONT, GL_DIFFUSE, mat);
      mat[0] = 0.727811;
      mat[1] = 0.626959;
      mat[2] = 0.626959;
      glMaterialfv(GL_FRONT, GL_SPECULAR, mat);
      glMaterialf(GL_FRONT, GL_SHININESS, 0.6 * 128.0);
    }

    glEnable(GL_FOG);
    {
      GLfloat fog_color[4] = {0.5, 0.5, 0.5, 1.0};

      fog_mode = GL_EXP;
      glFogi(GL_FOG_MODE, fog_mode);
      glFogfv(GL_FOG_COLOR, fog_color);
      glFogf(GL_FOG_DENSITY, density);
      glHint(GL_FOG_HINT, GL_DONT_CARE);
      glFogf(GL_FOG_START, 1.0);
      glFogf(GL_FOG_END, 5.0);
    }
    glClearColor(0.5, 0.5, 0.5, 1.0); /* fog color */
  }

  static void render_sphere(GLfloat x, GLfloat y, GLfloat z) {
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(0.4, 16, 16);
    glPopMatrix();
  }

  /* display() draws 5 spheres at different z positions.
   */
  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    render_sphere(-2., -0.5, -1.0);
    render_sphere(-1., -0.5, -2.0);
    render_sphere(0., -0.5, -3.0);
    render_sphere(1., -0.5, -4.0);
    render_sphere(2., -0.5, -5.0);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    const GLubyte info[] =
      "q : change fog mode \n"
      "w : change for density ";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      glOrtho(-2.5, 2.5, -2.5 * (GLfloat)h / (GLfloat)w,
        2.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
      glOrtho(-2.5 * (GLfloat)w / (GLfloat)h, 2.5 * (GLfloat)w / (GLfloat)h,
        -2.5, 2.5, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
      case 'Q':
        if (fog_mode == GL_EXP) {
          fog_mode = GL_EXP2;
          printf("fog mode is GL_EXP2\n");
        } else if (fog_mode == GL_EXP2) {
          fog_mode = GL_LINEAR;
          printf("fog mode is GL_LINEAR\n");
        } else if (fog_mode == GL_LINEAR) {
          fog_mode = GL_EXP;
          printf("fog mode is GL_EXP\n");
        }
        glFogi(GL_FOG_MODE, fog_mode);
        break;
      case 'w':
        density += 0.1;
        glFogf(GL_FOG_DENSITY, density);
        printf("density %3.4f\n", density);

        break;
      case 'W':
        density -= 0.1;
        if (density < 0) density = 0;
        glFogf(GL_FOG_DENSITY, density);
        printf("density %3.4f\n", density);
        break;

      default:
        break;
    }
  }

  /*  main loop
   *  open window with initial window size, title bar,
   *  RGBA display mode, depth buffer, and handle input events.
   */
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
