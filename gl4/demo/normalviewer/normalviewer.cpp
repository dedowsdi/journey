#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include "torus.h"

namespace zxd {

Torus torus;
GLfloat normalLength = 0.1f;
vec4 normalColor(1.0);
vec4 wireColor(0.0, 0.0, 0.0, 1.0);

struct NormalViewerProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint attrib_normal;

  GLint loc_normalLength;
  GLint loc_color;

  virtual void updateModel(const mat4 &_modelMatrix) {
    modelMatrix = _modelMatrix;
    modelViewMatrix = viewMatrix * modelMatrix;
    modelViewMatrixInverseTranspose =
      glm::inverse(glm::transpose(modelViewMatrix));

    glUniformMatrix4fv(loc_modelViewMatrixInverseTranspose, 1, 0,
      value_ptr(modelViewMatrixInverseTranspose));
    glUniformMatrix4fv(loc_modelViewMatrix, 1, 0, value_ptr(modelViewMatrix));
  }
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/normal_viewer.vs.glsl");
    attachShaderFile(GL_GEOMETRY_SHADER, "data/shader/normal_viewer.gs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/color.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_projMatrix, "projMatrix");
    setUniformLocation(&loc_modelViewMatrix, "modelViewMatrix");
    setUniformLocation(
      &loc_modelViewMatrixInverseTranspose, "modelViewMatrixInverseTranspose");

    setUniformLocation(&loc_normalLength, "normalLength");
    setUniformLocation(&loc_color, "color");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_normal = getAttribLocation("normal");
  };
};

struct WireProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint loc_color;

  virtual void updateModel(const mat4 &_modelMatrix) {
    modelMatrix = _modelMatrix;
    modelViewProjMatrix = projMatrix * viewMatrix * modelMatrix;

    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
  }
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/simple.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/color.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_color, "color");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
  };
};

NormalViewerProgram program;

WireProgram wireProgram;

class NormalViewerApp : public App {
protected:
  BitmapText mBitmapText;

public:
  NormalViewerApp() {}

protected:
  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "normal viewer";
  }
  virtual void createScene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    program.init();
    program.projMatrix = glm::perspective(fpi4, getWndAspect(), 0.1f, 50.0f);
    program.viewMatrix = glm::lookAt(vec3(0, 5, 5), vec3(0), vec3(0, 0, 1));

    wireProgram.init();
    wireProgram.projMatrix = program.projMatrix;

    setViewMatrix(&program.viewMatrix);

    torus.buildMesh();
    torus.bind(program.attrib_vertex, program.attrib_normal);

    torus.bind(wireProgram.attrib_vertex);

    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(program);

    mat4 model(1.0);
    program.updateModel(model);
    glUniformMatrix4fv(
      program.loc_projMatrix, 1, 0, value_ptr(program.projMatrix));
    glUniform1f(program.loc_normalLength, normalLength);
    glUniform4fv(program.loc_color, 1, value_ptr(normalColor));
    torus.draw();

    glUseProgram(wireProgram);

    wireProgram.viewMatrix = program.viewMatrix;
    wireProgram.updateModel(model);
    glUniform4fv(wireProgram.loc_color, 1, value_ptr(wireColor));
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    torus.draw();
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : normal length : " << normalLength << std::endl;
    mBitmapText.print(ss.str(), 10, mInfo.wndHeight - 25);
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

        case GLFW_KEY_Q:
          if (mods & GLFW_MOD_SHIFT) {
            normalLength -= 0.1f;
          } else {
            normalLength += 0.1f;
          }
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
  virtual void glfwChar(GLFWwindow *wnd, unsigned int codepoint) {
    App::glfwChar(wnd, codepoint);
  }
  virtual void glfwCharmod(GLFWwindow *wnd, unsigned int codepoint, int mods) {
    App::glfwCharmod(wnd, codepoint, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::NormalViewerApp app;
  app.run();
}
