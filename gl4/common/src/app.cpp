#include "app.h"
#include <stdexcept>
#include <iostream>
#include "glEnumString.h"

namespace zxd {

//--------------------------------------------------------------------
void glfwErrorCallback(int error, const char *description) {
  std::cout << description << std::endl;
}

//--------------------------------------------------------------------
void glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar *message, const void *userParam) {
  const char *debugSource = glDebugSourceToString(source);
  const char *debugType = glDebugTypeToString(type);
  const char *debugSeverity = glDebugSeverityToString(severity);

  printf("%s : %s : %s : %d : %.*s\n", debugSeverity, debugSource, debugType,
    id, length, message);
}

//--------------------------------------------------------------------
void App::init() {
  initInfo();
  initWnd();
  initGL();
}

//--------------------------------------------------------------------
void App::initInfo() {
  mInfo.title = "app";
  mInfo.wndWidth = 512;
  mInfo.wndHeight = 512;
  mInfo.majorVersion = 4;
  mInfo.minorVersion = 3;
  mInfo.samples = 1;
  mInfo.fullscreen = GL_FALSE;
  mInfo.vsync = GL_FALSE;
  mInfo.cursor = GL_TRUE;
  mInfo.stereo = GL_FALSE;
  mInfo.debug = GL_TRUE;
}

//--------------------------------------------------------------------
void App::initGL() {
  if (!gladLoadGL()) {
    std::cerr << "glad failed to load gl" << std::endl;
    return;
  }
  if (GLVersion.major < 4) {
    std::cerr << "Your system doesn't support OpenGL >= 4!" << std::endl;
  }
  printf(
    "GL_VERSION : %s\n"
    "GL_SHADING_LANGUAGE_VERSION : %s\n"
    "GL_RENDERER : %s\n"
    "GL_VENDOR : %s\n"
    "GL_SHADING_LANGUAGE_VERSION : %s\n",
    glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION),
    glGetString(GL_RENDERER), glGetString(GL_VENDOR),
    glGetString(GL_SHADING_LANGUAGE_VERSION));

  // init debugger
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(glDebugOutput, this);
  debugMessageControl();
}

//--------------------------------------------------------------------
void App::debugMessageControl() {
  GLuint ids[] = {131185};

  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE,
    sizeof(ids) / sizeof(GLuint), ids, GL_FALSE);
}

//--------------------------------------------------------------------
void App::initWnd() {
  glfwSetErrorCallback(glfwErrorCallback);
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, mInfo.majorVersion);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, mInfo.minorVersion);

#ifdef _DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, mInfo.samples);
  glfwWindowHint(GLFW_STEREO, mInfo.stereo);
  glfwWindowHint(GLFW_DEPTH_BITS, mInfo.stereo);

  if (mInfo.fullscreen) {
    if (mInfo.wndWidth == 0 || mInfo.wndHeight == 0) {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      mInfo.wndWidth = mode->width;
      mInfo.wndHeight = mode->height;
    }
    mWnd = glfwCreateWindow(mInfo.wndWidth, mInfo.wndHeight,
      mInfo.title.c_str(), glfwGetPrimaryMonitor(), NULL);
    // glfwSwapInterval((int)mInfo.flags.vsync);
  } else {
    mWnd = glfwCreateWindow(
      mInfo.wndWidth, mInfo.wndHeight, mInfo.title.c_str(), NULL, NULL);
    if (!mWnd) {
      std::cerr << "Failed to open window" << std::endl;
      return;
    }
  }

  glfwMakeContextCurrent(mWnd);

  // set up call back
  glfwSetWindowUserPointer(mWnd, this);

  auto resizeCallback = [](GLFWwindow *wnd, int w, int h) {
    auto pthis = static_cast<App *>(glfwGetWindowUserPointer(wnd));
    pthis->glfwResize(wnd, w, h);
  };
  auto keyCallback = [](
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    auto pthis = static_cast<App *>(glfwGetWindowUserPointer(wnd));
    pthis->glfwKey(wnd, key, scancode, action, mods);
  };
  auto mouseButtonCallback = [](
    GLFWwindow *wnd, int button, int action, int mods) {
    auto pthis = static_cast<App *>(glfwGetWindowUserPointer(wnd));
    pthis->glfwMouseButton(wnd, button, action, mods);
  };
  auto cursorCallback = [](GLFWwindow *wnd, double x, double y) {
    auto pthis = static_cast<App *>(glfwGetWindowUserPointer(wnd));
    pthis->glfwMouseMove(wnd, x, y);
  };
  auto scrollCallback = [](GLFWwindow *wnd, double xoffset, double yoffset) {
    auto pthis = static_cast<App *>(glfwGetWindowUserPointer(wnd));
    pthis->glfwMouseWheel(wnd, xoffset, yoffset);
  };
  auto charCallback = [](GLFWwindow *wnd, unsigned int codepoint) {
    auto pthis = static_cast<App *>(glfwGetWindowUserPointer(wnd));
    pthis->glfwChar(wnd, codepoint);
  };
  auto charmodeCallback = [](GLFWwindow *wnd, unsigned int codepoint, int mod) {
    auto pthis = static_cast<App *>(glfwGetWindowUserPointer(wnd));
    pthis->glfwCharmod(wnd, codepoint, mod);
  };

  glfwSetWindowSizeCallback(mWnd, resizeCallback);
  glfwSetKeyCallback(mWnd, keyCallback);
  glfwSetMouseButtonCallback(mWnd, mouseButtonCallback);
  glfwSetCursorPosCallback(mWnd, cursorCallback);
  glfwSetScrollCallback(mWnd, scrollCallback);
  glfwSetCharCallback(mWnd, charCallback);
  glfwSetCharModsCallback(mWnd, charmodeCallback);
}

//--------------------------------------------------------------------
void App::updateFps()
{
  static GLfloat lastTime = 0;
  static GLfloat time = 0;
  static GLfloat count = 0;

  GLuint curTime = glfwGetTime();

  time += curTime - lastTime;
  ++count;

  if (time >= 1) {
    mFps = count;
    time -= 1;
    count = 0;
  }

  lastTime = curTime;;
}

//--------------------------------------------------------------------
void App::run() {
  srand(time(0));
  init();
  createScene();
  loop();
}

//--------------------------------------------------------------------
void App::startReading() {
  mReading = GL_TRUE;
  mInput.clear();
}

//--------------------------------------------------------------------
void App::stopReading() { mReading = GL_FALSE; }

//--------------------------------------------------------------------
void App::finisheReading() { mReading = GL_FALSE; }

//--------------------------------------------------------------------
void App::loop() {
  glfwSwapInterval(mSwapInterval);
  while (!glfwWindowShouldClose(mWnd)) {
    updateFps();
    update();
    display();
    glfwSwapBuffers(mWnd);
    glfwPollEvents();
  }
  shutdown();
  glfwDestroyWindow(mWnd);
  glfwTerminate();
}

//--------------------------------------------------------------------
void App::glfwResize(GLFWwindow *wnd, int w, int h) {
  (void)wnd;
  mInfo.wndWidth = w;
  mInfo.wndWidth = h;
  glViewport(0, 0, w, h);
}

//--------------------------------------------------------------------
void App::glfwKey(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  (void)wnd;
  (void)scancode;
  (void)mods;

  if (mReading) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_BACKSPACE:
          mInput.pop_back();
          break;
        case GLFW_KEY_ESCAPE:
          stopReading();
        default:
          break;
      }
    }
    return;
  }

  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(mWnd, GL_TRUE);
        break;
      case GLFW_KEY_KP_SUBTRACT:
        mCameraMode = static_cast<CameraMode>((mCameraMode + 1) % 2);
        break;
      case GLFW_KEY_KP_0: {
        GLint polygonMode;
        glGetIntegerv(GL_POLYGON_MODE, &polygonMode);
        glPolygonMode(
          GL_FRONT_AND_BACK, GL_POINT + (polygonMode - GL_POINT + 1) % 3);
      } break;
      case GLFW_KEY_KP_2: {
        glfwSwapInterval(mSwapInterval ^= 1);
      } break;

      case GLFW_KEY_KP_1: {
        if (mViewMatrix) {
          GLfloat distance = glm::length((*mViewMatrix)[3].xyz());
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *mViewMatrix = glm::lookAt(glm::vec3(0.0f, -distance * factor, 0.0f),
            glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
      } break;
      case GLFW_KEY_KP_3: {
        if (mViewMatrix) {
          GLfloat distance = glm::length((*mViewMatrix)[3].xyz());
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *mViewMatrix = glm::lookAt(glm::vec3(-distance * factor, 0.0f, 0.0f),
            glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
      } break;

      case GLFW_KEY_KP_7: {
        if (mViewMatrix) {
          GLfloat distance = glm::length((*mViewMatrix)[3].xyz());
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *mViewMatrix = glm::lookAt(glm::vec3(0.0f, 0.0f, -distance * factor),
            glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
      } break;

      default:
        break;
    }
  }
}

//--------------------------------------------------------------------
void App::glfwMouseButton(GLFWwindow *wnd, int button, int action, int mods) {
  if (action == GLFW_PRESS && GLFW_MOUSE_BUTTON_MIDDLE == button) {
    glfwGetCursorPos(mWnd, &mLastButtonPosition[0], &mLastButtonPosition[1]);
  }
}

//--------------------------------------------------------------------
void App::glfwMouseMove(GLFWwindow *wnd, double x, double y) {
  if (mViewMatrix &&
      glfwGetMouseButton(mWnd, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    GLdouble dtX = x - mLastButtonPosition[0];
    GLdouble dtY = y - mLastButtonPosition[1];

    if (mCameraMode == CM_BLEND) {
      // yaw world, assume z up
      if (dtX != 0) {
        *mViewMatrix *=
          glm::rotate(static_cast<GLfloat>(dtX * 0.02), vec3(0, 0, 1));
      }

      // pitch camera, but reserve center
      if (dtY != 0) {
        glm::vec3 translation = glm::column(*mViewMatrix, 3).xyz();

        // translate world to camera
        (*mViewMatrix)[3][0] = 0;
        (*mViewMatrix)[3][1] = 0;
        (*mViewMatrix)[3][2] = 0;

        // rotate, translate world back
        *mViewMatrix =
          glm::translate(translation) *
          glm::rotate(static_cast<GLfloat>(dtY * 0.02), vec3(1, 0, 0)) *
          *mViewMatrix;
      }
    } else if (mCameraMode == CM_ARCBALL) {
      if (dtX != 0 || dtY != 0) {
        mat4 windowMatrixInverse = zxd::computeWindowMatrixInverse(
          0, 0, mInfo.wndWidth, mInfo.wndHeight, 0, 1);
        mat4 m = zxd::arcball(glm::vec2(mLastButtonPosition[0],
                                mInfo.wndHeight - 1 - mLastButtonPosition[1]),
          glm::vec2(x, mInfo.wndHeight - 1 - y), windowMatrixInverse);

        glm::vec3 translation = glm::column(*mViewMatrix, 3).xyz();
        // translate world to camera
        (*mViewMatrix)[3][0] = 0;
        (*mViewMatrix)[3][1] = 0;
        (*mViewMatrix)[3][2] = 0;

        // rotate, translate world back
        *mViewMatrix = glm::translate(translation) * m * *mViewMatrix;
      }
    }

    mLastButtonPosition[0] = x;
    mLastButtonPosition[1] = y;
  }
}

//--------------------------------------------------------------------
void App::glfwMouseWheel(GLFWwindow *wnd, double xoffset, double yoffset) {
  // yoffset is negative if you scroll toward yourself
  if (mViewMatrix) {
    GLfloat scale = 1 - 0.1 * yoffset;
    (*mViewMatrix)[3][0] *= scale;
    (*mViewMatrix)[3][1] *= scale;
    (*mViewMatrix)[3][2] *= scale;
  }
}

//--------------------------------------------------------------------
void App::glfwChar(GLFWwindow *wnd, unsigned int codepoint) {
  if (mReading) {
    mInput.push_back(char(codepoint));
  }
}

//--------------------------------------------------------------------
void App::glfwCharmod(GLFWwindow *wnd, unsigned int codepoint, int mods) {}
}
