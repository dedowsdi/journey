/* gamma.c
 *
 * monitor will apply pow(color, gamma) to transform color from linear space to
 * srgb space. gamma correction is ued to compensate this, it's done by
 * pow(color, 1/gamma). lots of textures(such as diffuse) is created directly in
 * srgb space which means it doesn't need gamma correction, make sure it's
 * internal_format is sRGB, opengl will apply pow(color, gamma) to it to
 * cancel gomma correction.
 */
#include "app.h"
#include "program.h"

namespace zxd {

#define IMAGE_WIDTH 64
#define IMAGE_HEIGHT 64
static GLubyte image[IMAGE_HEIGHT][IMAGE_WIDTH][1];
static GLuint tex_name;
GLint intensity = 120;
GLboolean srgb_texture = 0;

struct gamma_program : public program {
  GLint ul_iamge;
  virtual void attach_shaders() {
    attach(GL_FRAGMENT_SHADER, "shader2/gamma.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    ul_iamge = get_uniform_location("image");
  };
} prg0;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "gamma";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }

  void make_image(void) {
    memset(image, intensity, IMAGE_HEIGHT * IMAGE_WIDTH * sizeof(GLubyte));
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glEnable(GL_DEPTH_TEST);

    make_image();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &tex_name);
    glBindTexture(GL_TEXTURE_2D, tex_name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0,
      GL_LUMINANCE, GL_UNSIGNED_BYTE, image);

    prg0.init();
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glBindTexture(GL_TEXTURE_2D, tex_name);

    glBegin(GL_QUADS);

    glTexCoord2i(0, 0);
    glVertex2i(-1, -1);

    glTexCoord2i(1, 0);
    glVertex2i(1, -1);

    glTexCoord2i(1, 1);
    glVertex2i(1, 1);

    glTexCoord2i(0, 1);
    glVertex2i(-1, 1);

    glEnd();

    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 492);
    GLchar info[256];
    GLint current_program;
    glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
    glUseProgram(0);
    sprintf(info,
      " q : toggle gamma correction : %d\n"
      " w : toggle sRGB texture : %d\n"
      " eE : change intensity : %d",
      current_program != 0, srgb_texture, intensity);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
    glUseProgram(current_program);
    glEnable(GL_TEXTURE_2D);

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void update_texture() {
    GLenum internal_format = srgb_texture ? GL_SRGB : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, IMAGE_WIDTH, IMAGE_HEIGHT,
      0, GL_LUMINANCE, GL_UNSIGNED_BYTE, image);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q': {
        GLint current_program;
        glGetIntegerv(GL_CURRENT_PROGRAM, &current_program);
        if (current_program != 0) {
          glUseProgram(0);
        } else {
          glUseProgram(prg0.get_object());
          glUniform1i(prg0.ul_iamge, 0);
        }
        break;
      }
      case 'w': {
        srgb_texture = !srgb_texture;
        update_texture();
        break;
      }
      case 'e': {
        intensity += 15;
        if (intensity > 255) {
          intensity = 255;
        }
        make_image();
        update_texture();
        break;
      }
      case 'E': {
        intensity -= 15;
        if (intensity < 0) {
          intensity = 0;
        }
        make_image();
        update_texture();
        break;
      }

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
