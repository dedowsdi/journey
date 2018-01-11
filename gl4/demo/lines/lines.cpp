#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"

namespace zxd {

class Lines : public App {
  struct LineStippleProgram : public zxd::Program {
    GLint attrib_vertex;

    GLint loc_color;
    GLint loc_viewport;
    GLint loc_pattern;
    GLint loc_factor;

    void updateModel(const mat4 &_modelMatrix, const vec4 &color,
      GLuint pattern, GLint factor) {
      modelMatrix = _modelMatrix;
      modelViewProjMatrix = projMatrix * viewMatrix * modelMatrix;

      glUniformMatrix4fv(
        loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
      glUniform4fv(loc_color, 1, value_ptr(color));
      glUniform1ui(loc_pattern, pattern);
      glUniform1i(loc_factor, factor);
    }
    virtual void attachShaders() {
      attachShaderFile(GL_VERTEX_SHADER, "data/shader/stipple.vs.glsl");
      attachShaderFile(GL_GEOMETRY_SHADER, "data/shader/stipple.gs.glsl");
      attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/stipple.fs.glsl");

      projMatrix = glm::ortho(0.0f, 400.0f, 0.0f, 150.0f);
      viewMatrix = mat4(1.0);
    }
    virtual void bindUniformLocations() {
      setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
      setUniformLocation(&loc_color, "color");
      setUniformLocation(&loc_viewport, "viewport");
      setUniformLocation(&loc_pattern, "pattern");
      setUniformLocation(&loc_factor, "factor");
    }

    virtual void bindAttribLocations() {
      attrib_vertex = getAttribLocation("vertex");
    };
  };

protected:
  BitmapText mBitmapText;
  LineStippleProgram mProgram;
  GLuint mVao;
  GLuint mVertexBuffer;

public:
  Lines() {}

  void drawLines(GLenum mode, const Vec2Vector &vertices, glm::vec4 color,
    GLuint pattern, GLint factor) {
    mProgram.updateModel(mat4(1.0), color, pattern, factor);

    GLuint vao;
    GLuint vbo;

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2),
      value_ptr(vertices[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(
      mProgram.attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(mProgram.attrib_vertex);

    glDrawArrays(mode, 0, vertices.size());
  }

  void drawLine(const glm::vec2 &p0, const glm::vec2 &p1, glm::vec4 color,
    GLuint pattern, GLint factor) {
    Vec2Vector vertices;
    vertices.push_back(p0);
    vertices.push_back(p1);

    drawLines(GL_LINES, vertices, color, pattern, factor);
  }

  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "hello world";
    mInfo.wndWidth = 400;
    mInfo.wndHeight = 150;
  }
  virtual void createScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);

    mProgram.init();

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
    // glGenVertexArrays(1, &mVao);
    // glBindVertexArray(mVao);
    // glGenBuffers(1, &mVertexBuffer);
    // glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
    // GL_STATIC_DRAW);

    // glVertexAttribPointer(
    // mProgram.attrib_vertex, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    // glEnableVertexAttribArray(mProgram.attrib_vertex);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mProgram);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glUniform4iv(mProgram.loc_viewport, 1, viewport);

    // dotted
    drawLine(
      glm::vec2(50.0, 125.0), glm::vec2(150.0, 125.0), vec4(1.0), 0x0101, 1);
    // dashed
    drawLine(
      glm::vec2(150.0, 125.0), glm::vec2(250.0, 125.0), vec4(1.0), 0x00FF, 1);
    // dash/dot/dash
    drawLine(
      glm::vec2(250.0, 125.0), glm::vec2(350.0, 125.0), vec4(1.0), 0x1C47, 1);

    /* in 2nd row, 3 wide lines, each with different stipple */
    glLineWidth(5.0);
    drawLine(
      glm::vec2(50.0, 100.0), glm::vec2(150.0, 100.0), vec4(1.0), 0x0101, 1);
    drawLine(
      glm::vec2(150.0, 100.0), glm::vec2(250.0, 100.0), vec4(1.0), 0x00FF, 1);
    drawLine(
      glm::vec2(250.0, 100.0), glm::vec2(350.0, 100.0), vec4(1.0), 0x1C47, 1);
    glLineWidth(1.0);

    /* in 3rd row, 6 lines, with dash/dot/dash stipple  */
    {
      Vec2Vector vertices;
      for (int i = 0; i < 7; i++)
        vertices.push_back(glm::vec2(50.0 + ((GLfloat)i * 50.0), 75.0));
      drawLines(GL_LINE_STRIP, vertices, glm::vec4(1), 0x1C47, 1);
    }

    /* in 4th row, 6 independent lines with same stipple  */
    for (int i = 0; i < 6; i++) {
      drawLine(glm::vec2(50.0 + ((GLfloat)i * 50.0), 50.0),
        glm::vec2(50.0 + ((GLfloat)(i + 1) * 50.0), 50.0), vec4(1.0), 0x1C47,
        1);
    }

    /* in 5th row, 1 line, with dash/dot/dash stipple    */
    /* and a stipple repeat factor of 5                  */
    drawLine(
      glm::vec2(50.0, 25.0), glm::vec2(350.0, 25.0), vec4(1.0), 0x1C47, 5);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "line stipple" << std::endl;
    mBitmapText.print(ss.str(), 10, 390);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfwResize(GLFWwindow *wnd, int w, int h) {
    App::glfwResize(wnd, w, h);
    mBitmapText.reshape(w, h);
  }

  virtual void glfwKey(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(mWnd, GL_TRUE);
          break;
        default:
          break;
      }
    }
    App::glfwKey(wnd, key, scancode, action, mods);
  }

  virtual void glfwMouseButton(
    GLFWwindow *wnd, int button, int action, int mods) {
    App::glfwMouseButton(wnd, button, action, mods);
  }

  virtual void glfwMouseMove(GLFWwindow *wnd, double x, double y) {
    App::glfwMouseMove(wnd, x, y);
  }

  virtual void glfwMouseWheel(GLFWwindow *wnd, double xoffset, double yoffset) {
    App::glfwMouseWheel(wnd, xoffset, yoffset);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::Lines app;
  app.run();
}
