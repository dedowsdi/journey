/*
 *  blur.cpp
 *
 *  gaussian function is a random variable. it's peak is at 0, which makes it
 *  perfect for blur.
 *
 *  gaussian blur blend n*n pixels, which means you need to samples n*n samples
 *  to blend them, that's a lot of work, but it can be seperated into horizontal
 *  and vertical blur, which means 2 render pass, each pass blend n pixels, this
 *  is also called two-pass gaussian blur.
 *
 */
#include "app.h"
#include "program.h"
#include "common_program.h"
#include "quad.h"
#include <sstream>

namespace zxd {

/*	create checkerboard texture	*/
#define IMAGE_WIDTH 128
#define IMAGE_HEIGHT 128
static GLubyte check_image[IMAGE_HEIGHT][IMAGE_WIDTH][4];

GLuint checker_tex;
GLuint color_tex;
GLuint fbo;

GLfloat mean = 0, deviation = 1, gaussian_step = 1;
GLuint times = 5;

GLuint pingpong_fbo[2];
GLuint pingpong_tex[2];

quad_program quad_prg;
quad quad0;

struct gaussian_blur_program : public program {
  GLint ul_mean;
  GLint ul_deviation;
  GLint ul_gaussian_step;
  GLint ul_horizontal;
  GLint ul_quad_map;
  GLuint radius;

  gaussian_blur_program() : radius(4) {}

  virtual void bind_uniform_locations() {
    ul_mean = get_uniform_location("mean");
    ul_deviation = get_uniform_location("deviation");
    ul_gaussian_step = get_uniform_location("gaussian_step");
    ul_horizontal = get_uniform_location("horizontal");
    ul_quad_map = get_uniform_location("quad_map");
  }

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "shader2/quad.vs.glsl");

    std::stringstream ss;
    ss << "#version 120\n#define radius " << radius << "\n";
    string_vector sv;
    sv.push_back(ss.str());
    attach(
      GL_FRAGMENT_SHADER, sv, "shader2/gaussian_blur.fs.glsl");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  };

  void update_uniforms(GLint texunit = 0) {
    glUniform1f(ul_mean, mean);
    glUniform1f(ul_deviation, deviation);
    glUniform1f(ul_gaussian_step, gaussian_step);
    glUniform1i(ul_quad_map, texunit);
  }

} gb_prg;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "gaussian_blur";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, IMAGE_WIDTH, IMAGE_HEIGHT, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, check_image);

    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

    // create fbo used to get color attachment
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

    glGenFramebuffers(2, pingpong_fbo);
    glGenTextures(2, pingpong_tex);
    // create pingpong fbo used to blur texture
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
    quad_prg.init();
    gb_prg.init();

    quad0.build_mesh({attrib_semantic::vertex, attrib_semantic::texcoord});
  }

  void display(void) {
    // render to texture
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, checker_tex);
    quad_prg.use();
    quad_prg.update_uniforms(0);
    quad0.draw();

    // blur

    gb_prg.use();
    gb_prg.update_uniforms(0);

    for (int i = 0; i < times * 2; ++i) {
      GLint index = i & 1;
      glBindFramebuffer(GL_FRAMEBUFFER, pingpong_fbo[index]);
      GLuint tex = i == 0 ? color_tex : pingpong_tex[index ^ 1];
      glBindTexture(GL_TEXTURE_2D, tex);
      glUniform1i(gb_prg.ul_horizontal, index ^ 1);
      quad0.draw();
    }


    // draw final result to defult fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    quad_prg.use();

    glBindTexture(GL_TEXTURE_2D, pingpong_tex[1]);
    quad0.draw();

    glDisable(GL_TEXTURE_2D);
    glUseProgram(0);

    glColor3f(1.0f, 0.0f, 0.0f);
    glWindowPos2i(10, 492);
    GLchar info[512];
    sprintf(info,
      "qQ : mean : %.2f \n"
      "wW : deviation : %.2f \n"
      "eE : gaussian_step : %.2f \n"
      "rR : times : %d \n"
      "uU : radius : %d \n",
      mean, deviation, gaussian_step, times, gb_prg.radius);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

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

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        mean += 0.1;
        break;

      case 'Q':
        mean -= 0.1;
        break;

      case 'w':
        deviation += 0.1;
        break;

      case 'W':
        deviation -= 0.1;
        break;

      case 'e':
        gaussian_step += 0.02;
        break;

      case 'E':
        gaussian_step -= 0.02;
        break;

      case 'r':
        times += 1;
        break;

      case 'R':
        if (times > 1) {
          times -= 1;
        }
        break;

      case 'u':
        ++gb_prg.radius;
        gb_prg.clear();
        gb_prg.init();
        break;

      case 'U':
        if (gb_prg.radius > 1) {
          --gb_prg.radius;
          gb_prg.clear();
          gb_prg.init();
        }
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
