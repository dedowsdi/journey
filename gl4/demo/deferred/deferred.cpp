/*
 * N spheres and N point light. Each sphere has it's own material, each light
 * has it's own light property.
 *
 * Defered rendering is used to implement scenes with tons of lights.
 * It's down in two steps:
 *  1. Render everything you need into gbuffer.
 *  2. Rendre a quad, extract data from gbuffer to do the lighting.
 *
 * In this way, no calculation wasted on failed(such as depth test) fragment.
 *
 * You can mix forward and defered rendering, as long as you record depth in a
 * gbuffer, and apply it later, or simplely use glBlitFramebuffer to copy depth
 * buffer.
 *
 */

#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include "sphere.h"
#include "light.h"

namespace zxd {

struct RenderGbufferProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint attrib_normal;

  virtual void updateModel(const mat4 &_modelMatrix) {
    modelMatrix = _modelMatrix;
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
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/render_gbuffer.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/render_gbuffer.fs.glsl");
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

struct GlinnProgram : public zxd::Program {
  GLint loc_gVertex;
  GLint loc_gNormal;
  GLint loc_gDepth;
  GLint loc_gEmission;
  GLint loc_gAmbient;
  GLint loc_gDiffuse;
  GLint loc_gSpecular;
  GLint loc_gShininess;

  GlinnProgram() {}

  void updateUniforms() {
    glUniform1i(loc_gVertex, 0);
    glUniform1i(loc_gNormal, 1);
    glUniform1i(loc_gEmission, 2);
    glUniform1i(loc_gAmbient, 3);
    glUniform1i(loc_gDiffuse, 4);
    glUniform1i(loc_gSpecular, 5);
    glUniform1i(loc_gShininess, 6);
    glUniform1i(loc_gDepth, 7);
  }

  virtual void reset(GLuint lightCount) {
    if (object != -1) {
      glDeleteProgram(object);
    }

    object = glCreateProgram();
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/use_gbuffer.vs.glsl");
    StringVector sv;
    std::stringstream ss;
    ss << "#version 430 core" << std::endl;
    ss << "#define LIGHT_COUNT " << lightCount << std::endl;
    sv.push_back(ss.str());
    sv.push_back(readFile("data/shader/blinn.frag"));
    attachShaderSourceAndFile(
      GL_FRAGMENT_SHADER, sv, "data/shader/use_gbuffer.fs.glsl");

    this->link();
    bindUniformLocations();
    bindAttribLocations();
  }

  virtual void bindUniformLocations() {
    setUniformLocation(&loc_gVertex, "gVertex");
    setUniformLocation(&loc_gNormal, "gNormal");
    setUniformLocation(&loc_gEmission, "gEmission");
    setUniformLocation(&loc_gAmbient, "gAmbient");
    setUniformLocation(&loc_gDiffuse, "gDiffuse");
    setUniformLocation(&loc_gSpecular, "gSpecular");
    setUniformLocation(&loc_gShininess, "gShininess");
    setUniformLocation(&loc_gDepth, "gDepth");
  }

  virtual void bindAttribLocations(){};
};

class Defered : public App {
protected:
  BitmapText mBitmapText;
  GLint mNumSpheres;
  GLint mNumLights;
  glm::vec2 gBufferSize;

  GLuint mgVertex;
  GLuint mgNormal;

  GLuint mgEmission;
  GLuint mgAmbient;
  GLuint mgDiffuse;
  GLuint mgSpecular;
  GLuint mgShininess;

  GLuint mgDepth;

  // rtt
  GLuint mFbo;

  Sphere mSphere;
  LightModel mLightModel;
  std::vector<LightSource> mLights;
  std::vector<glm::mat4> mSphereModelMatrixes;
  std::vector<Material> mSphereMaterials;

  GlinnProgram mGlinn;
  RenderGbufferProgram mRenderGbufferProgram;

protected:
  void renderGbuffer() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(mRenderGbufferProgram.object);

    GLuint gbuffres[] = {mgVertex, mgNormal, mgEmission, mgAmbient, mgDiffuse,
      mgSpecular, mgShininess, mgDepth};

    for (int i = 0; i < sizeof(gbuffres) / sizeof(GLuint); ++i) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, gbuffres[i]);
    }

    for (int i = 0; i < mNumSpheres; ++i) {
      mRenderGbufferProgram.updateModel(mSphereModelMatrixes[i]);
      mSphereMaterials[i].updateUniforms();
      mSphere.draw();
    }
  }

  void useGbuffer() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(mGlinn.object);

    mGlinn.updateUniforms();

    GLuint gbuffres[] = {mgVertex, mgNormal, mgEmission, mgAmbient, mgDiffuse,
      mgSpecular, mgShininess, mgDepth};

    for (int i = 0; i < sizeof(gbuffres) / sizeof(GLuint); ++i) {
      glActiveTexture(GL_TEXTURE0 + i);
      glBindTexture(GL_TEXTURE_2D, gbuffres[i]);
    }

    // update lights
    for (int i = 0; i < mLights.size(); ++i) {
      mLights[i].updateUniforms(mRenderGbufferProgram.viewMatrix);
    }
    mLightModel.updateUniforms();

    glDrawArrays(GL_TRIANGLES, 0, 6);
  }

public:
  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "defered rendering";
  }
  virtual void createScene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, mInfo.wndWidth, mInfo.wndHeight);

    mNumSpheres = 100;
    mNumLights = 20;
    gBufferSize = vec2(mInfo.wndWidth, mInfo.wndHeight);

    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);

    // gbuffer textures
    glGenTextures(1, &mgVertex);
    glBindTexture(GL_TEXTURE_2D, mgVertex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gBufferSize[0], gBufferSize[1],
      0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mgNormal);
    glBindTexture(GL_TEXTURE_2D, mgNormal);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, gBufferSize[0], gBufferSize[1],
      0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mgDepth);
    glBindTexture(GL_TEXTURE_2D, mgDepth);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, gBufferSize[0],
      gBufferSize[1], 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    // gbuffer textures
    glGenTextures(1, &mgDiffuse);
    glBindTexture(GL_TEXTURE_2D, mgDiffuse);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, gBufferSize[0], gBufferSize[1], 0,
      GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mgAmbient);
    glBindTexture(GL_TEXTURE_2D, mgAmbient);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, gBufferSize[0], gBufferSize[1], 0,
      GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mgSpecular);
    glBindTexture(GL_TEXTURE_2D, mgSpecular);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, gBufferSize[0], gBufferSize[1], 0,
      GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mgEmission);
    glBindTexture(GL_TEXTURE_2D, mgEmission);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, gBufferSize[0], gBufferSize[1], 0,
      GL_RGBA, GL_UNSIGNED_BYTE, 0);

    // gbuffer textures
    glGenTextures(1, &mgShininess);
    glBindTexture(GL_TEXTURE_2D, mgShininess);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, gBufferSize[0], gBufferSize[1], 0,
      GL_RED, GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &mFbo);
    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);

    GLuint gbuffres[] = {mgVertex, mgNormal, mgEmission, mgAmbient, mgDiffuse,
      mgSpecular, mgShininess};

    for (int i = 0; i < sizeof(gbuffres) / sizeof(GLuint); ++i) {
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
        GL_TEXTURE_2D, gbuffres[i], 0);
    }
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, mgDepth, 0);

    // clang-format off
    GLenum drawBuffers[] = {
      GL_COLOR_ATTACHMENT0,
      GL_COLOR_ATTACHMENT1,
      GL_COLOR_ATTACHMENT2,
      GL_COLOR_ATTACHMENT3,
      GL_COLOR_ATTACHMENT4,
      GL_COLOR_ATTACHMENT5,
      GL_COLOR_ATTACHMENT6,
    };
    // clang-format on

    glDrawBuffers(sizeof(drawBuffers) / sizeof(GLenum), drawBuffers);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer");

    mRenderGbufferProgram.init();
    mRenderGbufferProgram.projMatrix =
      glm::perspective(glm::radians(45.0f), getWndAspect(), 10.0f, 50.0f);
    mRenderGbufferProgram.viewMatrix =
      glm::lookAt(vec3(0, -25, 25), vec3(0, 0, 0), vec3(0, 0, 1));

    setViewMatrix(&mRenderGbufferProgram.viewMatrix);

    // sphere and lights
    mSphere.buildVertex();
    mSphere.buildNormal();
    mSphere.bind(
      mRenderGbufferProgram.attrib_vertex, mRenderGbufferProgram.attrib_normal);

    resetSphere();

    resetLight();
    mLightModel.bindUniformLocations(mGlinn, "lightModel");
  }

  void resetSphere() {
    mSphereModelMatrixes.clear();
    mSphereMaterials.clear();

    for (int i = 0; i < mNumSpheres; ++i) {
      mSphereModelMatrixes.push_back(
        glm::translate(glm::linearRand(vec3(-10, -10, -2), vec3(10, 10, 2))));

      Material m;
      m.emission = glm::vec4(0);
      m.ambient = glm::vec4(glm::linearRand(vec3(0.0f), vec3(0.2f)), 1);
      m.diffuse = glm::vec4(glm::linearRand(vec3(0.5f), vec3(1.0f)), 1);
      m.specular = glm::vec4(glm::linearRand(vec3(0.5f), vec3(0.8f)), 1);
      m.shininess = glm::linearRand(1, 120);

      m.bindUniformLocations(mRenderGbufferProgram, "material");

      mSphereMaterials.push_back(m);
    }
  }

  void resetLight() {
    mGlinn.reset(mNumLights);
    mLights.clear();

    for (int i = 0; i < mNumLights; ++i) {
      LightSource light;
      light.position =
        glm::vec4(glm::linearRand(vec3(-10, -10, -2), vec3(10, 10, 2)), 1);
      light.diffuse = glm::vec4(glm::linearRand(vec3(0.75f), vec3(1.0f)), 1);
      light.specular = glm::vec4(glm::linearRand(vec3(0.5f), vec3(1.0f)), 1);
      light.ambient = vec4(0);
      light.constantAttenuation = 1;
      light.linearAttenuation = 0.5;
      light.quadraticAttenuation = 0.25;
      std::stringstream ss;
      ss << "lights[" << i << "]";
      light.bindUniformLocations(mGlinn.object, ss.str());
      mLights.push_back(light);
    }
  }

  virtual void display() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    glBindFramebuffer(GL_FRAMEBUFFER, mFbo);
    renderGbuffer();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    useGbuffer();

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : sphere number " << mNumSpheres << std::endl;
    ss << "wW : light number " << mNumLights << std::endl;
    mBitmapText.print(ss.str(), 10, 492);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfwResize(GLFWwindow *wnd, int w, int h) {
    App::glfwResize(wnd, w, h);
    mBitmapText.reshape(w, h);
  }

  virtual void glfwKey(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    App::glfwKey(wnd, key, scancode, action, mods);
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_Q:
          mNumSpheres += mods == GLFW_MOD_SHIFT ? -10 : 10;
          if (mNumSpheres < 0) {
            mNumSpheres = 0;
          }
          resetSphere();
          break;

        case GLFW_KEY_W:
          mNumLights += mods == GLFW_MOD_SHIFT ? -1 : 1;
          if (mNumLights < 0) {
            mNumLights = 0;
          }
          resetLight();
        default:
          break;
      }
    }
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
  zxd::Defered app;
  app.run();
}
