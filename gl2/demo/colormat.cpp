/*
 *  colormat.c
 *  after initialization, the program will be in
 *  color_material mode.  interaction:  pressing the
 *  mouse buttons will change the diffuse reflection values.
 *
 *  glColorMaterial must be called before glEnable(GL_COLOR_MATERIAL)
 *  according to official document, if only a single material parameter needs
 *  to be changed, glColorMaterial is better than glMaterial.
 *
 *  it tracks current color and update material parameter which has been setup
 *  by glColorMaterial
 *
 */
#include "app.h"

namespace zxd {

GLfloat lmodel_ambient[4] = {0.2, 0.2, 0.2, 1.0};
GLfloat mat_diffuse[4] = {0.5, 0.5, 0.5, 1.0};
GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
GLfloat mat_ambient[] = {0.2, 0.2, 0.2, 0.2};
GLfloat mat_emission[] = {0.1, 0.1, 0.1, 1.0};
GLfloat mat_shiness = 50.0;

double eqn[4] = {0, 0, -1, 0};

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "colormat";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  /*  initialize material property, light source, lighting model,
   *  and depth buffer.
   */
  void create_scene(void) {
    GLfloat light_position[] = {1.0, 1.0, 1.0, 0.0};

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_DEPTH_TEST);

    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_emission);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shiness);

    // glColormaterial must be called before
    // glEnable(gl__c_o_l_o_r__m_a_t_e_r_i_a_l)
    glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    glColor4fv(mat_diffuse);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();

    glTranslatef(250, 250, 0);
    glutSolidSphere(150.0, 40, 32);

    glPopMatrix();

    GLint twoSides;
    glGetIntegerv(GL_LIGHT_MODEL_TWO_SIDE, &twoSides);

    char info[512];
    sprintf(info,
      "q : diffuse : %.2f %.2f %.2f %.2f\n"
      "w : ambient : %.2f %.2f %.2f %.2f\n"
      "e : specular : %.2f %.2f %.2f %.2f\n"
      "r : emission : %.2f %.2f %.2f %.2f\n"
      "uU : shiness : %.2f\n"
      "i :  two side light model : %i\n"
      "o : toggle clip plane : 0 0 -1 0 : %i\n",
      mat_diffuse[0], mat_diffuse[1], mat_diffuse[2], mat_diffuse[3],
      mat_ambient[0], mat_ambient[1], mat_ambient[2], mat_ambient[3],
      mat_specular[0], mat_specular[1], mat_specular[2], mat_specular[3],
      mat_emission[0], mat_emission[1], mat_emission[2], mat_emission[3],
      mat_shiness,
      twoSides,
      glIsEnabled(GL_CLIP_PLANE0)
      );

    glDisable(GL_COLOR_MATERIAL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    GLfloat oldmat[4];
    glGetFloatv(GL_CURRENT_COLOR, oldmat);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glRasterPos2f(10, 480);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)info);
    // restore current color, which is used by color material
    glColor4fv(oldmat);

    glEnable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);

    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, w, 0, h, -1000, 1000);
    /*if (w <= h)*/
    /*glOrtho(-1.5, 1.5, -1.5 * (GLfloat)h / (GLfloat)w,*/
    /*1.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);*/
    /*else*/
    /*glOrtho(-1.5 * (GLfloat)w / (GLfloat)h, 1.5 * (GLfloat)w / (GLfloat)h,
     * -1.5,*/
    /*1.5, -10.0, 10.0);*/
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glClipPlane(GL_CLIP_PLANE0, eqn);
  }

  void mouse(int button, int state, int x, int y) {
    switch (button) {
      default:
        break;
    }
  }

  void update_material(float mat[4], float addition, GLenum type) {
    float f = (mat[0] + 0.1);
    if (f > 1.0) f = 0;
    mat[0] = mat[1] = mat[2] = f;
    if (type != GL_DIFFUSE) {
      glMaterialfv(GL_FRONT_AND_BACK, type, mat);
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        update_material(mat_diffuse, 0.1, GL_DIFFUSE);
        glColor4fv(mat_diffuse);
        break;
      case 'w':
        update_material(mat_ambient, 0.1, GL_AMBIENT);
        break;
      case 'e':
        update_material(mat_specular, 0.1, GL_SPECULAR);
        break;
      case 'r':
        update_material(mat_emission, 0.1, GL_EMISSION);
        break;
      case 'u':
        mat_shiness += 10;
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shiness);
        break;
      case 'U':
        mat_shiness -= 5;
        if (mat_shiness < 0) mat_shiness = 0;
        glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shiness);
        break;
      case 'i': {
        int i;
        glGetIntegerv(GL_LIGHT_MODEL_TWO_SIDE, &i);
        if (i)
          glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);
        else
          glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 1);
      } break;

      case 'o': {
        if (glIsEnabled(GL_CLIP_PLANE0))
          glDisable(GL_CLIP_PLANE0);
        else
          glEnable(GL_CLIP_PLANE0);
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
