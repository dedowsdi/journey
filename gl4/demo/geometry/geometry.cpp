#include "app.h"
#include "bitmap_text.h"
#include "sphere.h"
#include "common.h"
#include "light.h"
#include <sstream>
#include "glEnumString.h"
#include "cuboid.h"

namespace zxd {

GLfloat wndAspect;

struct BlinnProgram : public zxd::Program {
  // GLint loc_eye;
  GLint attrib_vertex;
  GLint attrib_normal;

  virtual void updateModel(const mat4 &_modelMatrix) {
    modelMatrix = _modelMatrix;
    // modelMatrixInverse = glm::inverse(modelMatrix);
    modelViewMatrix = viewMatrix * modelMatrix;
    modelViewMatrixInverseTranspose =
      glm::inverse(glm::transpose(modelViewMatrix));
    modelViewProjMatrix = projMatrix * modelViewMatrix;

    glUniformMatrix4fv(loc_modelViewMatrixInverseTranspose, 1, 0,
      value_ptr(modelViewMatrixInverseTranspose));
    glUniformMatrix4fv(loc_modelViewMatrix, 1, 0, value_ptr(modelViewMatrix));
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
  }
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/blinn.vs.glsl");
    StringVector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 8\n");
    sv.push_back(readFile("data/shader/blinn.frag"));
    attachShaderSourceAndFile(
      GL_FRAGMENT_SHADER, sv, "data/shader/blinn.fs.glsl");
  }
  virtual void bindUniformLocations() {
    // setUniformLocation(&loc_eye, "eye");
    setUniformLocation(&loc_modelViewMatrix, "modelViewMatrix");
    setUniformLocation(
      &loc_modelViewMatrixInverseTranspose, "modelViewMatrixInverseTranspose");
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_normal = getAttribLocation("normal");
  };
};

class GeometryApp : public App {
protected:
  BitmapText mBitmapText;
  vec3 mCameraPos;
  std::vector<zxd::LightSource> mLights;
  zxd::LightModel mLightModel;
  zxd::Material mMaterial;
  Sphere mSphere;
  Cuboid mCuboid;
  BlinnProgram mProgram;

public:
  GeometryApp() : mSphere(1, 16, 16), mCameraPos(0, -6, 6) {}

  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "geometry";
    wndAspect = getWndAspect();
  }

  virtual void createScene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);

    // light
    zxd::LightSource dirLight;
    dirLight.position = vec4(1, -1, 1, 0);
    dirLight.diffuse = vec4(1, 1, 1, 1);
    dirLight.specular = vec4(1, 1, 1, 1);

    mLights.push_back(dirLight);

    // material
    mMaterial.shininess = 50;
    mMaterial.specular = vec4(1.0, 1.0, 1.0, 1.0);

    // text
    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);

    // program
    mProgram.init();
    mProgram.projMatrix =
      glm::perspective(glm::radians(45.0f), wndAspect, 0.1f, 30.0f);
    mProgram.viewMatrix = glm::lookAt(mCameraPos, vec3(0, 0, 0), vec3(0, 0, 1));
    setViewMatrix(&mProgram.viewMatrix);

    mLightModel.bindUniformLocations(mProgram.object, "lightModel");
    for (int i = 0; i < mLights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      mLights[i].bindUniformLocations(mProgram.object, ss.str());
    }
    mMaterial.bindUniformLocations(mProgram.object, "material");

    // geometry
    mSphere.buildVertex(mProgram.attrib_vertex);
    mSphere.buildNormal(mProgram.attrib_normal);

    mCuboid.buildVertex(mProgram.attrib_vertex);
    mCuboid.buildNormal(mProgram.attrib_normal);

  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mProgram);

    mProgram.viewMatrixInverseTranspose =
      glm::inverse(glm::transpose(mProgram.viewMatrix));

    mMaterial.updateUniforms();
    mLightModel.updateUniforms();
    for (int i = 0; i < mLights.size(); ++i) {
      mLights[i].updateUniforms(
        mProgram.viewMatrix, mProgram.viewMatrixInverseTranspose);
    }

    mat4 model(1.0f);
    mProgram.updateModel(model);
    mSphere.draw();

    model = glm::translate(glm::vec3(2, 0, 0));
    mProgram.updateModel(model);
    mCuboid.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    GLint polygonMode;
    glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
    std::stringstream ss;
    ss << "q : polygon mode " << glPolygonModeToString(polygonMode);
    mBitmapText.print(ss.str(), 10, 492);
    glDisable(GL_BLEND);
  }

  virtual void glfwResize(GLFWwindow *wnd, int w, int h) {
    (void)wnd;
    mInfo.wndWidth = w;
    mInfo.wndWidth = h;
  }

  virtual void glfwKey(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(mWnd, GL_TRUE);
          break;
        case GLFW_KEY_Q: {
          GLint polygonMode;
          glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
          glPolygonMode(
            GL_FRONT_AND_BACK, GL_POINT + (polygonMode - GL_POINT + 1) % 3);
        } break;

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
  zxd::GeometryApp app;
  app.run();
}
