/*
 * Parallax map.
 *
 * Normalmap tried to use coarse grained geometry instead of fine grained
 * geometry, it reserved normals of fined grained geometry in normal map(tangent
 * space), but it doesn't reserve the surface detail, tangent space surface is
 * always flat, which means the vertex point you look at in a coarsed grained
 * geometry can not be the same vertex point as you llok at it's fine grained
 * cousin. Parallax map is used to overcome this problem, it was applied on
 * trangent space, with an extra depth map(invert of height map).  In tangent
 * space xy component of viewdir is aligned to uv, and the offset is propotion
 * to depth (in a positive random way), so we can approximate the offset based
 * on depth, and apply the offset to texcoord to retrieve normal from normal
 * map.
 *
 */
#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include "light.h"
#include "quad.h"
#include "texutil.h"

namespace zxd {

struct ParallaxProgram : public Program {
  GLint attrib_vertex;
  GLint attrib_normal;
  GLint attrib_tangent;
  GLint attrib_texcoord;
  GLint loc_normalMap;
  GLint loc_diffuseMap;
  GLint loc_depthMap;
  GLint loc_modelCamera;
  GLint loc_heightScale;

  GLuint parallaxMethod{0};

  virtual void updateModel(const mat4 &_modelMatrix) {
    modelMatrix = _modelMatrix;
    modelViewMatrix = viewMatrix * modelMatrix;
    modelViewMatrixInverse = glm::inverse(modelViewMatrix);
    modelMatrixInverse = glm::inverse(modelMatrix);
    modelViewProjMatrix = projMatrix * modelViewMatrix;
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, glm::value_ptr(modelViewProjMatrix));
  };
  virtual void attachShaders() {
    StringVector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 1\n");
    attachShaderSourceAndFile(
      GL_VERTEX_SHADER, sv, "data/shader/parallaxmap.vs.glsl");
    std::stringstream ss;
    ss << "#define PARALLAX_METHOD " << parallaxMethod << std::endl;
    sv.push_back(ss.str());
    sv.push_back(readFile("data/shader/blinn.frag"));
    attachShaderSourceAndFile(
      GL_FRAGMENT_SHADER, sv, "data/shader/parallaxmap.fs.glsl");

    setName("parallaxmap");
  }

  virtual void bindUniformLocations() {
    // setUniformLocation(&loc_eye, "eye");
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_normalMap, "normalMap");
    setUniformLocation(&loc_diffuseMap, "diffuseMap");
    setUniformLocation(&loc_depthMap, "depthMap");
    setUniformLocation(&loc_modelCamera, "modelCamera");
    setUniformLocation(&loc_heightScale, "heightScale");
  }
  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_normal = getAttribLocation("normal");
    attrib_tangent = getAttribLocation("tangent");
    attrib_texcoord = getAttribLocation("texcoord");
  };
} program;

GLuint normalMap, diffuseMap, depthMap;
std::vector<zxd::LightSource> lights;
zxd::LightModel lightModel;
zxd::Material material;
Quad quad;
GLfloat heightScale = 0.05f;
std::string parallaxMethods[] = {"parallaxOcclusionMap", "parallaxSteepMap",
  "parallaxMapWithOffset", "parallaxMapWithoutOffset"};

class NormalMapApp : public App {
protected:
  BitmapText mBitmapText;

public:
  NormalMapApp() {}

  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "hello world";
    mInfo.wndWidth = 512;
    mInfo.wndHeight = 512;
  }
  virtual void createScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    program.parallaxMethod = 3;
    program.init();
    program.projMatrix =
      glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 20.0f);
    program.viewMatrix = glm::lookAt(vec3(0, -5, 5), vec3(0.0f), vec3(0, 1, 0));

    // init quad
    quad.setupVertexAttrib(program.attrib_vertex, program.attrib_texcoord,
      program.attrib_normal, program.attrib_tangent);

    // load maps
    fipImage diffuseImage = zxd::fipLoadImage("data/texture/bricks2.jpg");
    fipImage normalImage = zxd::fipLoadImage("data/texture/bricks2_normal.jpg");
    fipImage depthImage = zxd::fipLoadImage("data/texture/bricks2_disp.jpg");

    glGenTextures(1, &diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuseImage.getWidth(),
      diffuseImage.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      diffuseImage.accessPixels());

    glGenTextures(1, &normalMap);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normalImage.getWidth(),
      normalImage.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      normalImage.accessPixels());

    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, depthImage.getWidth(),
      depthImage.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      depthImage.accessPixels());

    // light
    zxd::LightSource dirLight;
    dirLight.position = vec4(0, -1, 1, 0);
    dirLight.diffuse = vec4(1, 1, 1, 1);
    dirLight.specular = vec4(1, 1, 1, 1);
    dirLight.linearAttenuation = 1.0f;

    lights.push_back(dirLight);

    lightModel.localViewer = 1;

    // material
    material.ambient = vec4(0.2);
    material.diffuse = vec4(0.8);
    material.specular = vec4(0.8);
    material.shininess = 50;

    setViewMatrix(&program.viewMatrix);
    bindUniformLocations(program);

    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw low quality mesh with normal map
    glUseProgram(program);
    program.updateModel(mat4(1.0));
    glUniform1i(program.loc_diffuseMap, 0);
    glUniform1i(program.loc_normalMap, 1);
    glUniform1i(program.loc_depthMap, 2);
    glUniform1f(program.loc_heightScale, heightScale);

    // get camera model position
    vec3 camera = glm::column(program.modelViewMatrixInverse, 3).xyz();
    glUniform3fv(program.loc_modelCamera, 1, glm::value_ptr(camera));

    for (int i = 0; i < lights.size(); ++i) {
      lights[i].updateUniforms(program.modelMatrixInverse);
    }
    lightModel.updateUniforms();
    material.updateUniforms();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    quad.draw();

    glEnable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : parallax method : " << parallaxMethods[program.parallaxMethod]
       << std::endl;
    ss << "w : heightScale : " << heightScale << std::endl;
    mBitmapText.print(ss.str(), 10, mInfo.wndHeight - 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfwResize(GLFWwindow *wnd, int w, int h) {
    App::glfwResize(wnd, w, h);
    mBitmapText.reshape(w, h);
  }

  virtual void bindUniformLocations(zxd::Program &program) {
    lightModel.bindUniformLocations(program.object, "lightModel");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bindUniformLocations(program.object, ss.str());
    }
    material.bindUniformLocations(program.object, "material");
  }

  virtual void glfwKey(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(mWnd, GL_TRUE);
          break;
        case GLFW_KEY_Q: {
          program.parallaxMethod = (program.parallaxMethod + 1) % 4;
          program.clear();
          program.init();
        } break;

          break;
        default:
          break;
      }
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
      switch (key) {
        case GLFW_KEY_W:
          if (mods & GLFW_MOD_SHIFT) {
            heightScale -= 0.002;
            heightScale = glm::max(heightScale, 0.0f);
          } else {
            heightScale += 0.002;
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
};
}

int main(int argc, char *argv[]) {
  zxd::NormalMapApp app;
  app.run();
}
