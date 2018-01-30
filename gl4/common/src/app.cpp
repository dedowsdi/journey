#include "app.h"
#include <stdexcept>
#include <iostream>
#include "glEnumString.h"

namespace zxd {

//--------------------------------------------------------------------
void glfw_error_callback(int error, const char *description) {
  std::cout << description << std::endl;
}

//--------------------------------------------------------------------
void gl_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar *message, const void *user_param) {
  const char *debug_source = gl_debug_source_to_string(source);
  const char *debug_type = gl_debug_type_to_string(type);
  const char *debug_severity = gl_debug_severity_to_string(severity);

  printf("%s : %s : %s : %d : %.*s\n", debug_severity, debug_source, debug_type,
    id, length, message);
}

//--------------------------------------------------------------------
void app::init() {
  init_info();
  init_wnd();
  init_g_l();
}

//--------------------------------------------------------------------
void app::init_info() {
  m_info.title = "app";
  m_info.wnd_width = 512;
  m_info.wnd_height = 512;
  m_info.major_version = 4;
  m_info.minor_version = 3;
  m_info.samples = 1;
  m_info.fullscreen = GL_FALSE;
  m_info.vsync = GL_FALSE;
  m_info.cursor = GL_TRUE;
  m_info.stereo = GL_FALSE;
  m_info.debug = GL_TRUE;
}

//--------------------------------------------------------------------
void app::init_g_l() {
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
  glDebugMessageCallback(gl_debug_output, this);
  debug_message_control();
}

//--------------------------------------------------------------------
void app::debug_message_control() {
  GLuint ids[] = {131185};

  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE,
    sizeof(ids) / sizeof(GLuint), ids, GL_FALSE);
}

//--------------------------------------------------------------------
void app::init_wnd() {
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit()) {
    std::cerr << "Failed to initialize GLFW" << std::endl;
    return;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, m_info.major_version);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, m_info.minor_version);

#ifdef _DEBUG
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_SAMPLES, m_info.samples);
  glfwWindowHint(GLFW_STEREO, m_info.stereo);
  glfwWindowHint(GLFW_DEPTH_BITS, m_info.stereo);

  if (m_info.fullscreen) {
    if (m_info.wnd_width == 0 || m_info.wnd_height == 0) {
      const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
      m_info.wnd_width = mode->width;
      m_info.wnd_height = mode->height;
    }
    m_wnd = glfwCreateWindow(m_info.wnd_width, m_info.wnd_height,
      m_info.title.c_str(), glfwGetPrimaryMonitor(), NULL);
    // glfwSwapInterval((int)m_info.flags.vsync);
  } else {
    m_wnd = glfwCreateWindow(
      m_info.wnd_width, m_info.wnd_height, m_info.title.c_str(), NULL, NULL);
    if (!m_wnd) {
      std::cerr << "Failed to open window" << std::endl;
      return;
    }
  }

  glfwMakeContextCurrent(m_wnd);

  // set up call back
  glfwSetWindowUserPointer(m_wnd, this);

  auto resizeCallback = [](GLFWwindow *wnd, int w, int h) {
    auto pthis = static_cast<app *>(glfwGetWindowUserPointer(wnd));
    pthis->glfw_resize(wnd, w, h);
  };
  auto keyCallback = [](
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    auto pthis = static_cast<app *>(glfwGetWindowUserPointer(wnd));
    pthis->glfw_key(wnd, key, scancode, action, mods);
  };
  auto mouseButtonCallback = [](
    GLFWwindow *wnd, int button, int action, int mods) {
    auto pthis = static_cast<app *>(glfwGetWindowUserPointer(wnd));
    pthis->glfw_mouse_button(wnd, button, action, mods);
  };
  auto cursorCallback = [](GLFWwindow *wnd, double x, double y) {
    auto pthis = static_cast<app *>(glfwGetWindowUserPointer(wnd));
    pthis->glfw_mouse_move(wnd, x, y);
  };
  auto scrollCallback = [](GLFWwindow *wnd, double xoffset, double yoffset) {
    auto pthis = static_cast<app *>(glfwGetWindowUserPointer(wnd));
    pthis->glfw_mouse_wheel(wnd, xoffset, yoffset);
  };
  auto charCallback = [](GLFWwindow *wnd, unsigned int codepoint) {
    auto pthis = static_cast<app *>(glfwGetWindowUserPointer(wnd));
    pthis->glfw_char(wnd, codepoint);
  };
  auto charmodeCallback = [](GLFWwindow *wnd, unsigned int codepoint, int mod) {
    auto pthis = static_cast<app *>(glfwGetWindowUserPointer(wnd));
    pthis->glfw_charmod(wnd, codepoint, mod);
  };

  glfwSetWindowSizeCallback(m_wnd, resizeCallback);
  glfwSetKeyCallback(m_wnd, keyCallback);
  glfwSetMouseButtonCallback(m_wnd, mouseButtonCallback);
  glfwSetCursorPosCallback(m_wnd, cursorCallback);
  glfwSetScrollCallback(m_wnd, scrollCallback);
  glfwSetCharCallback(m_wnd, charCallback);
  glfwSetCharModsCallback(m_wnd, charmodeCallback);
}

//--------------------------------------------------------------------
void app::update_fps() {
  static GLfloat last_time = 0;
  static GLfloat time = 0;
  static GLfloat count = 0;

  GLuint cur_time = glfwGetTime();

  time += cur_time - last_time;
  ++count;

  if (time >= 1) {
    m_fps = count;
    time -= 1;
    count = 0;
  }

  last_time = cur_time;
  ;
}

//--------------------------------------------------------------------
void app::run() {
  srand(time(0));
  init();
  create_scene();
  loop();
}

//--------------------------------------------------------------------
void app::start_reading() {
  m_reading = GL_TRUE;
  m_input.clear();
}

//--------------------------------------------------------------------
void app::stop_reading() { m_reading = GL_FALSE; }

//--------------------------------------------------------------------
void app::finishe_reading() { m_reading = GL_FALSE; }

//--------------------------------------------------------------------
void app::loop() {
  glfwSwapInterval(m_swap_interval);
  while (!glfwWindowShouldClose(m_wnd)) {
    update_fps();
    update();
    display();
    glfwSwapBuffers(m_wnd);
    glfwPollEvents();
    ++m_frame_number;
  }
  shutdown();
  glfwDestroyWindow(m_wnd);
  glfwTerminate();
}

//--------------------------------------------------------------------
void app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  (void)wnd;
  m_info.wnd_width = w;
  m_info.wnd_width = h;
  glViewport(0, 0, w, h);
}

//--------------------------------------------------------------------
void app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  (void)wnd;
  (void)scancode;
  (void)mods;

  if (m_reading) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_BACKSPACE:
          m_input.pop_back();
          break;
        case GLFW_KEY_ESCAPE:
          stop_reading();
        default:
          break;
      }
    }
    return;
  }

  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;
      case GLFW_KEY_KP_SUBTRACT:
        m_camera_mode = static_cast<camera_mode>((m_camera_mode + 1) % 2);
        break;
      case GLFW_KEY_KP_0: {
        GLint polygon_mode;
        glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
        glPolygonMode(
          GL_FRONT_AND_BACK, GL_POINT + (polygon_mode - GL_POINT + 1) % 3);
      } break;
      case GLFW_KEY_KP_2: {
        glfwSwapInterval(m_swap_interval ^= 1);
      } break;

      case GLFW_KEY_KP_1: {
        if (m_v_mat) {
          GLfloat distance = glm::length((*m_v_mat)[3].xyz());
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *m_v_mat = glm::lookAt(glm::vec3(0.0f, -distance * factor, 0.0f),
            glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
      } break;
      case GLFW_KEY_KP_3: {
        if (m_v_mat) {
          GLfloat distance = glm::length((*m_v_mat)[3].xyz());
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *m_v_mat = glm::lookAt(glm::vec3(-distance * factor, 0.0f, 0.0f),
            glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
      } break;

      case GLFW_KEY_KP_7: {
        if (m_v_mat) {
          GLfloat distance = glm::length((*m_v_mat)[3].xyz());
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *m_v_mat = glm::lookAt(glm::vec3(0.0f, 0.0f, -distance * factor),
            glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
      } break;

      default:
        break;
    }
  }
}

//--------------------------------------------------------------------
void app::glfw_mouse_button(GLFWwindow *wnd, int button, int action, int mods) {
  if (action == GLFW_PRESS && GLFW_MOUSE_BUTTON_MIDDLE == button) {
    glfwGetCursorPos(
      m_wnd, &m_last_button_position[0], &m_last_button_position[1]);
  }
}

//--------------------------------------------------------------------
void app::glfw_mouse_move(GLFWwindow *wnd, double x, double y) {
  if (m_v_mat &&
      glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
    GLdouble dtx = x - m_last_button_position[0];
    GLdouble dty = y - m_last_button_position[1];

    if (m_camera_mode == CM_BLEND) {
      // yaw world, assume z up
      if (dtx != 0) {
        *m_v_mat *=
          glm::rotate(static_cast<GLfloat>(dtx * 0.02), vec3(0, 0, 1));
      }

      // pitch camera, but reserve center
      if (dty != 0) {
        glm::vec3 translation = glm::column(*m_v_mat, 3).xyz();

        // translate world to camera
        (*m_v_mat)[3][0] = 0;
        (*m_v_mat)[3][1] = 0;
        (*m_v_mat)[3][2] = 0;

        // rotate, translate world back
        *m_v_mat =
          glm::translate(translation) *
          glm::rotate(static_cast<GLfloat>(dty * 0.02), vec3(1, 0, 0)) *
          *m_v_mat;
      }
    } else if (m_camera_mode == CM_ARCBALL) {
      if (dtx != 0 || dty != 0) {
        mat4 w_mat_i = zxd::compute_window_matrix_inverse(
          0, 0, m_info.wnd_width, m_info.wnd_height, 0, 1);
        mat4 m =
          zxd::arcball(glm::vec2(m_last_button_position[0],
                         m_info.wnd_height - 1 - m_last_button_position[1]),
            glm::vec2(x, m_info.wnd_height - 1 - y), w_mat_i);

        glm::vec3 translation = glm::column(*m_v_mat, 3).xyz();
        // translate world to camera
        (*m_v_mat)[3][0] = 0;
        (*m_v_mat)[3][1] = 0;
        (*m_v_mat)[3][2] = 0;

        // rotate, translate world back
        *m_v_mat = glm::translate(translation) * m * *m_v_mat;
      }
    }

    m_last_button_position[0] = x;
    m_last_button_position[1] = y;
  }
}

//--------------------------------------------------------------------
void app::glfw_mouse_wheel(GLFWwindow *wnd, double xoffset, double yoffset) {
  // yoffset is negative if you scroll toward yourself
  if (m_v_mat) {
    GLfloat scale = 1 - 0.1 * yoffset;
    (*m_v_mat)[3][0] *= scale;
    (*m_v_mat)[3][1] *= scale;
    (*m_v_mat)[3][2] *= scale;
  }
}

//--------------------------------------------------------------------
void app::glfw_char(GLFWwindow *wnd, unsigned int codepoint) {
  if (m_reading) {
    m_input.push_back(char(codepoint));
  }
}

//--------------------------------------------------------------------
void app::glfw_charmod(GLFWwindow *wnd, unsigned int codepoint, int mods) {}
}
