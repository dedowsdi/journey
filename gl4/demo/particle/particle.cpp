#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common.h"
#include <list>
#include "texutil.h"
#include <algorithm>
#include <iterator>

using namespace glm;

namespace zxd {

struct ParticleProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint attrib_xyzs;
  GLint attrib_color;
  GLint loc_diffuseMap;
  GLint loc_cameraPos;
  GLint loc_cameraUp;
  vec3 cameraPos;
  vec3 cameraUp;

  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/particle.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/particle.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_viewProjMatrix, "viewProjMatrix");
    setUniformLocation(&loc_diffuseMap, "diffuseMap");
    setUniformLocation(&loc_cameraPos, "cameraPos");
    setUniformLocation(&loc_cameraUp, "cameraUp");
  }

  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
    attrib_xyzs = getAttribLocation("xyzs");
    attrib_color = getAttribLocation("color");
  };
};
GLuint maxParticles = 10000;
GLuint emitsPerSecond = 2000;

GLfloat minSpeed = 0.5;
GLfloat maxSpeed = 2.0;
GLfloat minLife = 0.5;
GLfloat maxLife = 3;
GLfloat minAcceleration = 0.1;
GLfloat maxAcceleration = 2.0;
GLfloat minSize = 0.5;
GLfloat maxSize = 3;

GLuint vao;
GLuint xyzsBuffer;
GLuint colorBuffer;

struct Particle {
  vec3 pos;
  vec3 velocity;
  vec3 accleleration;
  vec4 color;
  GLfloat size;
  GLfloat life;
  float cameraDistance;  // squared distance, used to sort particles
};

// clang-format off
glm::vec4 vertices[] = { 
  vec4(-0.5, 0.5, 0, 1), // x y s t
  vec4(-0.5, -0.5, 0, 0),
  vec4(0.5, 0.5, 1, 1),
  vec4(0.5, -0.5, 1, 0)
};
// clang-format on

typedef std::vector<Particle> ParticleVector;
typedef std::list<Particle *> ParticleList;

Vec4Vector positions;  // x,y,z,size
Vec4Vector colors;
ParticleVector particles;
ParticleList aliveParticles;
ParticleList deadParticles;

GLuint tex;

ParticleProgram prg;

class ParticleApp : public App {
protected:
  BitmapText mBitmapText;

public:
  ParticleApp() {}

protected:
  virtual void initInfo() {
    App::initInfo();
    mInfo.title = "hello world";
  }
  virtual void createScene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    mBitmapText.init();
    mBitmapText.reshape(mInfo.wndWidth, mInfo.wndHeight);

    // load particle texture
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    auto image = zxd::fipLoadImage("data/texture/flare.png");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(),
      0, GL_BGR, GL_UNSIGNED_BYTE, image.accessPixels());
    // init particles
    particles.reserve(maxParticles);
    for (int i = 0; i < maxParticles; ++i) {
      particles.push_back(Particle());
    }
    for (int i = 0; i < maxParticles; ++i) {
      deadParticles.push_back(&particles[i]);
    }

    // always use max particle sized buffer?
    positions.resize(maxParticles);
    colors.resize(maxParticles);

    // init program
    prg.init();
    prg.viewMatrix = glm::lookAt(vec3(0, -15, 15), vec3(0), vec3(0, 0, 1));
    prg.projMatrix = glm::perspective(fpi4, getWndAspect(), 0.1f, 1000.0f);
    setViewMatrix(&prg.viewMatrix);

    // create buffer
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), value_ptr(vertices[0]),
      GL_STATIC_DRAW);

    glVertexAttribPointer(
      prg.attrib_vertex, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(prg.attrib_vertex);

    glGenBuffers(1, &xyzsBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, xyzsBuffer);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(vec4) * maxParticles, 0, GL_STREAM_DRAW);

    glVertexAttribPointer(
      prg.attrib_xyzs, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(prg.attrib_xyzs);
    glVertexAttribDivisor(prg.attrib_xyzs, 1);

    glGenBuffers(1, &colorBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(vec4) * maxParticles, 0, GL_STREAM_DRAW);

    glVertexAttribPointer(
      prg.attrib_color, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(prg.attrib_color);
    glVertexAttribDivisor(prg.attrib_color, 1);

    update();
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prg.viewProjMatrix = prg.projMatrix * prg.viewMatrix;
    const glm::mat4 &viewProjMatrix = prg.viewProjMatrix;
    // const glm::mat4 &viewMatrix = prg.viewMatrix;

    glUseProgram(prg);

    glBindTexture(GL_TEXTURE_2D, tex);
    glUniformMatrix4fv(prg.loc_viewProjMatrix, 1, 0, value_ptr(viewProjMatrix));
    glUniform1i(prg.loc_diffuseMap, 0);
    glUniform3fv(prg.loc_cameraPos, 1, value_ptr(prg.cameraPos));
    glUniform3fv(prg.loc_cameraUp, 1, value_ptr(prg.cameraUp));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

    glBindVertexArray(vao);
    glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, aliveParticles.size());

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : emits per second : " << emitsPerSecond << std::endl;
    ss << "current particles : " << aliveParticles.size() << std::endl;
    ss << "fps : " << mFps << std::endl;
    mBitmapText.print(ss.str(), 10, mInfo.wndHeight - 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {
    static GLfloat t0 = glfwGetTime();
    static GLfloat t = 0;  // emitter timer

    GLfloat t1 = glfwGetTime();
    GLfloat dt = t1 - t0;
    GLfloat emitPeriod = 1.0f / emitsPerSecond;
    t += dt;

    prg.cameraPos = glm::inverse(prg.viewMatrix)[3].xyz();
    prg.cameraUp = glm::row(prg.viewMatrix, 1).xyz();

    GLint numNewParticles = t / emitPeriod;
    t -= numNewParticles * emitPeriod;

    numNewParticles = glm::min(static_cast<GLuint>(numNewParticles),
      static_cast<GLuint>(emitsPerSecond * 0.16));

    // generate new particles
    for (int i = 0; i < numNewParticles; ++i) {
      if (deadParticles.empty()) break;

      Particle &p = *deadParticles.front();
      p.life = glm::linearRand(minLife, maxLife);
      p.color = glm::linearRand(vec4(0.0), vec4(1.0));
      p.pos = vec3(0);
      vec3 dir(0);
      while (dir == vec3(0)) dir = glm::linearRand(vec3(-1.0), vec3(1.0));
      dir = glm::normalize(dir);
      p.velocity = dir * glm::linearRand(minSpeed, maxSpeed);
      p.accleleration = dir * glm::linearRand(minAcceleration, maxAcceleration);
      p.size = glm::linearRand(minSize, maxSize);

      aliveParticles.push_back(&p);
      deadParticles.pop_front();
    }

    // animating particles
    GLuint index = 0;
    for (auto iter = aliveParticles.begin(); iter != aliveParticles.end();) {
      Particle &p = **iter;
      p.life -= dt;
      if (p.life < 0) {
        auto it = iter;
        ++it;
        aliveParticles.erase(iter);
        deadParticles.push_back(&p);
        iter = it;
        continue;
      }

      p.pos += p.velocity * dt + p.accleleration * 0.5f * dt * dt;
      p.velocity += p.accleleration * dt;
      p.cameraDistance = length2(p.pos - prg.cameraPos);
      ++iter;
    }

    // particles are translucent, they need to be sorted by camera distance
    aliveParticles.sort([&](Particle *p0, Particle *p1) -> bool {
      return p0->cameraDistance < p1->cameraDistance;
    });

    index = 0;
    for (auto iter = aliveParticles.begin(); iter != aliveParticles.end();
         ++iter) {
      Particle &p = **iter;
      positions[index] = vec4(p.pos, p.size);
      colors[index] = p.color;
      ++index;
    }

    // update buffer
    glBindBuffer(GL_ARRAY_BUFFER, xyzsBuffer);
    //glBufferData(GL_ARRAY_BUFFER, aliveParticles.size() * sizeof(vec4), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, aliveParticles.size() * sizeof(vec4),
      value_ptr(positions.front()));

    glBindBuffer(GL_ARRAY_BUFFER, colorBuffer);
    //glBufferData(GL_ARRAY_BUFFER, aliveParticles.size() * sizeof(vec4), 0, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, aliveParticles.size() * sizeof(vec4),
      value_ptr(colors.front()));

    t0 = t1;
  }

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
          if (mods & GLFW_KEY_Q) {
            if (emitsPerSecond > 5) {
              emitsPerSecond -= 5;
            }
          } else {
            emitsPerSecond += 5;
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
  zxd::ParticleApp app;
  app.run();
}
