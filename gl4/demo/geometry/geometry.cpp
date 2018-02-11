#include "app.h"
#include "bitmaptext.h"
#include "sphere.h"
#include "common.h"
#include "light.h"
#include <sstream>
#include "glenumstring.h"
#include "cuboid.h"
#include "cone.h"
#include "stateutil.h"
#include "cylinder.h"
#include "torus.h"
#include "xyplane.h"
#include "disk.h"
#include "bezier.h"
#include "nurb.h"
#include "beziersurface.h"
#include "points.h"
#include "nurbsurface.h"

namespace zxd {

GLuint diffuse_map;
GLuint diffuse_map1d;

blinn_program blinn_prg;
normal_viewer_program nv_prg;

struct texline_program : public zxd::program {
  // GLint ul_eye;
  GLint al_vertex;
  GLint al_texcoord;

  GLint ul_diffuse_map;

  virtual void update_model(const mat4 &_m_mat) {
    m_mat = _m_mat;
    mvp_mat = p_mat * v_mat * m_mat;

    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  }
  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/texline.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/texline.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_texcoord = attrib_location("texcoord");
  };
} texline_prg;

class geometry_app : public app {
protected:
  GLboolean m_render_normal;
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
  bezier m_bezier;
  nurb m_nurb;
  bezier_surface m_bezier_surface;
  nurb_surface m_nurb_surface;

public:
  geometry_app() : m_camera_pos(0, -8, 8), m_render_normal(GL_FALSE) {}

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
    blinn_prg.with_texcoord = 1;
    blinn_prg.init();
    blinn_prg.p_mat =
      glm::perspective(glm::radians(45.0f), wnd_aspect(), 0.1f, 30.0f);
    blinn_prg.v_mat = glm::lookAt(m_camera_pos, vec3(0, 0, 0), vec3(0, 0, 1));
    set_v_mat(&blinn_prg.v_mat);

    blinn_prg.bind_lighting_uniform_locations(
      m_lights, m_light_model, m_material);

    nv_prg.init();
    nv_prg.p_mat = blinn_prg.p_mat;

    texline_prg.init();
    texline_prg.p_mat = blinn_prg.p_mat;

    // geometry
    m_sphere.build_mesh();

    m_cuboid.build_mesh();

    m_cone.build_mesh();

    m_cylinder.base(1);
    m_cylinder.top(0.5);
    m_cylinder.build_mesh();

    m_torus.rings(32);
    m_torus.sides(32);
    m_torus.build_mesh();

    m_xyplane.slice(8);
    m_xyplane.width(8);
    m_xyplane.height(8);
    m_xyplane.left(-4);
    m_xyplane.bottom(-4);
    m_xyplane.build_mesh();

    m_disk0.build_mesh();

    m_disk1.start(fpi4);
    m_disk1.sweep(fpi2);
    m_disk1.build_mesh();

    {
      vec3_vector2 vv;
      for (int i = 0; i < 5; ++i) {
        vec3_vector v;
        for (int j = 0; j < 5; ++j) {
          v.push_back(
            vec3(-1 + i * 0.5f, -1 + j * 0.5f, glm::linearRand(0.0f, 1.0f)));
        }
        vv.push_back(v);
      }

      m_bezier_surface.ctrl_points(vv);
      m_bezier_surface.build_mesh();
    }

    {
      vec4_vector2 vv;
      for (int i = 0; i < 5; ++i) {
        vec4_vector v;
        for (int j = 0; j < 5; ++j) {
          v.push_back(
            vec4(-1 + i * 0.5f, -1 + j * 0.5f, glm::linearRand(0.0f, 1.0f), 1));
        }
        vv.push_back(v);
      }

      m_nurb_surface.ctrl_points(vv);
      m_nurb_surface.uniform_knots();
      m_nurb_surface.udegree(3);
      m_nurb_surface.vdegree(3);
      m_nurb_surface.build_mesh();
    }

    {
      vec3_vector ctrl_points;
      ctrl_points.push_back(vec3(0));
      ctrl_points.push_back(vec3(1, 0, 1));
      ctrl_points.push_back(vec3(-1, 0, 2));
      ctrl_points.push_back(vec3(0, 0, 3));
      m_bezier.ctrl_points(ctrl_points);
      m_bezier.build_mesh(-1, 1);
    }

    {
      vec4_vector ctrl_points;
      ctrl_points.push_back(vec4(0, 0, 0, 1));
      ctrl_points.push_back(vec4(5, 0, 5, 5));
      ctrl_points.push_back(vec4(-5, 0, 10, 5));
      ctrl_points.push_back(vec4(0, 0, 3, 1));

      m_nurb.degree(3);
      m_nurb.ctrl_points(ctrl_points);
      m_nurb.uniform_knots();
      m_nurb.build_mesh(-1, 1);
    }

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

    glGenTextures(1, &diffuse_map1d);
    glBindTexture(GL_TEXTURE_1D, diffuse_map1d);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, image_width, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &image.front());
  }

  virtual void update() {}

  void render_blinn(geometry_base &gm, const vec3 &translation) {
    blinn_prg.use();

    mat4 m_mat = glm::translate(translation);
    blinn_prg.update_model(m_mat);
    gm.bind(blinn_prg.al_vertex, blinn_prg.al_normal, blinn_prg.al_texcoord);
    gm.draw();

    // bad performance
    if (m_render_normal) {
      nv_prg.use();
      nv_prg.update_model(m_mat);
      gm.bind(nv_prg.al_vertex, nv_prg.al_normal, -1);
      gm.draw();
    }
  }

  void render_texline(geometry_base &gm, const vec3 &translation) {
    texline_prg.use();

    glUniform1i(texline_prg.ul_diffuse_map, 0);
    mat4 m_mat = glm::translate(translation);
    texline_prg.update_model(m_mat);
    gm.bind(texline_prg.al_vertex, -1, texline_prg.al_texcoord);
    gm.draw();
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(blinn_prg);
    blinn_prg.update_lighting_uniforms(m_lights, m_light_model, m_material);
    nv_prg.v_mat = blinn_prg.v_mat;
    texline_prg.v_mat = blinn_prg.v_mat;

    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    render_blinn(m_xyplane, vec3(0, 0, -2));
    render_blinn(m_disk0, vec3(2, -2, -1));
    render_blinn(m_disk1, vec3(4, -2, -1));
    render_blinn(m_sphere, vec3(1.0f));
    render_blinn(m_cuboid, vec3(2, 0, 0));
    render_blinn(m_cone, vec3(-2, 0, 0));
    render_blinn(m_cylinder, vec3(0, 2, 0));
    render_blinn(m_torus, vec3(0, -2.5, 0));
    render_blinn(m_bezier_surface, vec3(-3, -3, 0));
    {
      glPointSize(3);
      draw_points(m_bezier_surface.ctrl_points(), blinn_prg.mvp_mat);
      glPointSize(1);
    }
    render_blinn(m_nurb_surface, vec3(3, -3, 0));
    {
      glPointSize(3);
      draw_points(m_nurb_surface.ctrl_points(), blinn_prg.mvp_mat);
      glPointSize(1);
    }

    glBindTexture(GL_TEXTURE_1D, diffuse_map1d);

    render_texline(m_bezier, glm::vec3(0, -2.5, 0));
    glPointSize(3);
    draw_points(m_bezier.ctrl_points(), texline_prg.mvp_mat);
    glPointSize(1);
    render_texline(m_nurb, glm::vec3(2, -2, -1));
    glPointSize(3);
    draw_points(m_nurb.ctrl_points(), texline_prg.mvp_mat);
    glPointSize(1);

    GLint cull_face;
    glGetIntegerv(GL_CULL_FACE_MODE, &cull_face);

    GLint polygon_mode;
    glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
    std::stringstream ss;
    ss << "q : polygon mode : " << gl_polygon_mode_to_string(polygon_mode)
       << std::endl;
    ss << "w : cullface : " << GLint(glIsEnabled(GL_CULL_FACE)) << std::endl;
    ss << "e : depth : " << GLint(glIsEnabled(GL_DEPTH_TEST)) << std::endl;
    ss << "r : cullface : " << gl_cull_face_mode_to_string(cull_face)
       << std::endl;
    ss << "u : render normal : " << static_cast<GLint>(m_render_normal)
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

        case GLFW_KEY_U:
          m_render_normal ^= 1;
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
  zxd::geometry_app app;
  app.run();
}
