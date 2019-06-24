/*
 * create a bunch of billboards, all in random position
 */
#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"


namespace zxd {

GLuint num_billboards = 1000;
GLuint method = 0;  // 0 : billboard_align, 1 : billboard_z, 2 : billboard_eye
GLuint tex;
std::string methods[] = {
  "align to camera rotation",
  "rotate z to center_to_camera",
  "look at camera, use camera up"};

glm::mat4 v_mat;
glm::mat4 p_mat;

struct program0 : public zxd::program {
  GLint ul_diffuse_map;
  GLint ul_v_mat;
  GLint ul_vp_mat;

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "shader4/billboard_align.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/tex2d.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_v_mat, "v_mat");
    uniform_location(&ul_vp_mat, "vp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "translation");
  };
};

struct program1 : public zxd::program {
  GLint ul_diffuse_map;
  GLint ul_camera_pos;
  GLint ul_vp_mat;

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "shader4/billboard_z.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/tex2d.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_vp_mat, "vp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_camera_pos, "camera_pos");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "translation");
  };
};

struct program2 : public zxd::program {
  GLint ul_diffuse_map;
  GLint ul_camera_pos;
  GLint ul_camera_up;
  GLint ul_vp_mat;

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "shader4/billboard_eye.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/tex2d.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_vp_mat, "vp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_camera_pos, "camera_pos");
    uniform_location(&ul_camera_up, "camera_up");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "translation");
  };
};

program0 program0;
program1 program1;
program2 program2;

// clang-format off
glm::vec4 vertices[] = { 
  vec4(-0.5, 0.5, 0, 1), // x y s t
  vec4(-0.5, -0.5, 0, 0),
  vec4(0.5, 0.5, 1, 1),
  vec4(0.5, -0.5, 1, 0)
};
// clang-format on

GLuint vao;
GLuint vbo;
vec3_vector translations;

class billboard_app : public app {
protected:
  bitmap_text m_text;

public:
  billboard_app() {}

protected:
  virtual void init_info() {
    app::init_info();
    m_info.title = "billboard";
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    // init program
    program0.init();
    program1.init();
    program2.init();

    v_mat = glm::lookAt(vec3(0, -100, 100), vec3(0), vec3(0, 0, 1));
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);

    set_v_mat(&v_mat);

    // texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto image = create_chess_image(64, 64, 8, 8);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &image.front());
    //glGenerateMipmap(GL_TEXTURE_2D);

    // create vertex buffer
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), value_ptr(vertices[0]),
      GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
    glVertexAttribDivisor(1, 1);

    // init billboards
    reset_billboards();
  }

  void reset_billboards() {
    translations.clear();
    translations.reserve(num_billboards);
    for (int i = 0; i < num_billboards; ++i) {
      translations.push_back(
        glm::linearRand(glm::vec3(-30.0f), glm::vec3(30.0f)));
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, translations.size() * sizeof(vec3),
      value_ptr(translations[0]), GL_STATIC_DRAW);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, tex);

    glm::mat4 vp_mat = p_mat * v_mat;;

    if (method == 0) {
      glUseProgram(program0);

      glUniformMatrix4fv(
        program0.ul_vp_mat, 1, 0, value_ptr(vp_mat));
      glUniformMatrix4fv(program0.ul_v_mat, 1, 0, value_ptr(v_mat));
      glUniform1i(program0.ul_diffuse_map, 0);

    } else if (method == 1) {
      glUseProgram(program1);

      vec3 camera_pos = glm::inverse(v_mat)[3].xyz();

      glUniformMatrix4fv(
        program1.ul_vp_mat, 1, 0, value_ptr(vp_mat));
      glUniform1i(program1.ul_diffuse_map, 0);
      glUniform3fv(program1.ul_camera_pos, 1, value_ptr(camera_pos));

    } else {
      glUseProgram(program2);

      vec3 camera_pos = glm::inverse(v_mat)[3].xyz();
      vec3 camera_up = glm::row(v_mat, 1).xyz();

      glUniformMatrix4fv(
        program2.ul_vp_mat, 1, 0, value_ptr(vp_mat));
      glUniform1i(program2.ul_diffuse_map, 0);
      glUniform3fv(program2.ul_camera_pos, 1, value_ptr(camera_pos));
      glUniform3fv(program2.ul_camera_up, 1, value_ptr(camera_up));
    }

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, num_billboards);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : method : " << methods[method] << std::endl;
    ss << "wW : billboard number : " << num_billboards << std::endl;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(w, h);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_Q:
          method = ++method % 3;
          break;

        case GLFW_KEY_W:
          if (GLFW_MOD_SHIFT & mods) {
            num_billboards -= 100;
            reset_billboards();
          } else {
            num_billboards += 100;
            reset_billboards();
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

  virtual void glfw_mouse_wheel(GLFWwindow *wnd, double xoffset, double yoffset) {
    app::glfw_mouse_wheel(wnd, xoffset, yoffset);
  }
  virtual void glfw_char(GLFWwindow *wnd, unsigned int codepoint) {
    app::glfw_char(wnd, codepoint);
  }
  virtual void glfw_charmod(GLFWwindow *wnd, unsigned int codepoint, int mods) {
    app::glfw_charmod(wnd, codepoint, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::billboard_app app;
  app.run();
}
