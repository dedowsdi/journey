/*
 *  alpha3D.c
 *  this program demonstrates how to intermix opaque and
 *  alpha blended polygons in the same scene, by using
 *  glDepthMask.  press the 'a' key to animate moving the
 *  transparent object through the opaque object.  press
 *  the 'r' key to reset the scene.
 *
 *  opaque objects must be drawn before transparent objects.
 *  transparent objects must be renderd by z order, from far to near.
 *
 *  it's said that depth mask mut be set to readonly when drawing transparent
 *  object, so transparent objects can be blocked by opaque objects, but can not
 *  block opaque objects, only blend with them.
 *  I have doubt about this, even if depth mask is RW, opaque objects still will
 *  not be blocked, the only different is the depth value changed to the value
 *  of the transparent object.
 *
 */
#include "app.h"

namespace zxd {

#define MAXZ 8.0
#define MINZ -8.0
#define ZINC 0.4

float solidZ = MAXZ;
float transparentZ = MINZ;
GLuint sphere_list, cube_list;
GLboolean read_only = 1;
GLboolean blend = 1;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "alpha3D";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    GLfloat mat_specular[] = {1.0, 1.0, 1.0, 0.15};
    GLfloat mat_shininess[] = {100.0};
    GLfloat position[] = {0.5, 0.5, 1.0, 0.0};

    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    sphere_list = glGenLists(1);
    glNewList(sphere_list, GL_COMPILE);
    glutSolidSphere(0.4, 16, 16);
    glEndList();

    cube_list = glGenLists(1);
    glNewList(cube_list, GL_COMPILE);
    glutSolidCube(0.6);
    glEndList();
  }

  void display(void) {
    GLfloat mat_solid[] = {0.75, 0.75, 0.0, 1.0};
    GLfloat mat_zero[] = {0.0, 0.0, 0.0, 1.0};
    GLfloat mat_transparent[] = {0.0, 0.8, 0.8, 0.6};
    GLfloat mat_emission[] = {0.0, 0.3, 0.3, 0.6};

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(-0.15, -0.15, solidZ);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_zero);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_solid);
    glCallList(sphere_list);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.15, 0.15, transparentZ);
    glRotatef(15.0, 1.0, 1.0, 0.0);
    glRotatef(30.0, 0.0, 1.0, 0.0);
    glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);

    // read depth only
    if (blend) glEnable(GL_BLEND);
    if (read_only) glDepthMask(GL_FALSE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glCallList(cube_list);
    if (read_only) glDepthMask(GL_TRUE);
    if (blend) glDisable(GL_BLEND);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    const GLubyte info[] =
      "a : animation\n"
      "r : reset\n"
      "d : toggle depthwrite when draw cube\n"
      "b : toggle blend when draw cube";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);
    glEnable(GL_LIGHTING);

    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLint)w, (GLint)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      glOrtho(-1.5, 1.5, -1.5 * (GLfloat)h / (GLfloat)w,
        1.5 * (GLfloat)h / (GLfloat)w, -10.0, 10.0);
    else
      glOrtho(-1.5 * (GLfloat)w / (GLfloat)h, 1.5 * (GLfloat)w / (GLfloat)h,
        -1.5, 1.5, -10.0, 10.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void update(void) {
    if (solidZ <= MINZ || transparentZ >= MAXZ)
      glutIdleFunc(NULL);
    else {
      solidZ -= ZINC;
      transparentZ += ZINC;
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'a':
      case 'A':
        solidZ = MAXZ;
        transparentZ = MINZ;
        break;
      case 'r':
      case 'R':
        solidZ = MAXZ;
        transparentZ = MINZ;
        break;
      case 'd':
      case 'D':
        read_only = !read_only;
        if (read_only)
          printf("depth read only\n");
        else
          printf("depth read write\n");
        break;
      case 'b':
      case 'B':
        blend = !blend;
        if (blend)
          printf("blend\n");
        else
          printf("no blend\n");
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
