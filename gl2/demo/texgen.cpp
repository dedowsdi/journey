/*  texgen.c
 *  this program draws a texture mapped teapot with
 *  automatically generated texture coordinates.  the
 *  texture is rendered as stripes on the teapot.
 *  initially, the object is drawn with texture coordinates
 *  based upon the object coordinates of the vertex
 *  and distance from the plane x = 0.  pressing the 'e'
 *  key changes the coordinate generation to eye coordinates
 *  of the vertex.  pressing the 'o' key switches it back
 *  to the object coordinates.  pressing the 's' key
 *  changes the plane to a slanted one (x + y + z = 0).
 *  pressing the 'x' key switches it back to x = 0.
 *
 *  GL_OBJECT_LINEAR:
 *    s = object_plane∙obj_vertex
 *
 *  GL_EYE_LINEAR
 *    s = eye_plane_t * mv_mat_i(model_view when glTexGen called) * eye_vertex
 *
 *    you can set model_view to identity to generate tex in eyespace
 *    you can set model_view to view,
 *        s = eye_plane_t * inv_view *  eye_vertex
 *          = eye_plane_t * world_vertex
 *        which means tex generated in world space
 *    don't set model_view to be the same as rederable model_view, it'll
 *    degraded to object linear.
 *
 */

#include <app.h>

namespace zxd {

#define stripe_image_width 32
GLubyte stripe_image[4 * stripe_image_width];

#ifdef GL_VERSION_1_1
static GLuint tex_name;
#endif

/*  planes for texture coordinate generation  */
GLfloat yz_plane[] = {1.0, 0.0, 0.0, 0.0};
GLfloat slanted[] = {1.0, 1.0, 1.0, 0.0};
GLfloat* current_coeff;
GLenum current_plane;
GLint current_gen_mode;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "texgen";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 600;
    m_info.wnd_height = 600;
  }

  // 1 dimension,  4 red + 28 green
  void make_stripe_image(void) {
    int j;

    for (j = 0; j < stripe_image_width; j++) {
      stripe_image[4 * j] = (GLubyte)((j <= 4) ? 255 : 0);
      stripe_image[4 * j + 1] = (GLubyte)((j > 4) ? 255 : 0);
      stripe_image[4 * j + 2] = (GLubyte)0;
      stripe_image[4 * j + 3] = (GLubyte)255;
    }
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);

    make_stripe_image();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef GL_VERSION_1_1
    glGenTextures(1, &tex_name);
    glBindTexture(GL_TEXTURE_1D, tex_name);
#endif
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#ifdef GL_VERSION_1_1
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripe_image_width, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, stripe_image);
#else
    glTexImage1D(GL_TEXTURE_1D, 0, 4, stripe_image_width, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, stripe_image);
#endif

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    current_coeff = yz_plane;
    current_gen_mode = GL_OBJECT_LINEAR;
    current_plane = GL_OBJECT_PLANE;
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, current_gen_mode);
    glTexGenfv(GL_S, current_plane, current_coeff);

    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_1D);
    glEnable(GL_CULL_FACE);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_NORMALIZE);
    glFrontFace(GL_CW);  // why???????????
    glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPushMatrix();
    glRotatef(45.0, 0.0, 0.0, 1.0);
#ifdef GL_VERSION_1_1
    glBindTexture(GL_TEXTURE_1D, tex_name);
#endif
    glutSolidTeapot(2.0);
    glPopMatrix();

    glDisable(GL_TEXTURE_1D);
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(20, 580);
    const GLubyte info[] =
      "q : set mode to GL_EYE_LINEAR, plane to GL_EYE_PLANE\n"
      "w : set mode to GL_OBJECT_LINEAR, plane to GL_OBJECT_PLANE\n"
      "e : set current plane to x + y + z = 0\n"
      "r : set current plane to x = 0";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);
    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_1D);

    glutSwapBuffers();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      glOrtho(-3.5, 3.5, -3.5 * (GLfloat)h / (GLfloat)w,
        3.5 * (GLfloat)h / (GLfloat)w, -3.5, 3.5);
    else
      glOrtho(-3.5 * (GLfloat)w / (GLfloat)h, 3.5 * (GLfloat)w / (GLfloat)h,
        -3.5, 3.5, -3.5, 3.5);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
      case 'Q':
        // model_view is identity right now
        current_gen_mode = GL_EYE_LINEAR;
        current_plane = GL_EYE_PLANE;
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, current_gen_mode);
        glTexGenfv(GL_S, current_plane, current_coeff);
        break;
      case 'w':
      case 'W':
        current_gen_mode = GL_OBJECT_LINEAR;
        current_plane = GL_OBJECT_PLANE;
        glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, current_gen_mode);
        glTexGenfv(GL_S, current_plane, current_coeff);
        break;
      case 'e':
      case 'E':
        current_coeff = slanted;
        glTexGenfv(GL_S, current_plane, current_coeff);
        break;
      case 'r':
      case 'R':
        current_coeff = yz_plane;
        glTexGenfv(GL_S, current_plane, current_coeff);
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
