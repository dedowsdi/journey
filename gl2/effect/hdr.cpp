/*
 *  hdr.c
 *  need arb_framebuffer_object arb_texture_float
 *
 *  HDR(high dynamic range) is used to break the limit of [0,1] in framebuffer.
 *  you should use GL_RGBAF* family internal format for framebuffer color
 *  attachment, it's also called floating point framebuffer.
 *
 *  the procedure of transform color back to [0,1] is called tone map.
 *  reinhard toon map:
 *    x/(1+x)
 *  simple exposre toon map:
 *    1 - e^(-x * exposure)
 *
 *  any function that result in [0,1] should work.
 */
#include "app.h"
#include "program.h"
#include "light.h"
#include "disk.h"
#include <sstream>
#include "quad.h"
#include "common_program.h"

namespace zxd {

glm::mat4 v_mat;
glm::mat4 p_mat;

GLuint fbo;
GLuint color_tex;
GLuint program;
GLboolean use_hdr = 0;

GLint method = 0;
GLfloat exposure = 1.0;

disk disk0(0, 1, 16, 8);
quad quad0;

const char* methods[] = {"reinhard", "exposure"};

std::vector<zxd::light_source> lights;
zxd::light_model lm;
zxd::material mtl;

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256
GLubyte check_image[IMAGE_HEIGHT][IMAGE_WIDTH][4];

static GLuint checker_tex;

blinn_program blinn_prg;

struct hdr_program : public zxd::program {
  GLint ul_method;
  GLint ul_exposure;
  GLint ul_diffuse_map;

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "shader2/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader2/hdr.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    ul_method = get_uniform_location("method");
    ul_exposure = get_uniform_location("exposure");
    ul_diffuse_map = get_uniform_location("diffuse_map");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }
} hdr_prg;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "hdr";
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
    // spot 0
    light_source spot_light;
    spot_light.position = vec4(0.0, 0.0, 0.05, 1.0);
    spot_light.diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    spot_light.specular = vec4(1.0, 1.0, 1.0, 1.0);
    spot_light.ambient = vec4(0.0, 0.0, 0.0, 1.0);
    spot_light.constant_attenuation = 1;
    spot_light.linear_attenuation = 0;
    spot_light.quadratic_attenuation = 0;
    spot_light.spot_direction = vec3(0, 0, -1);
    spot_light.spot_cutoff = 90;
    spot_light.spot_exponent = 1;

    // point 1
    light_source point_light;
    point_light.position = vec4(0.0, 0.0, 0.01, 1.0);
    point_light.diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    point_light.specular = vec4(1.0, 1.0, 1.0, 1.0);
    point_light.ambient = vec4(0.0, 0.0, 0.0, 1.0);
    point_light.constant_attenuation = 0.0001;
    point_light.linear_attenuation = 1;
    point_light.quadratic_attenuation = 10;

    lights.push_back(spot_light);
    lights.push_back(point_light);

    lm.ambient = vec4(0, 0, 0, 1.0);  // global ambient

    mtl.ambient = vec4(0.2, 0.2, 0.2, 1.0);
    mtl.diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    mtl.specular = vec4(1.0, 1.0, 1.0, 1.0);
    mtl.emission = vec4(0.0, 0.0, 0.0, 1.0);
    mtl.shininess = 2.0;

    // texture
    make_check_image();
    glGenTextures(1, &checker_tex);
    glBindTexture(GL_TEXTURE_2D, checker_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_WIDTH, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, check_image);

    // fbo
    glGenTextures(1, &color_tex);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, m_info.wnd_width,
      m_info.wnd_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &fbo);

    blinn_prg.with_texcoord = GL_TRUE;
    blinn_prg.light_count = 2;
    blinn_prg.legacy = GL_TRUE;
    blinn_prg.init();
    blinn_prg.bind_lighting_uniform_locations(lights, lm, mtl);
    v_mat = mat4(1.0);
    p_mat = mat4(1.0);

    disk0.build_mesh({attrib_semantic::vertex, attrib_semantic::normal,
      attrib_semantic::texcoord});
    //disk0.bind(blinn_prg.al_vertex, blinn_prg.al_normal, blinn_prg.al_texcoord);

    hdr_prg.init();
    quad0.build_mesh({attrib_semantic::vertex, attrib_semantic::texcoord});
    //quad0.bind(hdr_prg.al_vertex, -1, hdr_prg.al_texcoord);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
  }

  void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(blinn_prg.get_object());
    glBindTexture(GL_TEXTURE_2D, checker_tex);

    blinn_prg.tex_unit = 0;
    blinn_prg.update_lighting_uniforms(lights, lm, mtl, v_mat);
    blinn_prg.update_uniforms(glm::translate(vec3(0)), v_mat, p_mat);

    disk0.draw();
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    if (use_hdr) {
      glBindFramebuffer(GL_FRAMEBUFFER, fbo);
      glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);

      render();

      glBindFramebuffer(GL_FRAMEBUFFER, 0);
      // tone map
      glUseProgram(hdr_prg);
      glBindTexture(GL_TEXTURE_2D, color_tex);

      glUniform1i(hdr_prg.ul_method, method);
      glUniform1f(hdr_prg.ul_exposure, exposure);
      glUniform1i(hdr_prg.ul_diffuse_map, 0);

      quad0.draw();
    } else {
      render();
    }

    glUseProgram(0);

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 492);
    GLchar info[256];

    sprintf(info,
      "q : toggle hdr : %d\n"
      "w : toon map : %s\n"
      "eE : exposure : %.2f",
      use_hdr, methods[method], exposure);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

    glFlush();
    glutSwapBuffers();
  }

  void reshape(int w, int h) { app::reshape(w, h); }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        use_hdr ^= 1;
        break;
      case 'w':
        method = (method + 1) % 2;
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
