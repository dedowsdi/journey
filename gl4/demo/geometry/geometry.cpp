#include "app.h"
#include "bitmap_text.h"
#include "sphere.h"
#include "common.h"
#include "light.h"
#include <sstream>
#include "glEnumString.h"
#include "cuboid.h"
#include "cone.h"
#include "stateutil.h"
#include "cylinder.h"
#include "torus.h"
#include "xyplane.h"
#include "disk.h"

namespace zxd {

GLuint diffuse_map;

struct blinn_program : public zxd::program {
  // GLint ul_eye;
  GLint al_vertex;
  GLint al_normal;
  GLint al_texcoord;
  GLint ul_diffuse_map;

  virtual void update_model(const mat4 &_m_mat) {
    m_mat = _m_mat;
    // m_mat_i = glm::inverse(m_mat);
    mv_mat = v_mat * m_mat;
    mv_mat_it = glm::inverse(glm::transpose(mv_mat));
    mvp_mat = p_mat * mv_mat;

    glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
    glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  }
  virtual void attach_shaders() {
    string_vector sv;
    sv.push_back("#version 430 core\n#define WITH_TEX\n");
    attach_shader_source_and_file(
      GL_VERTEX_SHADER, sv, "data/shader/blinn.vs.glsl");
    sv.push_back("#define LIGHT_COUNT 3\n");
    sv.push_back(read_file("data/shader/blinn.frag"));
    attach_shader_source_and_file(
      GL_FRAGMENT_SHADER, sv, "data/shader/blinn.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    // uniform_location(&ul_eye, "eye");
    uniform_location(&ul_mv_mat, "mv_mat");
    uniform_location(&ul_mv_mat_it, "mv_mat_it");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_normal = attrib_location("normal");
    al_texcoord = attrib_location("texcoord");
  };
};

blinn_program prg;

class geometry_app : public app {
protected:
  bitmap_text m_text;
  vec3 m_camera_pos;
  std::vector<zxd::light_source> m_lights;
  zxd::light_model m_light_model;
  zxd::material m_material;
  sphere m_sphere;
  cuboid m_cuboid;
  cone m_cone;
  cylinder m_cylinder;
  torus m_torus;
  xyplane m_xyplane;
  disk m_disk0;
  disk m_disk1;

public:
  geometry_app() : m_camera_pos(0, -8, 8) {}

  virtual void init_info() {
    app::init_info();
    m_info.title = "geometry";
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);

    // light
    zxd::light_source dir_light;
    dir_light.position = vec4(1, -1, 1, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);

    m_lights.push_back(dir_light);

    // material
    m_material.shininess = 50;
    m_material.specular = vec4(1.0, 1.0, 1.0, 1.0);
    m_material.ambient = vec4(0.8);

    // text
    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    // program
    prg.init();
    prg.p_mat =
      glm::perspective(glm::radians(45.0f), wnd_aspect(), 0.1f, 30.0f);
    prg.v_mat = glm::lookAt(m_camera_pos, vec3(0, 0, 0), vec3(0, 0, 1));
    set_v_mat(&prg.v_mat);

    m_light_model.bind_uniform_locations(prg.object, "lm");
    for (int i = 0; i < m_lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      m_lights[i].bind_uniform_locations(prg.object, ss.str());
    }
    m_material.bind_uniform_locations(prg.object, "mtl");

    // geometry
    m_sphere.build_mesh();
    m_sphere.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord);

    m_cuboid.build_mesh();
    m_cuboid.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord);

    m_cone.build_mesh();
    m_cone.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord);

    m_cylinder.base(1);
    m_cylinder.top(0.5);
    m_cylinder.build_mesh();
    m_cylinder.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord);

    m_torus.rings(32);
    m_torus.sides(32);
    m_torus.build_mesh();
    m_torus.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord);

    m_xyplane.slice(8);
    m_xyplane.width(8);
    m_xyplane.height(8);
    m_xyplane.left(-4);
    m_xyplane.bottom(-4);
    m_xyplane.build_mesh();
    m_xyplane.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord);

    m_disk0.build_mesh();
    m_disk0.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord);

    m_disk1.start(fpi4);
    m_disk1.sweep(fpi2);
    m_disk1.build_mesh();
    m_disk1.bind(prg.al_vertex, prg.al_normal, prg.al_texcoord);

    // texture
    GLint image_width = 64;
    GLint image_height = 64;
    auto image = create_chess_image(image_width, image_height, 8, 8);

    glGenTextures(1, &diffuse_map);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, &image.front());
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, diffuse_map);

    glUseProgram(prg);

    glUniform1i(prg.ul_diffuse_map, 0);

    prg.v_mat_it = glm::inverse(glm::transpose(prg.v_mat));

    m_material.update_uniforms();
    m_light_model.update_uniforms();
    for (int i = 0; i < m_lights.size(); ++i) {
      m_lights[i].update_uniforms(prg.v_mat);
    }

    mat4 model;

    model = glm::translate(glm::vec3(0, 0, -2));
    prg.update_model(model);
    m_xyplane.draw();

    model = glm::translate(glm::vec3(2, -2, -1));
    prg.update_model(model);
    m_disk0.draw();

    model = glm::translate(glm::vec3(4, -2, -1));
    prg.update_model(model);
    m_disk1.draw();

    model = mat4(1.0f);
    prg.update_model(model);
    m_sphere.draw();

    model = glm::translate(glm::vec3(2, 0, 0));
    prg.update_model(model);
    m_cuboid.draw();

    model = glm::translate(glm::vec3(-2, 0, 0));
    prg.update_model(model);
    m_cone.draw();

    model = glm::translate(glm::vec3(0, 2, 0));
    prg.update_model(model);
    m_cylinder.draw();

    model = glm::translate(glm::vec3(0, -2.5, 0));
    prg.update_model(model);
    m_torus.draw();

    GLint cull_face;
    glGetIntegerv(GL_CULL_FACE_MODE, &cull_face);

    GLint polygon_mode;
    glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
    std::stringstream ss;
    ss << "q : polygon mode " << gl_polygon_mode_to_string(polygon_mode)
       << std::endl;
    ss << "w : cullface " << GLint(glIsEnabled(GL_CULL_FACE)) << std::endl;
    ss << "e : depth " << GLint(glIsEnabled(GL_DEPTH_TEST)) << std::endl;
    ss << "r : cullface " << gl_cull_face_mode_to_string(cull_face)
       << std::endl;

    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      capability_guard<GL_BLEND> blend(GL_TRUE);
      capability_guard<GL_CULL_FACE> cull_face(GL_FALSE);
      m_text.print(ss.str(), 10, 492);
    }
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    (void)wnd;
    m_info.wnd_width = w;
    m_info.wnd_width = h;
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_Q: {
          GLint polygon_mode;
          glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
          glPolygonMode(
            GL_FRONT_AND_BACK, GL_POINT + (polygon_mode - GL_POINT + 1) % 3);
        } break;
        case GLFW_KEY_W: {
          if (glIsEnabled(GL_CULL_FACE)) {
            glDisable(GL_CULL_FACE);
          } else {
            glEnable(GL_CULL_FACE);
          }
        } break;
        case GLFW_KEY_E: {
          if (glIsEnabled(GL_DEPTH_TEST)) {
            glDisable(GL_DEPTH_TEST);
          } else {
            glEnable(GL_DEPTH_TEST);
          }
        } break;
        case GLFW_KEY_R: {
          GLint cull_face;
          glGetIntegerv(GL_CULL_FACE_MODE, &cull_face);
          if (cull_face == GL_FRONT) {
            glCullFace(GL_BACK);
          } else if (cull_face == GL_BACK) {
            glCullFace(GL_FRONT_AND_BACK);
          } else {
            glCullFace(GL_FRONT);
          }
        } break;

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
  zxd::geometry_app app;
  app.run();
}
