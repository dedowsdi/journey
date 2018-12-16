/*
 * n spheres and n point light. each sphere has it's own material, each light
 * has it's own light property.
 *
 * defered rendering is used to implement scenes with tons of lights.
 * it's down in two steps:
 *  1. render everything you need into gbuffer.
 *  2. rendre a quad, extract data from gbuffer to do the lighting.
 *
 * in this way, no calculation wasted on failed(such as depth test) fragment.
 *
 * you can mix forward and defered rendering, as long as you record depth in a
 * gbuffer, and apply it later, or simplely use glBlitFramebuffer to copy depth
 * buffer.
 *
 */

#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "common.h"
#include "sphere.h"
#include "light.h"
#include "stream_util.h"

namespace zxd {

struct render_gbuffer_program : public zxd::program {

  virtual void update_model(const mat4 &_m_mat) {
    m_mat = _m_mat;
    mv_mat = v_mat * m_mat;
    mv_mat_it = glm::inverse(glm::transpose(mv_mat));
    mvp_mat = p_mat * mv_mat;

    glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
    glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  }
  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "shader/render_gbuffer.vs.glsl");
    attach(
      GL_FRAGMENT_SHADER, "shader/render_gbuffer.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    // uniform_location(&ul_eye, "eye");
    uniform_location(&ul_mv_mat, "mv_mat");
    uniform_location(&ul_mv_mat_it, "mv_mat_it");
    uniform_location(&ul_mvp_mat, "mvp_mat");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
  };
};

struct glinn_program : public zxd::program {
  GLint ul_g_vertex;
  GLint ul_g_normal;
  GLint ul_g_depth;
  GLint ul_g_emission;
  GLint ul_g_ambient;
  GLint ul_g_diffuse;
  GLint ul_g_specular;
  GLint ul_g_shininess;

  glinn_program() {}

  void update_uniforms() {
    glUniform1i(ul_g_vertex, 0);
    glUniform1i(ul_g_normal, 1);
    glUniform1i(ul_g_emission, 2);
    glUniform1i(ul_g_ambient, 3);
    glUniform1i(ul_g_diffuse, 4);
    glUniform1i(ul_g_specular, 5);
    glUniform1i(ul_g_shininess, 6);
    glUniform1i(ul_g_depth, 7);
  }

  virtual void reset(GLuint light_count) {
    if (object != -1) {
      glDeleteProgram(object);
    }

    object = glCreateProgram();
    attach(GL_VERTEX_SHADER, "shader/use_gbuffer.vs.glsl");
    string_vector sv;
    std::stringstream ss;
    ss << "#version 430 core" << std::endl;
    ss << "#define LIGHT_COUNT " << light_count << std::endl;
    sv.push_back(ss.str());
    sv.push_back(stream_util::read_resource("shader/blinn.frag"));
    attach(
      GL_FRAGMENT_SHADER, sv, "shader/use_gbuffer.fs.glsl");

    this->link();
    bind_uniform_locations();
    bind_attrib_locations();
  }

  virtual void bind_uniform_locations() {
    uniform_location(&ul_g_vertex, "g_vertex");
    uniform_location(&ul_g_normal, "g_normal");
    uniform_location(&ul_g_emission, "g_emission");
    uniform_location(&ul_g_ambient, "g_ambient");
    uniform_location(&ul_g_diffuse, "g_diffuse");
    uniform_location(&ul_g_specular, "g_specular");
    uniform_location(&ul_g_shininess, "g_shininess");
    uniform_location(&ul_g_depth, "g_depth");
  }

  virtual void bind_attrib_locations(){};
};

class defered : public app {
protected:
  bitmap_text m_text;
  GLint m_numspheres;
  GLint m_num_lights;
  glm::vec2 g_buffer_size;

  GLuint mg_vertex;
  GLuint mg_normal;

  GLuint mg_emission;
  GLuint mg_ambient;
  GLuint mg_diffuse;
  GLuint mg_specular;
  GLuint mg_shininess;

  GLuint mg_depth;

  // rtt
  GLuint m_fbo;

  sphere m_sphere;
  light_model m_light_model;
  std::vector<light_source> m_lights;
  std::vector<glm::mat4> m_sphere_m_mates;
  std::vector<material> m_sphere_materials;

  glinn_program m_glinn;
  render_gbuffer_program m_render_gbuffer_program;

protected:
  void render_gbuffer() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(m_render_gbuffer_program.object);

    GLuint gbuffres[] = {mg_vertex, mg_normal, mg_emission, mg_ambient,
      mg_diffuse, mg_specular, mg_shininess, mg_depth};

    for (int i = 0; i < sizeof(gbuffres) / sizeof(GLuint); ++i) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, gbuffres[i]);
    }

    for (int i = 0; i < m_numspheres; ++i) {
      m_render_gbuffer_program.update_model(m_sphere_m_mates[i]);
      m_sphere_materials[i].update_uniforms();
      m_sphere.draw();
    }
  }

  void use_gbuffer() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_glinn.object);

    m_glinn.update_uniforms();

    GLuint gbuffres[] = {mg_vertex, mg_normal, mg_emission, mg_ambient,
      mg_diffuse, mg_specular, mg_shininess, mg_depth};

    for (int i = 0; i < sizeof(gbuffres) / sizeof(GLuint); ++i) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, gbuffres[i]);
    }

    // update lights
    for (int i = 0; i < m_lights.size(); ++i) {
      m_lights[i].update_uniforms(m_render_gbuffer_program.v_mat);
    }
    m_light_model.update_uniforms();

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "defered rendering";
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, wnd_width(), wnd_height());

    m_numspheres = 100;
    m_num_lights = 20;
    g_buffer_size = vec2(wnd_width(), wnd_height());

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    // gbuffer textures
    glGenTextures(1, &mg_vertex);
    glBindTexture(GL_TEXTURE_2D, mg_vertex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, g_buffer_size[0],
      g_buffer_size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mg_normal);
    glBindTexture(GL_TEXTURE_2D, mg_normal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, g_buffer_size[0],
      g_buffer_size[1], 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mg_depth);
    glBindTexture(GL_TEXTURE_2D, mg_depth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, g_buffer_size[0],
      g_buffer_size[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    // gbuffer textures
    glGenTextures(1, &mg_diffuse);
    glBindTexture(GL_TEXTURE_2D, mg_diffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, g_buffer_size[0], g_buffer_size[1],
      0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mg_ambient);
    glBindTexture(GL_TEXTURE_2D, mg_ambient);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, g_buffer_size[0], g_buffer_size[1],
      0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mg_specular);
    glBindTexture(GL_TEXTURE_2D, mg_specular);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, g_buffer_size[0], g_buffer_size[1],
      0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mg_emission);
    glBindTexture(GL_TEXTURE_2D, mg_emission);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, g_buffer_size[0], g_buffer_size[1],
      0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mg_shininess);
    glBindTexture(GL_TEXTURE_2D, mg_shininess);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, g_buffer_size[0], g_buffer_size[1], 0,
      GL_RED, GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);

    GLuint gbuffres[] = {mg_vertex, mg_normal, mg_emission, mg_ambient,
      mg_diffuse, mg_specular, mg_shininess};

    for (int i = 0; i < sizeof(gbuffres) / sizeof(GLuint); ++i) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
        GL_TEXTURE_2D, gbuffres[i], 0);
    }
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mg_depth, 0);

    // clang-format off
    GLenum draw_buffers[] = {
      GL_COLOR_ATTACHMENT0,
      GL_COLOR_ATTACHMENT1,
      GL_COLOR_ATTACHMENT2,
      GL_COLOR_ATTACHMENT3,
      GL_COLOR_ATTACHMENT4,
      GL_COLOR_ATTACHMENT5,
      GL_COLOR_ATTACHMENT6,
    };
    // clang-format on

    glDrawBuffers(sizeof(draw_buffers) / sizeof(GLenum), draw_buffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer");

    m_render_gbuffer_program.init();
    m_render_gbuffer_program.p_mat =
      glm::perspective(glm::radians(45.0f), wnd_aspect(), 10.0f, 50.0f);
    m_render_gbuffer_program.v_mat =
      glm::lookAt(vec3(0, -25, 25), vec3(0, 0, 0), vec3(0, 0, 1));

    set_v_mat(&m_render_gbuffer_program.v_mat);

    // sphere and lights
    m_sphere.include_normal(true);
    m_sphere.build_mesh();

    resetsphere();

    reset_light();
    m_light_model.bind_uniform_locations(m_glinn, "lm");
  }

  void resetsphere() {
    m_sphere_m_mates.clear();
    m_sphere_materials.clear();

    for (int i = 0; i < m_numspheres; ++i) {
      m_sphere_m_mates.push_back(
        glm::translate(glm::linearRand(vec3(-10, -10, -2), vec3(10, 10, 2))));

      material m;
      m.emission = glm::vec4(0);
      m.ambient = glm::vec4(glm::linearRand(vec3(0.0f), vec3(0.2f)), 1);
      m.diffuse = glm::vec4(glm::linearRand(vec3(0.5f), vec3(1.0f)), 1);
      m.specular = glm::vec4(glm::linearRand(vec3(0.5f), vec3(0.8f)), 1);
      m.shininess = glm::linearRand(1, 120);

      m.bind_uniform_locations(m_render_gbuffer_program, "mtl");

      m_sphere_materials.push_back(m);
    }
  }

  void reset_light() {
    m_glinn.reset(m_num_lights);
    m_lights.clear();

    for (int i = 0; i < m_num_lights; ++i) {
      light_source light;
      light.position =
        glm::vec4(glm::linearRand(vec3(-10, -10, -2), vec3(10, 10, 2)), 1);
      light.diffuse = glm::vec4(glm::linearRand(vec3(0.75f), vec3(1.0f)), 1);
      light.specular = glm::vec4(glm::linearRand(vec3(0.5f), vec3(1.0f)), 1);
      light.ambient = vec4(0);
      light.constant_attenuation = 1;
      light.linear_attenuation = 0.5;
      light.quadratic_attenuation = 0.25;
      std::stringstream ss;
      ss << "lights[" << i << "]";
      light.bind_uniform_locations(m_glinn.object, ss.str());
      m_lights.push_back(light);
    }

    m_light_model.bind_uniform_locations(m_glinn.object, "lm");
  }

  virtual void display() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    render_gbuffer();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    use_gbuffer();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : sphere number " << m_numspheres << std::endl;
    ss << "wW : light number " << m_num_lights << std::endl;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, 492);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(w, h);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    app::glfw_key(wnd, key, scancode, action, mods);
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_Q:
          m_numspheres += mods == GLFW_MOD_SHIFT ? -10 : 10;
          if (m_numspheres < 0) {
            m_numspheres = 0;
          }
          resetsphere();
          break;

        case GLFW_KEY_W:
          m_num_lights += mods == GLFW_MOD_SHIFT ? -1 : 1;
          if (m_num_lights < 0) {
            m_num_lights = 0;
          }
          reset_light();
        default:
          break;
      }
    }
  }

  virtual void glfw_mouse_button(
    GLFWwindow *wnd, int button, int action, int mods) {
    app::glfw_mouse_button(wnd, button, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y) {
    app::glfw_mouse_move(wnd, x, y);
  }

  virtual void glfw_mouse_wheel(
    GLFWwindow *wnd, double xoffset, double yoffset) {
    app::glfw_mouse_wheel(wnd, xoffset, yoffset);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::defered app;
  app.run();
}
