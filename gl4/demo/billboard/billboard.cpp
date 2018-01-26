/*
 * create a bunch of billboards, all in random position
 */
#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"

GLuint numBillboards = 1000;
GLuint method = 0;  // 0 : billboard_align, 1 : billboard_z, 2 : billboard_eye
std::string methods[] = {"align to camera rotation",
  "rotate z to center_to_camera",
  "look at camera, use camera up"};
GLuint tex;

namespace zxd {

struct Program0 : public zxd::Program {
  GLint attrib_vertex;
  GLint attrib_translation;
  GLint loc_diffuseMap;

  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/billboard_align.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/tex2d.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_viewMatrix, "viewMatrix");
    setUniformLocation(&loc_viewProjMatrix, "viewProjMatrix");
    setUniformLocation(&loc_diffuseMap, "diffuseMap");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_translation = getAttribLocation("translation");
  };
};

struct Program1 : public zxd::Program {
  GLint attrib_vertex;
  GLint attrib_translation;
  GLint loc_diffuseMap;
  GLint loc_cameraPos;

  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/billboard_z.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/tex2d.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_viewProjMatrix, "viewProjMatrix");
    setUniformLocation(&loc_diffuseMap, "diffuseMap");
    setUniformLocation(&loc_cameraPos, "cameraPos");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_translation = getAttribLocation("translation");
  };
};

struct Program2 : public zxd::Program {
  GLint attrib_vertex;
  GLint attrib_translation;
  GLint loc_diffuseMap;
  GLint loc_cameraPos;
  GLint loc_cameraUp;

  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/billboard_eye.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/tex2d.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_viewProjMatrix, "viewProjMatrix");
    setUniformLocation(&loc_diffuseMap, "diffuseMap");
    setUniformLocation(&loc_cameraPos, "cameraPos");
    setUniformLocation(&loc_cameraUp, "cameraUp");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_translation = getAttribLocation("translation");
  };
};

Program0 program0;
Program1 program1;
Program2 program2;

// clang-format off
glm::vec4 vertices[] = { 
  vec4(-0.5, 0.5, 0, 1), // x y s t
  vec4(-0.5, -0.5, 0, 0),
  vec4(0.5, 0.5, 1, 1),
  vec4(0.5, -0.5, 1, 0)
};
// clang-format on

GLuint vao;
Vec3Vector translations;

class BillboardApp : public App {
protected:
  BitmapText mBitmapText;

public:
  BillboardApp() {}

protected:
  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "billboard";
  }
  virtual void createScene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);

    // init program
    program0.init();
    program1.init();
    program2.init();

    program0.viewMatrix =
      glm::lookAt(vec3(0, -100, 100), vec3(0), vec3(0, 0, 1));
    program0.projMatrix = glm::perspective(fpi4, getWndAspect(), 0.1f, 1000.0f);

    setViewMatrix(&program0.viewMatrix);

    // texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto image = createChessImage(64, 64, 8, 8);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 64, 64, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &image.front());
    //glGenerateMipmap(GL_TEXTURE_2D);

    // create vertex buffer
    GLuint vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), value_ptr(vertices[0]),
      GL_STATIC_DRAW);

    glVertexAttribPointer(
      program0.attrib_vertex, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(program0.attrib_vertex);

    glVertexAttribPointer(
      program1.attrib_vertex, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(program1.attrib_vertex);

    // init billboards
    resetBillboards();
  }

  void resetBillboards() {
    translations.clear();
    translations.reserve(numBillboards);
    for (int i = 0; i < numBillboards; ++i) {
      translations.push_back(
        glm::linearRand(glm::vec3(-30.0f), glm::vec3(30.0f)));
    }

    // setup instance attribute
    glBindVertexArray(vao);
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, translations.size() * sizeof(vec3),
      value_ptr(translations[0]), GL_STATIC_DRAW);

    glVertexAttribPointer(
      program0.attrib_translation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(program0.attrib_translation);
    glVertexAttribDivisor(program0.attrib_translation, 1);

    glVertexAttribPointer(
      program1.attrib_translation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(program1.attrib_translation);
    glVertexAttribDivisor(program1.attrib_translation, 1);

    glVertexAttribPointer(
      program2.attrib_translation, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(program2.attrib_translation);
    glVertexAttribDivisor(program2.attrib_translation, 1);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(vao);
    glBindTexture(GL_TEXTURE_2D, tex);

    program0.viewProjMatrix = program0.projMatrix * program0.viewMatrix;
    const glm::mat4 &viewProjMatrix = program0.viewProjMatrix;
    const glm::mat4 &projMatrix = program0.projMatrix;
    const glm::mat4 &viewMatrix = program0.viewMatrix;

    if (method == 0) {
      glUseProgram(program0);

      glUniformMatrix4fv(
        program0.loc_viewProjMatrix, 1, 0, value_ptr(viewProjMatrix));
      glUniformMatrix4fv(program0.loc_viewMatrix, 1, 0, value_ptr(viewMatrix));
      glUniform1i(program0.loc_diffuseMap, 0);

    } else if (method == 1) {
      glUseProgram(program1);

      vec3 cameraPos = glm::inverse(viewMatrix)[3].xyz();

      glUniformMatrix4fv(
        program1.loc_viewProjMatrix, 1, 0, value_ptr(viewProjMatrix));
      glUniform1i(program1.loc_diffuseMap, 0);
      glUniform3fv(program1.loc_cameraPos, 1, value_ptr(cameraPos));

    } else {
      glUseProgram(program2);

      vec3 cameraPos = glm::inverse(viewMatrix)[3].xyz();
      vec3 cameraUp = glm::row(viewMatrix, 1).xyz();

      glUniformMatrix4fv(
        program2.loc_viewProjMatrix, 1, 0, value_ptr(viewProjMatrix));
      glUniform1i(program2.loc_diffuseMap, 0);
      glUniform3fv(program2.loc_cameraPos, 1, value_ptr(cameraPos));
      glUniform3fv(program2.loc_cameraUp, 1, value_ptr(cameraUp));
    }

    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, numBillboards);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : method : " << methods[method] << std::endl;
    ss << "wW : billboard number : " << numBillboards << std::endl;
    ss << "fps : " << mFps << std::endl;
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
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(mWnd, GL_TRUE);
          break;
        case GLFW_KEY_Q:
          method = ++method % 3;
          break;

        case GLFW_KEY_W:
          if (GLFW_MOD_SHIFT & mods) {
            numBillboards -= 100;
            resetBillboards();
          } else {
            numBillboards += 100;
            resetBillboards();
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
  zxd::BillboardApp app;
  app.run();
}
