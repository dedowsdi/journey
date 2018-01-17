#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include "light.h"

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

namespace zxd {

// clang-format off
struct Pyramid0 {
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

struct Pyramid1 {
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

struct RenderNormalmapProgram : public zxd::Program {
  GLint attrib_texcoord;
  GLint attrib_normal;

  virtual void updateModel(const mat4 &_modelMatrix) {}
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/render_normalmap.vs.glsl");
    attachShaderFile(
      GL_FRAGMENT_SHADER, "data/shader/render_normalmap.fs.glsl");
  }
  virtual void bindUniformLocations() {}

  virtual void bindAttribLocations() {
    attrib_texcoord = getAttribLocation("texcoord");
    attrib_normal = getAttribLocation("normal");
  };
} renderNormalmapProgram;

struct UseNormalMapViewProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint attrib_normal;
  GLint attrib_tangent;
  GLint attrib_texcoord;
  GLint loc_normalMap;

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
    attachShaderFile(
      GL_VERTEX_SHADER, "data/shader/use_normalmap_view.vs.glsl");
    StringVector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 1\n");
    sv.push_back(readFile("data/shader/blinn.frag"));
    attachShaderSourceAndFile(
      GL_FRAGMENT_SHADER, sv, "data/shader/use_normalmap_view.fs.glsl");

    setName("use_normalmap_view");
  }
  virtual void bindUniformLocations() {
    // setUniformLocation(&loc_eye, "eye");
    setUniformLocation(&loc_modelViewMatrix, "modelViewMatrix");
    setUniformLocation(
      &loc_modelViewMatrixInverseTranspose, "modelViewMatrixInverseTranspose");
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_normalMap, "normalMap");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_normal = getAttribLocation("normal");
    attrib_tangent = getAttribLocation("tangent");
    attrib_texcoord = getAttribLocation("texcoord");
  };
} useNormalMapViewProgram;

struct UseNormalMapTangentProgram : public Program {
  GLint attrib_vertex;
  GLint attrib_normal;
  GLint attrib_tangent;
  GLint attrib_texcoord;
  GLint loc_normalMap;
  GLint loc_modelCamera;

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
      GL_VERTEX_SHADER, sv, "data/shader/use_normalmap_tangent.vs.glsl");
    sv.push_back(readFile("data/shader/blinn.frag"));
    attachShaderSourceAndFile(
      GL_FRAGMENT_SHADER, sv, "data/shader/use_normalmap_tangent.fs.glsl");

    setName("use_normalmap_tangent");
  }
  virtual void bindUniformLocations() {
    // setUniformLocation(&loc_eye, "eye");
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_normalMap, "normalMap");
    setUniformLocation(&loc_modelCamera, "modelCamera");
  }
  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_normal = getAttribLocation("normal");
    attrib_tangent = getAttribLocation("tangent");
    attrib_texcoord = getAttribLocation("texcoord");
  };
} useNormalMapTangentProgram;

GLuint normalMap;
std::vector<zxd::LightSource> lights;
zxd::LightModel lightModel;
zxd::Material material;
GLint lightSpace = 0;  // 0 : view, 1 : tangent

class NormalMapApp : public App {
protected:
  BitmapText mBitmapText;

public:
  NormalMapApp() {}

  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "hello world";
    mInfo.wndWidth = IMAGE_WIDTH * 2;
    mInfo.wndHeight = IMAGE_HEIGHT;
  }
  virtual void createScene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    renderNormalmapProgram.init();
    useNormalMapViewProgram.init();
    useNormalMapViewProgram.projMatrix =
      glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 20.0f);
    useNormalMapViewProgram.viewMatrix =
      glm::lookAt(vec3(0, -5, 5), vec3(0.0f), vec3(0, 1, 0));
    useNormalMapTangentProgram.init();
    useNormalMapTangentProgram.projMatrix = useNormalMapViewProgram.projMatrix;
    useNormalMapTangentProgram.viewMatrix = useNormalMapViewProgram.viewMatrix;

    // create mesh data
    zxd::generateFaceNormals(
      pyramid0.vertices, pyramid0.vertices + 12, pyramid0.normals);

    mat4 tbn = zxd::getTangetnBasis(pyramid1.vertices[0], pyramid1.vertices[1],
      pyramid1.vertices[2], pyramid1.texcoords[0], pyramid1.texcoords[1],
      pyramid1.texcoords[2], &pyramid1.normals[0]);
    mat4 invTbn = inverse(tbn);
    vec3 tangent = vec3(glm::column(tbn, 0));
    // vec3 tangent = glm::row(tbn, 0).xyz();

    for (int i = 0; i < 4; ++i) pyramid1.tangents[i] = tangent;

    // transform modle normals to tbn space
    for (int i = 0; i < 12; ++i)
      pyramid0.normals[i] = zxd::transformVector(invTbn, pyramid0.normals[i]);

    // create vertex arries
    glGenVertexArrays(1, &pyramid0.vao);
    glBindVertexArray(pyramid0.vao);

    setupVertexAttribBuiltinArray(
      renderNormalmapProgram.attrib_normal, pyramid0.normals);
    setupVertexAttribBuiltinArray(
      renderNormalmapProgram.attrib_texcoord, pyramid0.texcoords);

    glGenVertexArrays(1, &pyramid1.vao);
    glBindVertexArray(pyramid1.vao);

    setupVertexAttribBuiltinArray(
      useNormalMapViewProgram.attrib_vertex, pyramid1.vertices);
    setupVertexAttribBuiltinArray(
      useNormalMapViewProgram.attrib_normal, pyramid1.normals);
    setupVertexAttribBuiltinArray(
      useNormalMapViewProgram.attrib_tangent, pyramid1.tangents);
    setupVertexAttribBuiltinArray(
      useNormalMapViewProgram.attrib_texcoord, pyramid1.texcoords);

    glGenTextures(1, &normalMap);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB,
      GL_UNSIGNED_BYTE, 0);

    // light
    zxd::LightSource dirLight;
    dirLight.position = vec4(1, -1, 1, 0);
    dirLight.diffuse = vec4(1, 1, 1, 1);
    dirLight.specular = vec4(1, 1, 1, 1);
    dirLight.linearAttenuation = 1.0f;

    lights.push_back(dirLight);

    lightModel.localViewer = 1;

    // material
    material.ambient = vec4(0.2);
    material.diffuse = vec4(0.8);
    material.specular = vec4(1.0);
    material.shininess = 50;

    bindUniformLocations(useNormalMapViewProgram);

    setViewMatrix(&useNormalMapViewProgram.viewMatrix);

    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);
  }

  void generateNormalMap() {
    glViewport(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glScissor(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(pyramid0.vao);
    glUseProgram(renderNormalmapProgram);

    glDrawArrays(GL_TRIANGLES, 0, 12);

    glBindTexture(GL_TEXTURE_2D, normalMap);
    glCopyTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, 0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, 0);
  }
  virtual void display() {
    generateNormalMap();

    glViewport(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glEnable(GL_SCISSOR_TEST);
    glScissor(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw low quality mesh with normal map
    if (lightSpace == 0) {
      glUseProgram(useNormalMapViewProgram);
      useNormalMapViewProgram.updateModel(mat4(1.0));
      glUniform1i(useNormalMapViewProgram.loc_normalMap, 0);

      for (int i = 0; i < lights.size(); ++i) {
        lights[i].updateUniforms(useNormalMapViewProgram.viewMatrix);
      }
    } else {
      glUseProgram(useNormalMapTangentProgram);
      useNormalMapTangentProgram.updateModel(mat4(1.0));
      glUniform1i(useNormalMapTangentProgram.loc_normalMap, 0);

      // get camera model position
      vec3 camera =
        glm::column(useNormalMapTangentProgram.modelViewMatrixInverse, 3).xyz();
      glUniform3fv(
        useNormalMapTangentProgram.loc_modelCamera, 1, glm::value_ptr(camera));

      for (int i = 0; i < lights.size(); ++i) {
        lights[i].updateUniforms(useNormalMapTangentProgram.modelMatrixInverse);
      }
    }
    lightModel.updateUniforms();
    material.updateUniforms();

    glBindVertexArray(pyramid1.vao);

    glDisable(GL_SCISSOR_TEST);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, mInfo.wndWidth, mInfo.wndHeight);
    glEnable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : lighting space : " << (lightSpace == 0 ? "view" : "tangent")
       << std::endl;
    ss << "fps : "<< mFps << std::endl;
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
        case GLFW_KEY_Q:
          lightSpace ^= 1;
          if (lightSpace == 0) {
            useNormalMapViewProgram.viewMatrix = *mViewMatrix;
              
            setViewMatrix(&useNormalMapViewProgram.viewMatrix);
            bindUniformLocations(useNormalMapViewProgram);
          } else {
            useNormalMapTangentProgram.viewMatrix = *mViewMatrix;
            setViewMatrix(&useNormalMapTangentProgram.viewMatrix);
            bindUniformLocations(useNormalMapTangentProgram);
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
