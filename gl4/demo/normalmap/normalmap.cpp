#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include "light.h"

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

namespace zxd {

// clang-format off
struct pyramid0 {
  GLuint vao;
  vec3 vertices[12] = {
    vec3(0, 0, 0.5) , vec3(-1, -1, 0) , vec3(1,  -1, 0) ,
    vec3(0, 0, 0.5) , vec3(1,  -1, 0) , vec3(1,  1,  0) ,
    vec3(0, 0, 0.5) , vec3(1,  1,  0) , vec3(-1, 1,  0) ,
    vec3(0, 0, 0.5) , vec3(-1, 1,  0) , vec3(-1, -1, 0)
  };
  vec3 normals[12];
  vec2 texcoords[12] = {
    vec2(0.5,0.5), vec2(0,0), vec2(1,0),
    vec2(0.5,0.5), vec2(1,0), vec2(1,1),
    vec2(0.5,0.5), vec2(1,1), vec2(0,1),
    vec2(0.5,0.5), vec2(0,1), vec2(0,0)
  };

} pyramid0;

struct pyramid1 {
  GLuint vao;
vec3 vertices[4] = {
  vec3(-1, 1,  0),
  vec3(-1, -1, 0),
  vec3(1,  1,  0),
  vec3(1,  -1, 0)
};
vec3 normals[4] = {
  vec3(0, 0, 1),
  vec3(0, 0, 1),
  vec3(0, 0, 1),
  vec3(0, 0, 1)
};
vec3 tangents[4];
vec2 texcoords[4] = {
   vec2(0,1),
   vec2(0,0), 
   vec2(1,1),
   vec2(1,0) 
};

}pyramid1;
// clang-format on

struct render_normalmap_program : public zxd::program {
  GLint al_texcoord;
  GLint al_normal;

  virtual void update_model(const mat4 &_m_mat) {}
  virtual void attach_shaders() {
    attach_shader_file(
      GL_VERTEX_SHADER, "data/shader/render_normalmap.vs.glsl");
    attach_shader_file(
      GL_FRAGMENT_SHADER, "data/shader/render_normalmap.fs.glsl");
  }
  virtual void bind_uniform_locations() {}

  virtual void bind_attrib_locations() {
    al_texcoord = attrib_location("texcoord");
    al_normal = attrib_location("normal");
  };
} render_normalmap_program;

struct use_normal_map_view_program : public zxd::program {
  GLint al_vertex;
  GLint al_normal;
  GLint al_tangent;
  GLint al_texcoord;
  GLint ul_normal_map;

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
    attach_shader_file(
      GL_VERTEX_SHADER, "data/shader/use_normalmap_view.vs.glsl");
    string_vector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 1\n");
    sv.push_back(read_file("data/shader/blinn.frag"));
    attach_shader_source_and_file(
      GL_FRAGMENT_SHADER, sv, "data/shader/use_normalmap_view.fs.glsl");

    set_name("use_normalmap_view");
  }
  virtual void bind_uniform_locations() {
    // uniform_location(&ul_eye, "eye");
    uniform_location(&ul_mv_mat, "mv_mat");
    uniform_location(&ul_mv_mat_it, "mv_mat_it");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_normal_map, "normal_map");
  }

  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_normal = attrib_location("normal");
    al_tangent = attrib_location("tangent");
    al_texcoord = attrib_location("texcoord");
  };
} use_normal_map_view_program;

struct use_normal_map_tangent_program : public program {
  GLint al_vertex;
  GLint al_normal;
  GLint al_tangent;
  GLint al_texcoord;
  GLint ul_normal_map;
  GLint ul_model_camera;

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
    attach_shader_source_and_file(
      GL_VERTEX_SHADER, sv, "data/shader/use_normalmap_tangent.vs.glsl");
    sv.push_back(read_file("data/shader/blinn.frag"));
    attach_shader_source_and_file(
      GL_FRAGMENT_SHADER, sv, "data/shader/use_normalmap_tangent.fs.glsl");

    set_name("use_normalmap_tangent");
  }
  virtual void bind_uniform_locations() {
    // uniform_location(&ul_eye, "eye");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_normal_map, "normal_map");
    uniform_location(&ul_model_camera, "model_camera");
  }
  virtual void bind_attrib_locations() {
    al_vertex = attrib_location("vertex");
    al_normal = attrib_location("normal");
    al_tangent = attrib_location("tangent");
    al_texcoord = attrib_location("texcoord");
  };
} use_normal_map_tangent_program;

GLuint normal_map;
std::vector<zxd::light_source> lights;
zxd::light_model light_model;
zxd::material material;
GLint light_space = 0;  // 0 : view, 1 : tangent

class normal_map_app : public app {
protected:
  bitmap_text m_text;

public:
  normal_map_app() {}

  virtual void init_info() {
    app::init_info();
    m_info.title = "hello world";
    m_info.wnd_width = IMAGE_WIDTH * 2;
    m_info.wnd_height = IMAGE_HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    render_normalmap_program.init();
    use_normal_map_view_program.init();
    use_normal_map_view_program.p_mat =
      glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 20.0f);
    use_normal_map_view_program.v_mat =
      glm::lookAt(vec3(0, -5, 5), vec3(0.0f), vec3(0, 1, 0));
    use_normal_map_tangent_program.init();
    use_normal_map_tangent_program.p_mat = use_normal_map_view_program.p_mat;
    use_normal_map_tangent_program.v_mat = use_normal_map_view_program.v_mat;

    // create mesh data
    zxd::generate_face_normals(
      pyramid0.vertices, pyramid0.vertices + 12, pyramid0.normals);

    mat4 tbn = zxd::get_tangetn_basis(pyramid1.vertices[0],
      pyramid1.vertices[1], pyramid1.vertices[2], pyramid1.texcoords[0],
      pyramid1.texcoords[1], pyramid1.texcoords[2], &pyramid1.normals[0]);
    mat4 tbn_i = inverse(tbn);
    vec3 tangent = vec3(glm::column(tbn, 0));
    // vec3 tangent = glm::row(tbn, 0).xyz();

    for (int i = 0; i < 4; ++i) pyramid1.tangents[i] = tangent;

    // transform modle normals to tbn space
    for (int i = 0; i < 12; ++i)
      pyramid0.normals[i] = zxd::transform_vector(tbn_i, pyramid0.normals[i]);

    // create vertex arries
    glGenVertexArrays(1, &pyramid0.vao);
    glBindVertexArray(pyramid0.vao);

    setup_vertex_al_builtin_array(
      render_normalmap_program.al_normal, pyramid0.normals);
    setup_vertex_al_builtin_array(
      render_normalmap_program.al_texcoord, pyramid0.texcoords);

    glGenVertexArrays(1, &pyramid1.vao);
    glBindVertexArray(pyramid1.vao);

    setup_vertex_al_builtin_array(
      use_normal_map_view_program.al_vertex, pyramid1.vertices);
    setup_vertex_al_builtin_array(
      use_normal_map_view_program.al_normal, pyramid1.normals);
    setup_vertex_al_builtin_array(
      use_normal_map_view_program.al_tangent, pyramid1.tangents);
    setup_vertex_al_builtin_array(
      use_normal_map_view_program.al_texcoord, pyramid1.texcoords);

    glGenTextures(1, &normal_map);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, IMAGE_WIDTH, IMAGE_HEIGHT, 0,
      GL_RGB, GL_UNSIGNED_BYTE, 0);

    // light
    zxd::light_source dir_light;
    dir_light.position = vec4(1, -1, 1, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);
    dir_light.linear_attenuation = 1.0f;

    lights.push_back(dir_light);

    light_model.local_viewer = 1;

    // material
    material.ambient = vec4(0.2);
    material.diffuse = vec4(0.8);
    material.specular = vec4(1.0);
    material.shininess = 50;

    bind_uniform_locations(use_normal_map_view_program);

    set_v_mat(&use_normal_map_view_program.v_mat);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
  }

  void generate_normal_map() {
    glViewport(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glScissor(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(pyramid0.vao);
    glUseProgram(render_normalmap_program);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    glBindTexture(GL_TEXTURE_2D, normal_map);
    glCopyTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, 0);
  }
  virtual void display() {
    generate_normal_map();

    glViewport(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glEnable(GL_SCISSOR_TEST);
    glScissor(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw low quality mesh with normal map
    if (light_space == 0) {
      glUseProgram(use_normal_map_view_program);
      use_normal_map_view_program.update_model(mat4(1.0));
      glUniform1i(use_normal_map_view_program.ul_normal_map, 0);

      for (int i = 0; i < lights.size(); ++i) {
        lights[i].update_uniforms(use_normal_map_view_program.v_mat);
      }
    } else {
      glUseProgram(use_normal_map_tangent_program);
      use_normal_map_tangent_program.update_model(mat4(1.0));
      glUniform1i(use_normal_map_tangent_program.ul_normal_map, 0);

      // get camera model position
      vec3 camera =
        glm::column(use_normal_map_tangent_program.mv_mat_i, 3).xyz();
      glUniform3fv(use_normal_map_tangent_program.ul_model_camera, 1,
        glm::value_ptr(camera));

      for (int i = 0; i < lights.size(); ++i) {
        lights[i].update_uniforms(use_normal_map_tangent_program.m_mat_i);
      }
    }
    light_model.update_uniforms();
    material.update_uniforms();

    glBindVertexArray(pyramid1.vao);

    glDisable(GL_SCISSOR_TEST);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, m_info.wnd_width, m_info.wnd_height);
    glEnable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : lighting space : " << (light_space == 0 ? "view" : "tangent")
       << std::endl;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(w, h);
  }

  virtual void bind_uniform_locations(zxd::program &program) {
    light_model.bind_uniform_locations(program.object, "lm");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(program.object, ss.str());
    }
    material.bind_uniform_locations(program.object, "mtl");
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_Q:
          light_space ^= 1;
          if (light_space == 0) {
            use_normal_map_view_program.v_mat = *m_v_mat;

            set_v_mat(&use_normal_map_view_program.v_mat);
            bind_uniform_locations(use_normal_map_view_program);
          } else {
            use_normal_map_tangent_program.v_mat = *m_v_mat;
            set_v_mat(&use_normal_map_tangent_program.v_mat);
            bind_uniform_locations(use_normal_map_tangent_program);
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
