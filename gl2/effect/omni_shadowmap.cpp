/*
 * cast shadow for point type light.
 *
 * similar to directional_shadowmap, except it use cube map as depth texture,
 * which means you need to render 6 times to create the depth cubemap.
 *
 * depth component of result of perspective component isn't linear, it's
 * precision is very bad if get_object() is not near the nearplane, so i store the
 * real linear depth in the depth map:
 *    depth = (z_distance - near_plane) / (far_plane - near_plane)
 *
 * to use the depth cubemap, you need a l2v(light to vertex) vector in world
 * space to get stored light space depth, you can also get current fragment
 * z_distance as max(abs(l2v.x), max(abs(l2v.y), abs(l2v.x)))
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

glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 light_p_mat;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512

GLuint shadow_width = WINDOWS_WIDTH;
GLuint shadow_height = WINDOWS_HEIGHT;
GLfloat wnd_aspect = WINDOWS_WIDTH / WINDOWS_HEIGHT;
GLfloat bias = 0.005;

GLuint fbo;
GLuint depth_cube_map;
GLfloat light_near = 0.1, light_far = 30;

material mtl;

const char* filter_strings[] = {"NEAREST", "LINEAR"};
GLenum filters[] = {GL_NEAREST, GL_LINEAR};
GLint filter = 0;

sphere sphere0(1, 16, 16);
xyplane plane0(-12, -12, 12, 12);

std::vector<zxd::light_source> lights;
light_model lm;

vec3 camera_pos = vec3(0, -15, 15);

struct render_program : public zxd::program {

  GLint ul_near_plane;
  GLint ul_far_plane;
  GLint ul_mvp_mat;

  mat4 light_v_mats[6];
  GLint index;  // current rendering index of cubemap

  render_program() {}

  void bind_uniform_locations() {
    ul_mvp_mat = get_uniform_location("mvp_mat");
    ul_near_plane = get_uniform_location("near_plane");
    ul_far_plane = get_uniform_location("far_plane");
  }

  void attach_shaders() {
    // render shadow program
    attach(
      GL_VERTEX_SHADER, "shader2/render_omni_shadowmap.vs.glsl");
    attach(
      GL_FRAGMENT_SHADER, "shader2/render_omni_shadowmap.fs.glsl");

    name("render_prg");
  }

  void reset_light_position() {
    vec3 light_position(
      lights[0].position[0], lights[0].position[1], lights[0].position[2]);

    // TODO understand up vector.
    light_v_mats[0] = glm::lookAt(
      light_position, light_position + vec3(1, 0, 0), vec3(0, -1, 0));

    light_v_mats[1] = glm::lookAt(
      light_position, light_position + vec3(-1, 0, 0), vec3(0, -1, 0));

    light_v_mats[2] = glm::lookAt(
      light_position, light_position + vec3(0, 1, 0), vec3(0, 0, 1));

    light_v_mats[3] = glm::lookAt(
      light_position, light_position + vec3(0, -1, 0), vec3(0, 0, -1));

    light_v_mats[4] = glm::lookAt(
      light_position, light_position + vec3(0, 0, 1), vec3(0, -1, 0));

    light_v_mats[5] = glm::lookAt(
      light_position, light_position + vec3(0, 0, -1), vec3(0, -1, 0));
  }

  virtual void update_uniforms(const glm::mat4& m_mat) {
    mat4 mvp_mat = light_p_mat * light_v_mats[index] * m_mat;

    glUniformMatrix4fv(ul_mvp_mat, 1, 0, &mvp_mat[0][0]);
    glUniform1f(ul_near_plane, light_near);
    glUniform1f(ul_far_plane, light_far);
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
  }
} render_prg;

struct use_program : public zxd::program {

  GLint ul_w_light_pos;
  GLint ul_depth_cube_map;
  GLint ul_bias;
  GLint ul_near_plane;
  GLint ul_far_plane;
  GLint ul_m_mat;
  GLint ul_mv_mat;
  GLint ul_mvp_mat;
  GLint ul_mv_mat_it;

  virtual void update_frame() {
    p_mat = glm::perspective<GLfloat>(45.0, wnd_aspect, 0.1, 100);
    v_mat = glm::lookAt(camera_pos, vec3(0, 0, 0), vec3(0, 0, 1));

    mtl.update_uniforms();
    lm.update_uniforms();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].update_uniforms(v_mat);
    }

    glUniform1f(ul_bias, bias);
    glUniform1i(ul_depth_cube_map, 0);
  }
  virtual void update_uniforms(const glm::mat4& m_mat) {
    mat4 mv_mat = v_mat * m_mat;
    mat4 mv_mat_it = glm::inverse(glm::transpose(mv_mat));
    mat4 mvp_mat = p_mat * mv_mat;
    // mat4 m_mat_i = glm::inverse(m_mat);

    glUniformMatrix4fv(ul_m_mat, 1, 0, &m_mat[0][0]);
    glUniformMatrix4fv(ul_mv_mat, 1, 0, &mv_mat[0][0]);
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, &mvp_mat[0][0]);
    glUniformMatrix4fv(ul_mv_mat_it, 1, 0, &mv_mat_it[0][0]);

    glUniform3f(ul_w_light_pos, lights[0].position[0], lights[0].position[1],
      lights[0].position[2]);
    glUniform1i(ul_depth_cube_map, 0);
    glUniform1f(ul_bias, bias);
    glUniform1f(ul_near_plane, light_near);
    glUniform1f(ul_far_plane, light_far);
  }

  virtual void attach_shaders() {
    // use_program
    attach(
      GL_VERTEX_SHADER, "shader2/use_omni_shadowmap.vs.glsl");

    string_vector sv;
    sv.push_back("#version 120\n");
    sv.push_back("#define LIGHT_COUNT 9\n");
    sv.push_back(stream_util::read_resource("shader2/blinn.frag"));
    attach(
      GL_FRAGMENT_SHADER, sv, "shader2/use_omni_shadowmap.fs.glsl");

    name("use_prg");
  }

  virtual void bind_uniform_locations() {
    ul_m_mat = get_uniform_location("m_mat");
    ul_mv_mat_it = get_uniform_location("mv_mat_it");
    ul_mv_mat = get_uniform_location("mv_mat");
    ul_mvp_mat = get_uniform_location("mvp_mat");
    ul_bias = get_uniform_location("bias");
    ul_w_light_pos = get_uniform_location("w_light_pos");
    ul_depth_cube_map = get_uniform_location("depth_cube_map");
    ul_bias = get_uniform_location("bias");
    ul_near_plane = get_uniform_location("near_plane");
    ul_far_plane = get_uniform_location("far_plane");

    lm.bind_uniform_locations(get_object(), "lm");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(get_object(), ss.str());
    }
    mtl.bind_uniform_locations(get_object(), "mtl");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
  }
} use_prg;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "omni_shadowmap";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = WINDOWS_WIDTH;
    m_info.wnd_height = WINDOWS_HEIGHT;
  }

  template<typename T>
  void render(T& program) {
    glm::mat4 model = glm::translate(vec3(0, 0, 2));
    program.update_uniforms(model);
    sphere0.draw();

    GLuint num_sphere = 8;
    GLfloat step_rotate = glm::pi<GLfloat>() * 2 / num_sphere;
    for (int i = 0; i < num_sphere; ++i) {
      vec4 pos = glm::rotate(step_rotate * i, vec3(0, 0, 1)) * vec4(5, 0, 2, 1);
      model = glm::translate(vec3(pos));

      program.update_uniforms(model);
      sphere0.draw();
    }

    model = glm::mat4(1.0);
    program.update_uniforms(model);
    plane0.draw();
  }

  void render_shadow_map() {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, shadow_width, shadow_height);

    glCullFace(GL_FRONT);
    glUseProgram(render_prg.get_object());

    // render 6 faces of cube map
    for (int i = 0; i < 6; ++i) {
      ZCGE(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, depth_cube_map, 0));

      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("incomplete frame buffer\n");

      glClear(GL_DEPTH_BUFFER_BIT);

      render_prg.index = i;
      render(render_prg);
    }
  }

  void display(void) {
    render_shadow_map();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glCullFace(GL_BACK);

    glUseProgram(use_prg.get_object());
    use_prg.update_frame();
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cube_map);

    render(use_prg);

    glUseProgram(0);
    glColor3f(1.0f, 0.0f, 0.0f);
    glWindowPos2i(10, 492);

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    GLchar info[1024];
    sprintf(info,
      "wW : camera position : %.2f %.2f %.2f\n"
      "r  : filter : %s\n"
      "fF : shadow resolution : %d\n"
      "jJ : light_near : %.2f\n"
      "KK : light_far : %.2f\n"
      "lL : light_position : %.2f %.2f %.2f\n"
      ";: : bias : %.3f\n"
      "m  : GL_CULL_FACE : %d",
      camera_pos[0], camera_pos[1], camera_pos[2], filter_strings[filter],
      shadow_width, light_near, light_far, lights[0].position[0],
      lights[0].position[1], lights[0].position[2], bias,
      glIsEnabled(GL_CULL_FACE) ? 1 : 0);
    glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

    glFlush();
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(GL_SMOOTH);

    light_source point_light;
    // light
    point_light.diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    point_light.specular = vec4(1.0, 1.0, 1.0, 1.0);
    point_light.ambient = vec4(0.0, 0.0, 0.0, 1.0);
    point_light.position = vec4(0, 0, 8, 0);
    point_light.constant_attenuation = 1;

    lights.push_back(point_light);

    lm.local_viewer = 1;

    // material
    mtl.ambient = vec4(0.2, 0.2, 0.2, 1.0);
    mtl.diffuse = vec4(1.0, 1.0, 1.0, 1.0);
    mtl.specular = vec4(0.0, 0.0, 0.0, 1.0);
    mtl.emission = vec4(0.0, 0.0, 0.0, 1.0);
    mtl.shininess = 50.0;

    // create depth cube map
    glGenTextures(1, &depth_cube_map);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cube_map);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    for (int i = 0; i < 6; ++i) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT16,
        shadow_width, shadow_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    }

    // create frame buffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    render_prg.init();
    light_p_mat = glm::perspective(
      glm::radians(90.0f), wnd_aspect(), light_near, light_far);
    render_prg.reset_light_position();

    use_prg.init();

    sphere0.include_normal(true);
    sphere0.build_mesh();
    plane0.slice(10);
    plane0.include_normal(true);
    plane0.build_mesh();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'q':
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

      case 'e':
        break;

      case 'r': {
        filter ^= 1;
        glTexParameteri(
          GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filters[filter]);
        glTexParameteri(
          GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filters[filter]);
      } break;

      case 'f':
        shadow_width *= 2;
        shadow_height *= 2;
        // reset texture size
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cube_map);
        for (int i = 0; i < 6; ++i) {
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            GL_DEPTH_COMPONENT16, shadow_width, shadow_height, 0,
            GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        }
        break;

      case 'F':
        shadow_width /= 2;
        shadow_height /= 2;
        // reset texture size
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cube_map);
        for (int i = 0; i < 6; ++i) {
          glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
            GL_DEPTH_COMPONENT16, shadow_width, shadow_height, 0,
            GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        }
        break;

      case 'i':
        break;

      case 'I':
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
        lights[0].position[2] += 0.5;
        render_prg.reset_light_position();
        break;

      case 'L':
        lights[0].position[2] -= 0.5;
        render_prg.reset_light_position();
        break;

      case ';':
        bias += 0.01f;
        break;

      case ':':
        bias -= 0.01f;
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
