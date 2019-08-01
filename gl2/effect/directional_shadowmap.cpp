/*
 * directional_shadowmap.c
 *
 * need arb_framebuffer_object
 *
 * shadow acne:
 *  as depth map is discrete, different fragments with different depth might
 *  sample the same texel, the depth discrepancy could cause shadow acne, bias
 *  is used to eliminite shaodw acne. it's calculated as :
 *    float shadow_bias = clamp(bias * tan(acos(ndotl)), 0, 0.01);
 *
 * peter pan:
 *  caused by shadow bias, can be fiexed via replacing thin geometry with
 *  thicker geometry.
 *
 * over sampling:
 *    this only happens when the light proj doesn't include the entire visibile
 *    area of the camea.
 *
 *    if it's beyound one of left, right, bottom, top plane, the generated
 *    texcoord doesn't belong to [0,1], you can control the visibility by
 *    setting wrap mode to clamp_to_border, and set border_color to the depth
 *    you thought fitful.
 *
 *    if it's beyound one of near and far plane, the generated light space depth
 *    doesn't belong to [0,1], you have to manually handle them in the shader.
 *
 * PCF(percentage-closer filter):
 *    sample more than once from the depth map, each time with slightly
 *    different texture coordinates. example:
 *
 *    float visibility = 1.0;
 *    vec2 texel_size = 1.0 / texture_size(shadow_map, 0);
 *    for(int x = -1; x <= 1; ++x)
 *    {
 *        for(int y = -1; y <= 1; ++y)
 *        {
 *            float pcf_depth = texture(shadow_map, proj_coords.xy + vec2(x, y)
 * *
 * texel_size).r;
 *            visibility += current_depth - bias > pcf_depth ? 0.0 : 1.0;
 *        }
 *    }
 *    visibility /= 9.0;
 *
 *    if you use sample2_d_shadow, use shadow2D or shadow2_d_proj, pcf will be
 * done
 *    on hardware.
 *
 * you can use the same depth texture for both sample2D and sample2_d_shadow,
 * although opengl will complain sampler type and depth compare discrapancy.
 * it's properbally safer to make a copy for shadow map.
 *
 */

#include <sstream>

#include "app.h"
#include "program.h"
#include "light.h"
#include "sphere.h"
#include "xyplane.h"
#include "stream_util.h"

namespace zxd {
using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLuint shadow_width = WINDOWS_WIDTH;
GLuint shadow_height = WINDOWS_HEIGHT;

glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 light_v_mat;
glm::mat4 light_p_mat;

sphere sphere0(1, 16, 16);
xyplane plane0(-10, -10, 10, 10);
xyplane plane1(-6, -10, 0, 10);

GLdouble light_top = 15;
GLdouble light_near = 0.1, light_far = 30;
GLfloat bias = 0.005;
GLboolean use_sampler2_d_shadow = GL_FALSE;

mat4 light_bsvp_mat;  // bias * scale * light_proj * light_view

GLuint fbo;
GLuint depth_map, shadow_map;
GLdouble wnd_aspect;

const char* filter_strings[] = {"NEAREST", "LINEAR"};
GLenum filters[] = {GL_NEAREST, GL_LINEAR};
GLint filter = 0;

vec3 camera_pos = vec3(5, -20, 20);

std::vector<zxd::light_source> lights;
light_model lm;
material mtl;

GLboolean camera_at_light = GL_FALSE;

struct render_program : public zxd::program {
  GLint ul_mvp_mat;

  virtual void update_uniforms(const glm::mat4& m_mat) {
    mat4 mvp_mat = light_p_mat * light_v_mat * m_mat;
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, &mvp_mat[0][0]);
  }
  virtual void update_frame() {
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);

    GLdouble right = light_top * wnd_aspect;
    GLdouble left = -right;

    light_p_mat = glm::ortho(left, right, -light_top, light_top, light_near, light_far);
    light_v_mat = glm::lookAt(vec3(lights[0].position), vec3(0, 0, 0), vec3(0, 0, 1));

    light_bsvp_mat = glm::translate(vec3(0.5, 0.5, 0.5)) *
                     glm::scale(vec3(0.5, 0.5, 0.5)) * light_p_mat * light_v_mat;
  }
  virtual void attach_shaders() {
    // render shadow program
    attach(
      GL_VERTEX_SHADER, "shader2/render_directional_shadowmap.vs.glsl");
    attach(
      GL_FRAGMENT_SHADER, "shader2/render_directional_shadowmap.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_mvp_mat, "mvp_mat");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
  }

} render_prg;

struct use_program : public zxd::program {
  GLint ul_light_mat;
  GLint ul_bias;
  GLint ul_use_sampler2_d_shadow;
  GLint ul_depth_map;
  GLint ul_shadow_map;
  GLint ul_mv_mat;
  GLint ul_mvp_mat;
  GLint ul_mv_mat_it;

  virtual void update_frame() {
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, depth_map);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, shadow_map);

    if (camera_at_light)
      v_mat = glm::lookAt(vec3(lights[0].position), vec3(0, 0, 0), vec3(0, 0, 1));
    else
      v_mat = glm::lookAt(camera_pos, vec3(0, 0, 0), vec3(0, 0, 1));

    mtl.update_uniforms();
    lm.update_uniforms();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].update_uniforms(v_mat);
    }

    p_mat = glm::perspective<GLfloat>(45.0f, wnd_aspect, 0.1, 100);

    glUniform1f(ul_bias, bias);
    glUniform1i(ul_use_sampler2_d_shadow, use_sampler2_d_shadow);
    glUniform1i(ul_depth_map, 0);
    glUniform1i(ul_shadow_map, 1);
  }

  void update_uniforms(const glm::mat4& m_mat) {
    mat4 mv_mat = v_mat * m_mat;
    mat4 mv_mat_it = glm::inverse(glm::transpose(mv_mat));
    mat4 mvp_mat = p_mat * mv_mat;
    // mat4 m_mat_i = glm::inverse(m_mat);
    mat4 light_mat = light_bsvp_mat * m_mat;

    glUniformMatrix4fv(ul_light_mat, 1, 0, &light_mat[0][0]);
    glUniformMatrix4fv(ul_mv_mat, 1, 0, &mv_mat[0][0]);
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, &mvp_mat[0][0]);
    glUniformMatrix4fv(ul_mv_mat_it, 1, 0, &mv_mat_it[0][0]);
  }
  virtual void attach_shaders() {
    // use_program
    attach(
      GL_VERTEX_SHADER, "shader2/use_directional_shadowmap.vs.glsl");

    string_vector sv;
    sv.push_back("#version 120\n");
    sv.push_back("#define LIGHT_COUNT 8\n");
    sv.push_back(stream_util::read_resource("shader2/blinn.frag"));
    attach(
      GL_FRAGMENT_SHADER, sv, "shader2/use_directional_shadowmap.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_light_mat, "light_mat");
    uniform_location(&ul_mv_mat_it, "mv_mat_it");
    uniform_location(&ul_mv_mat, "mv_mat");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_bias, "bias");
    uniform_location(&ul_depth_map, "depth_map");
    uniform_location(&ul_shadow_map, "shadow_map");
    uniform_location(&ul_use_sampler2_d_shadow, "use_sampler2_d_shadow");

    lm.bind_uniform_locations(object, "lm");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(object, ss.str());
    }
    mtl.bind_uniform_locations(object, "mtl");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
  }

} use_prg;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "directional_shadowmap";
    m_info.display_mode = GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = WINDOWS_WIDTH;
    m_info.wnd_height = WINDOWS_HEIGHT;
  }

  template<typename T>
  void render(T& prg) {
    glUseProgram(prg);
    prg.update_frame();
    mat4 m_mat(1);

    m_mat = glm::translate(vec3(0, 0, 3));
    prg.update_uniforms(m_mat);
    sphere0.draw();

    m_mat = mat4(1);
    prg.update_uniforms(m_mat);
    glNormal3f(0, 0, 1);
    plane0.draw();

    // create a wall
    m_mat = glm::translate(vec3(-10, 0, 0)) * glm::rotate(90.f, vec3(0, 1, 0));
    prg.update_uniforms(m_mat);
    glNormal3f(0, 0, 1);
    plane1.draw();

    glFlush();
  }
  void render_shadowmap() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glViewport(0, 0, shadow_width, shadow_height);
    glClear(GL_DEPTH_BUFFER_BIT);

    glCullFace(GL_FRONT);

    // copy texture
    render(render_prg);

    // you can use the same depth texture for both sample2D and
    // sample2_d_shadow,
    // although opengl will complain sampler type and depth compare discrapancy.
    // it's properbally safer to make a copy for shadow map.
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow_map);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 0, 0, shadow_width,
      shadow_height, 0);
  }

  void setup_camera() {}

  void display(void) {
    render_shadowmap();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glViewport(0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, depth_map);

    glCullFace(GL_BACK);
    render(use_prg);

    glUseProgram(0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glWindowPos2i(10, 492);

    GLchar info[512];
    sprintf(info,
      "q  : place camera at light \n"
      "wW : camera position : %.2f %.2f %.2f\n"
      "e  : use use_sampler2_d_shadow : %d\n"
      "r  : filter : %s\n"
      "fF : shadow resolution : %d\n"
      "iI : light_top : %.2f\n"
      "jJ : light_near : %.2f\n"
      "KK : light_far : %.2f\n"
      "lL : light_position : %.2f %.2f %.2f\n"
      ";: : bias : %.3f\n"
      "m: GL_CULL_FACE : %d\n"
      "fps : %f",
      camera_pos[0], camera_pos[1], camera_pos[2], use_sampler2_d_shadow,
      filter_strings[filter], shadow_width, light_top, light_near, light_far,
      lights[0].position[0], lights[0].position[1], lights[0].position[2], bias,
      glIsEnabled(GL_CULL_FACE), m_fps);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

    glutSwapBuffers();
  }

  void create_scene(void) {
    glClearColor(0.0, 0.5, 1.0, 0.0);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    // depth map
    glGenTextures(1, &depth_map);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filters[filter]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filters[filter]);
    GLfloat border_color[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_width,
      shadow_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

    // shadow map
    glGenTextures(1, &shadow_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, shadow_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, border_color);

    // setup texture compare, this will be used by shadow2D
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
    glTexParameteri(
      GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadow_width, shadow_height, 0,
    // GL_RGBA,
    // GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map, 0);
    glDrawBuffer(GL_NONE);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");

    // dir light
    light_source dir_light;
    dir_light.position = vec4(-8, 8, 8, 0);
    dir_light.diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    dir_light.specular = vec4(1.0, 1.0, 1.0, 1.0);
    dir_light.ambient = vec4(0.0, 0.0, 0.0, 1.0);
    lights.push_back(dir_light);

    mtl.ambient = vec4(0.2, 0.2, 0.2, 1.0);
    mtl.diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    mtl.specular = vec4(0.0, 0.0, 0.0, 1.0);
    mtl.emission = vec4(0.0, 0.0, 0.0, 1.0);
    mtl.shininess = 50.0;

    lm.local_viewer = 1;

    render_prg.init();
    use_prg.init();

    sphere0.include_normal(true);
    sphere0.build_mesh();
    plane0.include_normal(true);
    plane0.build_mesh();
    plane1.include_normal(true);
    plane1.build_mesh();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    zxd::wnd_aspect = wnd_aspect();
  }

  void mouse(int button, int state, int x, int y) {
    switch (button) {
      default:
        break;
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
        camera_at_light = !camera_at_light;
        break;

      case 'w': {
        float d = glm::length(camera_pos);
        vec3 n = glm::normalize(camera_pos);
        camera_pos = n * (d - 0.5f);
      } break;

      case 'W': {
        float d = glm::length(camera_pos);
        vec3 n = glm::normalize(camera_pos);
        camera_pos = n * (d + 0.5f);
      } break;

      case 'e': {
        use_sampler2_d_shadow = !use_sampler2_d_shadow;
        // toggle compare mode,  you can't use regular sample2D but tunn on
        // texture compare
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
        // use_sampler2_d_shadow ? GL_COMPARE_R_TO_TEXTURE : GL_NONE);
      } break;

      case 'r': {
        filter ^= 1;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filters[filter]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filters[filter]);
      } break;

      case 'f':
        shadow_width *= 2;
        shadow_height *= 2;
        // reset texture size
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth_map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_width,
          shadow_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        break;

      case 'F':
        shadow_width /= 2;
        shadow_height /= 2;
        // reset texture size
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, depth_map);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadow_width,
          shadow_height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
        break;

      case 'i':
        light_top += 0.5;
        break;

      case 'I':
        light_top -= 0.5;
        break;
      case 'j':
        light_near += 0.5;
        break;

      case 'J':
        light_near -= 0.5;
        break;

      case 'k':
        light_far += 0.5;
        break;

      case 'K':
        light_far -= 0.5;
        break;

      case 'l':
        lights[0].position[0] += 0.5;
        lights[0].position[1] -= 0.5;
        lights[0].position[2] -= 0.5;
        break;

      case 'L':
        lights[0].position[0] -= 0.5;
        lights[0].position[1] += 0.5;
        lights[0].position[2] += 0.5;
        break;

      case ';':
        bias += 0.001f;
        break;

      case ':':
        bias -= 0.001f;
        bias = std::max(0.0f, bias);
        break;

      case 'm': {
        GLboolean e = glIsEnabled(GL_CULL_FACE);
        if (e)
          glDisable(GL_CULL_FACE);
        else
          glEnable(GL_CULL_FACE);
      } break;

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
