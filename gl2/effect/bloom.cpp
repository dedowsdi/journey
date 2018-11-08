/*
 *  bloom.cpp
 *  need arb_framebuffer_object, ext_gpu_shader4, arb_texture_float
 *
 *  bleeds around the light source.
 *
 *    1. generate hdr map and brightness map.
 *    2. gaussian blur brightness map to make it bleeds.
 *    3. blend(add) hdr map and brightness map, tone map back to ldr.
 *
 */
#include "app.h"
#include "program.h"
#include "common_program.h"
#include "quad.h"

namespace zxd {

/*	create checkerboard texture	*/
#define IMAGE_WIDTH 128
#define IMAGE_HEIGHT 128
static GLubyte check_image[IMAGE_HEIGHT][IMAGE_WIDTH][4];

GLuint checker_tex;
GLuint color_tex, brightness_tex;
GLuint fbo;

GLuint times = 5;  // blur times
GLfloat threshold = 0.95;
GLfloat exposure = 1;

GLuint pingpong_fbo[2];
GLuint pingpong_tex[2];

quad quad0;

struct filter_brightess_program : public quad_base {
  GLint ul_threshold;

  virtual void do_attach_shaders() {
    attach(
      GL_FRAGMENT_SHADER, "data/shader/filter_brightness.fs.glsl");
  }

  virtual void do_update_uniforms() { glUniform1f(ul_threshold, threshold); }

  virtual void do_bind_uniform_locations() {
    uniform_location(&ul_threshold, "threshold");
  }
} filter_prg;

struct gaussion_blur_program : public quad_base {
  GLint ul_horizontal;
  GLint horizontal;

  virtual void do_attach_shaders() {
    attach(
      GL_FRAGMENT_SHADER, "data/shader/gaussian_blur_0_1_0.5_4.fs.glsl");
  }

  virtual void do_update_uniforms() { glUniform1i(ul_horizontal, horizontal); }

  virtual void do_bind_uniform_locations() {
    uniform_location(&ul_horizontal, "horizontal");
  }
} blur_prg;

struct bloom_program : public program {
  GLint ul_exposure;
  GLint ul_hdr_map;
  GLint ul_brightness_map;

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/bloom.fs.glsl");
  }

  virtual void update_uniforms(GLuint hdr_map_ti, GLuint brightness_map_ti) {
    glUniform1f(ul_exposure, exposure);
    glUniform1i(ul_hdr_map, hdr_map_ti);
    glUniform1i(ul_brightness_map, brightness_map_ti);
  }

  virtual void bind_uniform_locations() {
    uniform_location(&ul_exposure, "exposure");
    uniform_location(&ul_hdr_map, "hdr_map");
    uniform_location(&ul_brightness_map, "brightness_map");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }
} bloom_prg;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "bloom";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB;
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }

  void make_check_image(void) {
    int i, j, c;

    for (i = 0; i < IMAGE_HEIGHT; i++) {
      for (j = 0; j < IMAGE_WIDTH; j++) {
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
    // glEnable(GL_DEPTH_TEST);

    make_check_image();
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &checker_tex);
    glBindTexture(GL_TEXTURE_2D, checker_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, IMAGE_WIDTH, IMAGE_HEIGHT, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, check_image);

    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    glGenTextures(1, &brightness_tex);
    glBindTexture(GL_TEXTURE_2D, brightness_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // create fbo used to get color texture and brightness texture
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightness_tex, 0);

    // enable both attachments as draw buffers
    GLenum drawbuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, drawbuffers);

    // create pingpong fbo used to blur texture
    glGenFramebuffers(2, pingpong_fbo);
    glGenTextures(2, pingpong_tex);
    for (int i = 0; i < 2; ++i) {
      glBindTexture(GL_TEXTURE_2D, pingpong_tex[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      glTexImage2D(
        GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
      glBindFramebuffer(GL_FRAMEBUFFER, pingpong_fbo[i]);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
        GL_TEXTURE_2D, pingpong_tex[i], 0);
    }

    // init programs
    filter_prg.init();
    blur_prg.init();
    bloom_prg.init();

    quad0.include_texcoord(true);
    quad0.build_mesh();
  }

  void display(void) {
    // extract textures
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, checker_tex);
    glEnable(GL_TEXTURE_2D);

    filter_prg.use();
    filter_prg.update_uniforms();
    quad0.draw();

    // blur brightness texture
    blur_prg.use();
    for (int i = 0; i < times * 2; ++i) {
      GLint index = i & 1;
      glBindFramebuffer(GL_FRAMEBUFFER, pingpong_fbo[index]);
      GLuint tex = i == 0 ? brightness_tex : pingpong_tex[index ^ 1];
      glBindTexture(GL_TEXTURE_2D, tex);
      blur_prg.horizontal = index ^ 1;
      blur_prg.update_uniforms();
      quad0.draw();
    }

    // add bleeding brightness
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, color_tex);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, pingpong_tex[1]);

    bloom_prg.use();
    bloom_prg.update_uniforms(0, 1);

    quad0.draw();

    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);

    glUseProgram(0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glWindowPos2i(10, 492);
    GLchar info[512];
    sprintf(info,
      "qQ : threshold : %.2f \n"
      "wW : times : %d \n"
      "eE : exposure : %.2f \n",
      threshold, times, exposure);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

    glutSwapBuffers();
  }

  void reshape(int w, int h) { app::reshape(w, h); }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        threshold += 0.1;
        break;
      case 'Q':
        threshold -= 0.1;
        break;

      case 'w':
        times += 1;
        break;
      case 'W':
        if (times > 1) {
          times -= 1;
        }
        break;

      case 'e':
        exposure += 0.1;
        break;
      case 'E':
        exposure -= 0.1;
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
