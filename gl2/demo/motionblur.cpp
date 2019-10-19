#include <app.h>

namespace zxd {

GLfloat angle = 0;
GLfloat delta_angle = 2;
GLfloat persistance = 0.25f;
GLint motion_frames = 6;
bool animate = false;

enum motion_technique
{
  mt_average, // 1st img weights 1, second 1 - 1/n ,third 1 - 2/n ...
  mt_mult,    // color buffer = current * (1 - t) + acc_buf * t
  mt_num_technique
};

const char* techniques[] = 
{
  "average",
  "mult"
};

GLint technique = 0;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "clip";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    glMatrixMode(GL_MODELVIEW);

    GLfloat light_position0[] = {0.0, 5.0f, 0.0, 1.0};
    GLfloat light_diffuse0[] = {1.0f, 1.0f, 1.0f, 1.0};
    GLfloat light_specular0[] = {1.0f, 1.0f, 0.0, 1.0};
    GLfloat light_ambient0[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat light_attenuation0[] = {1, 0, 0};

    glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, light_attenuation0[0]);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light_attenuation0[1]);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light_attenuation0[2]);

    GLfloat mat_ambient[] = {0.2, 0.2, 0.2, 1.0};
    GLfloat mat_diffuse[] = {0.8, 0.8, 0.8, 1.0};
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat mat_shininess = 90;

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glMaterialf(GL_FRONT, GL_SHININESS, mat_shininess);

    glClear(GL_ACCUM_BLUE_BITS);
  }

  virtual void update() {
    if (!animate)
      return;

    angle += delta_angle;
    if (angle > 360.0f)
    {
      angle -= 360.0f;
    }
  }

  void display(void) {
    GLdouble eqn[4] = {0.0, 1.0, 0.0, 0.0};   // clip out down part
    GLdouble eqn2[4] = {1.0, 0.0, 0.0, 0.0};  // clip out left part

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);

    if (technique == mt_average)
    {
      GLfloat weight = 1.0f / motion_frames;
      for (GLint i = 0; i < motion_frames; ++i)
      {
        // note that  no clear happen between this motion frames, which means the
        // final weight of the ith motion frame is weight * (motion_frames - i).
        // To make current frame solid, wu must render it first.
        glPushMatrix();
        glRotatef(angle - delta_angle * i, 0, 1, 0);
        glTranslatef(5, 0, 0);
        glutSolidTeapot(1);
        glPopMatrix();

        glAccum(i == 0 ? GL_LOAD : GL_ACCUM, weight);
      }
    }
    else
    {
      glPushMatrix();
      glRotatef(angle, 0, 1, 0);
      glTranslatef(5, 0, 0);
      glutSolidTeapot(1);
      glPopMatrix();

      // if fps is 60, persistance is 0.25, t is about 0.9
      GLfloat t = std::pow(0.2f, m_delta_time / persistance);
      glAccum(GL_MULT, t);
      glAccum(GL_ACCUM, 1 - t);
    }


    glAccum(GL_RETURN, 1.0);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 482);
    GLchar info[512];
    sprintf(info,
      "qQ  : animate : %d \n"
      "wW  : motion frames : %d \n"
      "eE  : delta angles : %f \n"
      "u   : technique : %s \n"
      , animate, motion_frames, delta_angle, techniques[technique]
      );
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 1.0, 50.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -20.0);
    glRotatef(45, 1, 0, 0);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key)
    {
      case 'q':
        animate = !animate;
        break;

      case 'w':
        motion_frames += 1;
        break;

      case 'W':
        motion_frames -= 1;
        if (motion_frames < 0)
          motion_frames = 0;
        break;

      case 'e':
        delta_angle += 0.1;
        break;

      case 'E':
        delta_angle -= 0.1;
        break;

      case 'u':
        technique = ++technique % mt_num_technique;
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
