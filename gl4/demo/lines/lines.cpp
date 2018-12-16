#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "common.h"

namespace zxd {

class lines : public app {
  struct line_stipple_program : public zxd::program {
    using program::update_model;
    GLint ul_color;
    GLint ul_viewport;
    GLint ul_pattern;
    GLint ul_factor;

    void update_model(
      const mat4 &m, const vec4 &color, GLuint pattern, GLint factor) {
      m_mat = m;
      mvp_mat = p_mat * v_mat * m_mat;

      glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
      glUniform4fv(ul_color, 1, value_ptr(color));
      glUniform1ui(ul_pattern, pattern);
      glUniform1i(ul_factor, factor);
    }
    virtual void attach_shaders() {
      attach(GL_VERTEX_SHADER, "shader/stipple.vs.glsl");
      attach(GL_GEOMETRY_SHADER, "shader/stipple.gs.glsl");
      attach(GL_FRAGMENT_SHADER, "shader/stipple.fs.glsl");

      p_mat = glm::ortho(0.0f, 400.0f, 0.0f, 150.0f);
      v_mat = mat4(1.0);
    }
    virtual void bind_uniform_locations() {
      uniform_location(&ul_mvp_mat, "mvp_mat");
      uniform_location(&ul_color, "color");
      uniform_location(&ul_viewport, "viewport");
      uniform_location(&ul_pattern, "pattern");
      uniform_location(&ul_factor, "factor");
    }

    virtual void bind_attrib_locations() {
      bind_attrib_location(0, "vertex");
    };
  };

protected:
  bitmap_text m_text;
  line_stipple_program m_program;
  GLuint m_vao;
  GLuint m_vertex_buffer;

public:
  lines() {}

  void draw_lines(GLenum mode, const vec2_vector &vertices, glm::vec4 color,
    GLuint pattern, GLint factor) {
    m_program.update_model(mat4(1.0), color, pattern, factor);

    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
      value_ptr(vertices[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glDrawArrays(mode, 0, vertices.size());
  }

  void draw_line(const glm::vec2 &p0, const glm::vec2 &p1, glm::vec4 color,
    GLuint pattern, GLint factor) {
    vec2_vector vertices;
    vertices.push_back(p0);
    vertices.push_back(p1);

    draw_lines(GL_LINES, vertices, color, pattern, factor);
  }

  virtual void init_info() {
    app::init_info();
    m_info.title = "hello world";
    m_info.wnd_width = 400;
    m_info.wnd_height = 150;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    m_program.init();

    // clang-format off
    //GLfloat vertices[][2] = {
      //{ 50.0,  125.0}, {150.0, 125.0},
      //{ 150.0, 125.0}, {250.0, 125.0},
      //{ 250.0, 125.0}, {350.0, 125.0},
      //{ 50.0,  100.0}, {150.0, 100.0},
      //{ 150.0, 100.0}, {250.0, 100.0},
      //{ 250.0, 100.0}, {350.0, 100.0}
    //};
    // clang-format on

    //// create lines
    // glGenVertexArrays(1, &m_vao);
    // glBindVertexArray(m_vao);
    // glGenBuffers(1, &m_vertex_buffer);
    // glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
    // GL_STATIC_DRAW);

    // glVertexAttribPointer(
    // m_program.al_vertex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // glEnableVertexAttribArray(m_program.al_vertex);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_program);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glUniform4iv(m_program.ul_viewport, 1, viewport);

    // dotted
    draw_line(
      glm::vec2(50.0, 125.0), glm::vec2(150.0, 125.0), vec4(1.0), 0x0101, 1);
    // dashed
    draw_line(
      glm::vec2(150.0, 125.0), glm::vec2(250.0, 125.0), vec4(1.0), 0x00ff, 1);
    // dash/dot/dash
    draw_line(
      glm::vec2(250.0, 125.0), glm::vec2(350.0, 125.0), vec4(1.0), 0x1c47, 1);

    /* in 2nd row, 3 wide lines, each with different stipple */
    glLineWidth(5.0);
    draw_line(
      glm::vec2(50.0, 100.0), glm::vec2(150.0, 100.0), vec4(1.0), 0x0101, 1);
    draw_line(
      glm::vec2(150.0, 100.0), glm::vec2(250.0, 100.0), vec4(1.0), 0x00ff, 1);
    draw_line(
      glm::vec2(250.0, 100.0), glm::vec2(350.0, 100.0), vec4(1.0), 0x1c47, 1);
    glLineWidth(1.0);

    /* in 3rd row, 6 lines, with dash/dot/dash stipple  */
    {
      vec2_vector vertices;
      for (int i = 0; i < 7; i++)
        vertices.push_back(glm::vec2(50.0 + ((GLfloat)i * 50.0), 75.0));
      draw_lines(GL_LINE_STRIP, vertices, glm::vec4(1), 0x1C47, 1);
    }

    /* in 4th row, 6 independent lines with same stipple  */
    for (int i = 0; i < 6; i++) {
      draw_line(glm::vec2(50.0 + ((GLfloat)i * 50.0), 50.0),
        glm::vec2(50.0 + ((GLfloat)(i + 1) * 50.0), 50.0), vec4(1.0), 0x1C47,
        1);
    }

    /* in 5th row, 1 line, with dash/dot/dash stipple    */
    /* and a stipple repeat factor of 5                  */
    draw_line(
      glm::vec2(50.0, 25.0), glm::vec2(350.0, 25.0), vec4(1.0), 0x1C47, 5);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "line stipple" << std::endl;
    m_text.print(ss.str(), 10, 390);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(w, h);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
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
  zxd::lines app;
  app.run();
}
