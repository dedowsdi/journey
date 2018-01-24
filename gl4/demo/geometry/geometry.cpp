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

namespace zxd {

GLfloat wndAspect;
GLuint diffuseMap;

struct BlinnProgram : public zxd::Program {
  // GLint loc_eye;
  GLint attrib_vertex;
  GLint attrib_normal;
  GLint attrib_texcoord;
  GLint loc_diffuseMap;

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
    StringVector sv;
    sv.push_back("#version 430 core\n#define WITH_TEX\n");
    attachShaderSourceAndFile(
      GL_VERTEX_SHADER, sv, "data/shader/blinn.vs.glsl");
    sv.push_back("#define LIGHT_COUNT 3\n");
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
    setUniformLocation(&loc_diffuseMap, "diffuseMap");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_normal = getAttribLocation("normal");
    attrib_texcoord = getAttribLocation("texcoord");
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
  Cone mCone;
  Cylinder mCylinder;
  Torus mTorus;
  BlinnProgram mProgram;

public:
  GeometryApp() : mCameraPos(0, -6, 6) {}

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
    mMaterial.ambient = vec4(1.0);

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
    mSphere.buildMesh();
    mSphere.bind(mProgram.attrib_vertex, mProgram.attrib_normal, mProgram.attrib_texcoord);

    mCuboid.buildMesh();
    mCuboid.bind(mProgram.attrib_vertex, mProgram.attrib_normal, mProgram.attrib_texcoord);

    mCone.buildMesh();
    mCone.bind(mProgram.attrib_vertex, mProgram.attrib_normal, mProgram.attrib_texcoord);

    mCylinder.buildMesh();
    mCylinder.bind(mProgram.attrib_vertex, mProgram.attrib_normal, mProgram.attrib_texcoord);

    mTorus.setRings(32);
    mTorus.setSides(32);
    mTorus.buildMesh();
    mTorus.bind(mProgram.attrib_vertex, mProgram.attrib_normal, mProgram.attrib_texcoord);

    // texture
    GLint imageWidth = 64;
    GLint imageHeight = 64;
    auto image = createChessImage(imageWidth, imageHeight, 8, 8);

    glGenTextures(1, &diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &image.front());
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glUseProgram(mProgram);

    glUniform1i(mProgram.loc_diffuseMap, 0);

    mProgram.viewMatrixInverseTranspose =
      glm::inverse(glm::transpose(mProgram.viewMatrix));

    mMaterial.updateUniforms();
    mLightModel.updateUniforms();
    for (int i = 0; i < mLights.size(); ++i) {
      mLights[i].updateUniforms(mProgram.viewMatrix);
    }

    mat4 model(1.0f);
    mProgram.updateModel(model);
    mSphere.draw();

    model = glm::translate(glm::vec3(2, 0, 0));
    mProgram.updateModel(model);
    mCuboid.draw();

    model = glm::translate(glm::vec3(-2, 0, 0));
    mProgram.updateModel(model);
    mCone.draw();

    model = glm::translate(glm::vec3(0, 2, 0));
    mProgram.updateModel(model);
    mCylinder.draw();

    model = glm::translate(glm::vec3(0, -3, 0));
    mProgram.updateModel(model);
    mTorus.draw();

    GLint cullFace;
    glGetIntegerv(GL_CULL_FACE_MODE, &cullFace);

    GLint polygonMode;
    glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
    std::stringstream ss;
    ss << "q : polygon mode " << glPolygonModeToString(polygonMode)
       << std::endl;
    ss << "w : cullface " << GLint(glIsEnabled(GL_CULL_FACE)) << std::endl;
    ss << "e : depth " << GLint(glIsEnabled(GL_DEPTH_TEST)) << std::endl;
    ss << "r : cullface " << glCullFaceModeToString(cullFace) << std::endl;

    {
      glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      CapabilityGuard<GL_BLEND> blend(GL_TRUE);
      CapabilityGuard<GL_CULL_FACE> cullFace(GL_FALSE);
      mBitmapText.print(ss.str(), 10, 492);
    }
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
          GLint cullFace;
          glGetIntegerv(GL_CULL_FACE_MODE, &cullFace);
          if (cullFace == GL_FRONT) {
            glCullFace(GL_BACK);
          } else if (cullFace == GL_BACK) {
            glCullFace(GL_FRONT_AND_BACK);
          } else {
            glCullFace(GL_FRONT);
          }
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
