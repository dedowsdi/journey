/*
 * Parallax map.
 *
 * Normalmap tried to use coarse grained geometry instead of fine grained
 * geometry, it reserved normals of fined grained geometry in normal map(tangent
 * space), but it doesn't reserve the surface detail, tangent space surface is
 * always flat, which means the vertex point you look at in a coarsed grained
 * geometry can not be the same vertex point as you llok at it's fine grained
 * cousin. Parallax map is used to overcome this problem, it was applied on
 * trangent space, with an extra depth map(invert of height map).  In tangent
 * space xy component of viewdir is aligned to uv, and the offset is propotion
 * to depth (in a positive random way), so we can approximate the offset based
 * on depth, and apply the offset to texcoord to retrieve normal from normal
 * map.
 *
 */
#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "common.h"
#include "light.h"
#include "quad.h"
#include "texutil.h"
#include "program.h"

namespace zxd {

struct parallax_program : public program {
  GLint al_vertex;
  GLint al_normal;
  GLint al_tangent;
  GLint al_texcoord;
  GLint ul_normal_map;
  GLint ul_diffuse_map;
  GLint ul_depth_map;
  GLint ul_m_camera;
  GLint ul_height_scale;

  GLuint parallax_method{0};

  virtual void update_model(const mat4 &_m_mat) {
    m_mat = _m_mat;
    mv_mat = v_mat * m_mat;
    mv_mat_i = glm::inverse(mv_mat);
    m_mat_i = glm::inverse(m_mat);
    mvp_mat = p_mat * mv_mat;
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
  };
  virtual void attach_shaders() {
    string_vector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 1\n");
    attach(
      GL_VERTEX_SHADER, sv, "data/shader/parallaxmap.vs.glsl");
    std::stringstream ss;
    ss << "#define PARALLAX_METHOD " << parallax_method << std::endl;
    sv.push_back(ss.str());
    sv.push_back(read_file("data/shader/blinn.frag"));
    attach(
      GL_FRAGMENT_SHADER, sv, "data/shader/parallaxmap.fs.glsl");

    name("parallaxmap");
  }

  virtual void bind_uniform_locations() {
    // uniform_location(&ul_eye, "eye");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_normal_map, "normal_map");
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_depth_map, "depth_map");
    uniform_location(&ul_m_camera, "m_camera");
    uniform_location(&ul_height_scale, "height_scale");
  }
  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_normal = attrib_location("normal");
    al_tangent = attrib_location("tangent");
    al_texcoord = attrib_location("texcoord");
  };
} prg;

GLuint normal_map, diffuse_map, depth_map;
std::vector<zxd::light_source> lights;
zxd::light_model light_model;
zxd::material material;
quad q;
GLfloat height_scale = 0.05f;
std::string parallax_methods[] = {"parallax_occlusion_map", "parallaxSteepMap",
  "parallax_map_with_offset", "parallaxMapWithoutOffset"};

class normal_map_app : public app {
protected:
  bitmap_text m_text;

public:
  normal_map_app() {}

  virtual void init_info() {
    app::init_info();
    m_info.title = "hello world";
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    prg.parallax_method = 3;
    prg.init();
    prg.p_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 20.0f);
    prg.v_mat = glm::lookAt(vec3(0, -5, 5), vec3(0.0f), vec3(0, 1, 0));

    // init quad
    q.build_mesh(1, 1, 1);
    q.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord, prg.al_tangent);

    // load maps
    fipImage diffuse_image = zxd::fipLoadImage("data/texture/bricks2.jpg");
    fipImage normal_image =
      zxd::fipLoadImage("data/texture/bricks2_normal.jpg");
    fipImage depth_image = zxd::fipLoadImage("data/texture/bricks2_disp.jpg");

    glGenTextures(1, &diffuse_map);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuse_image.getWidth(),
      diffuse_image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      diffuse_image.accessPixels());

    glGenTextures(1, &normal_map);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normal_image.getWidth(),
      normal_image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      normal_image.accessPixels());

    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, depth_image.getWidth(),
      depth_image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      depth_image.accessPixels());

    // light
    zxd::light_source dir_light;
    dir_light.position = vec4(0, -1, 1, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);
    dir_light.linear_attenuation = 1.0f;

    lights.push_back(dir_light);

    light_model.local_viewer = 1;

    // material
    material.ambient = vec4(0.2);
    material.diffuse = vec4(0.8);
    material.specular = vec4(0.8);
    material.shininess = 50;

    set_v_mat(&prg.v_mat);
    bind_uniform_locations(prg);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw low quality mesh with normal map
    glUseProgram(prg);
    prg.update_model(mat4(1.0));
    glUniform1i(prg.ul_diffuse_map, 0);
    glUniform1i(prg.ul_normal_map, 1);
    glUniform1i(prg.ul_depth_map, 2);
    glUniform1f(prg.ul_height_scale, height_scale);

    // get camera model position
    vec3 camera = glm::column(prg.mv_mat_i, 3).xyz();
    glUniform3fv(prg.ul_m_camera, 1, glm::value_ptr(camera));

    for (int i = 0; i < lights.size(); ++i) {
      lights[i].update_uniforms(prg.m_mat_i);
    }
    light_model.update_uniforms();
    material.update_uniforms();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depth_map);

    q.draw();

    glEnable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : parallax method : " << parallax_methods[prg.parallax_method]
       << std::endl;
    ss << "w : height_scale : " << height_scale << std::endl;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(w, h);
  }

  virtual void bind_uniform_locations(zxd::program &prg) {
    light_model.bind_uniform_locations(prg.object, "lm");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(prg.object, ss.str());
    }
    material.bind_uniform_locations(prg.object, "mtl");
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_Q: {
          prg.parallax_method = (prg.parallax_method + 1) % 4;
          prg.clear();
          prg.init();
        } break;

          break;
        default:
          break;
      }
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      switch (key) {
        case GLFW_KEY_W:
          if (mods & GLFW_MOD_SHIFT) {
            height_scale -= 0.002;
            height_scale = glm::max(height_scale, 0.0f);
          } else {
            height_scale += 0.002;
          }

          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
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
  zxd::normal_map_app app;
  app.run();
}
