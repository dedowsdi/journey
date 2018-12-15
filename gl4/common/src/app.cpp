#include "app.h"
#include <stdexcept>
#include <iostream>
#include "glenumstring.h"

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
  m_shutdown = GL_FALSE;
  m_dirty_view = GL_TRUE;
  init_info();

  m_info.wm.z = m_info.wnd_width;
  m_info.wm.w = m_info.wnd_height;

  init_wnd();
  init_gl();
}

//--------------------------------------------------------------------
void app::init_info() {
  m_info.title = "app";
  m_info.wnd_width = 512;
  m_info.wnd_height = 512;
  m_info.wm = uvec4(100, 100, 0, 0);
  m_info.major_version = 4;
  m_info.minor_version = 3;
  m_info.samples = 4;
  m_info.fullscreen = GL_FALSE;
  m_info.vsync = GL_FALSE;
  m_info.cursor = GL_TRUE;
  m_info.stereo = GL_FALSE;
  m_info.debug = GL_TRUE;
  m_info.decorated = GL_TRUE;
  m_info.double_buffer = GL_TRUE;
}

//--------------------------------------------------------------------
void app::init_gl() {
#ifndef CLANG_COMPLETE_ONLY
  if (!gladLoadGL()) {
    std::cerr << "glad failed to load gl" << std::endl;
    return;
  }
  if (GLVersion.major < 4) {
    std::cerr << "Your system doesn't support OpenGL >= 4!" << std::endl;
  }
#endif
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

  if(m_info.samples == 1)
    glDisable(GL_MULTISAMPLE);
  std::cout << "init gl finished" << std::endl;
}

//--------------------------------------------------------------------
void app::debug_message_control() {
  // GL_DEBUG_SEVERITY_NOTIFICATION : GL_DEBUG_SOURCE_API : GL_DEBUG_TYPE_OTHER : 131185 : Buffer detailed info:
  // Buffer object 1 (bound to GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (0), and GL_ARRAY_BUFFER_ARB, usage hint
  // is GL_STREAM_DRAW) will use VIDEO memory as the source for buffer object operations.
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
  glfwWindowHint(GLFW_DECORATED, m_info.decorated);
  glfwWindowHint(GLFW_DOUBLEBUFFER, m_info.double_buffer);

  if(m_info.fullscreen)
  {
    auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    // although wnd_width and wnd_height will be updated in resize, sometimes
    // you need them in create_screen, which is called before resize.
    m_info.wnd_width = mode->width;
    m_info.wnd_height = mode->height;
    m_wnd = glfwCreateWindow(mode->width, mode->height, m_info.title.c_str(), glfwGetPrimaryMonitor(), NULL);
  }
  else
  {
    m_wnd = glfwCreateWindow(m_info.wnd_width, m_info.wnd_height, m_info.title.c_str(), NULL, NULL);
  }

  if (!m_wnd) {
    std::cerr << "Failed to open window" << std::endl;
    return;
  }

  glfwMakeContextCurrent(m_wnd);

  // set up call back
  glfwSetWindowUserPointer(m_wnd, this);
  if(!m_info.fullscreen)
    glfwSetWindowPos(m_wnd, m_info.wm.x, m_info.wm.y);

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
  std::cout << "init wnd finished" << std::endl;
}

//--------------------------------------------------------------------
void app::update_time() {
  m_last_time = m_current_time;
  m_current_time = glfwGetTime();
  m_delta_time = m_current_time - m_last_time;
}

//--------------------------------------------------------------------
void app::update_fps() {
  static GLdouble time = 0;
  static GLdouble count = 0;

  time += m_delta_time;
  ++count;

  if (time >= 1) {
    m_fps = count;
    time -= 1;
    count = 0;
  }
}

//--------------------------------------------------------------------
void app::update_camera() {
  if (!m_v_mat) return;

  if (m_camera_mode == CM_FREE && m_camera_moving == 1) {
    vec3 dir(0);
    if (m_move_dir & MD_LEFT) dir.x = 1;
    if (m_move_dir & MD_RIGHT) dir.x = -1;
    if (m_move_dir & MD_FOWARD) dir.z = 1;
    if (m_move_dir & MD_BACK) dir.z = -1;

    dir = glm::normalize(dir);
    (*m_v_mat)[3] +=
      vec4(dir * static_cast<GLfloat>(m_delta_time * m_camera_move_speed), 0);
  }
}

//--------------------------------------------------------------------
void app::run() {
  srand(time(0));
  init();
  create_scene();
  loop();
}

//--------------------------------------------------------------------
void app::set_camera_mode(camera_mode v) {
  static std::string modes[] = {"CM_YAW_PITCH", "CM_ARCBALL", "CM_FREE"};
  m_camera_mode = v;
  std::cout << "current camera mode : " << modes[m_camera_mode] << std::endl;

  if (m_camera_mode == CM_FREE) {
    m_camera_translation = vec3(0);

    // hide, fix cursor
    glfwSetInputMode(m_wnd, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPos(m_wnd, m_info.wnd_width / 2.0, m_info.wnd_height / 2.0);

    glfwGetCursorPos(
      m_wnd, &m_last_cursor_position[0], &m_last_cursor_position[1]);
    m_last_cursor_position[1] = glfw_to_gl(m_last_cursor_position[1]);
  } else {
    glfwSetInputMode(m_wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
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
void app::toggle_full_screen()
{
  m_info.fullscreen ^= 1;
  const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  if(m_info.fullscreen)
  {
    glfwSetWindowMonitor(m_wnd, glfwGetPrimaryMonitor(), 0, 0, mode->width, mode->height, mode->refreshRate);
  }
  else
  {
    glfwSetWindowMonitor(m_wnd, NULL, m_info.wm.x, m_info.wm.y, m_info.wm.z, m_info.wm.w, mode->refreshRate);
  }

}

//--------------------------------------------------------------------
void app::loop() {
  // TODO swap interval not working on my laptop gt635m
  //glfwSwapInterval(m_swap_interval);
  while (!glfwWindowShouldClose(m_wnd)) {
    update_time();
    update_fps();
    update_camera();
    // trigger resize for full screen, there are two resize events, don't know
    // why?
    glfwPollEvents();
    if(!m_pause || m_update_count > 0)
    {
      if(m_time_update)
        m_timer.reset();

      update();

      if(m_time_update)
        std::cout << "update : " << m_timer.time_miliseconds() << "ms" << std::endl;
    }
    if(m_pause && m_update_count > 0) --m_update_count;
    display();
    if(m_info.double_buffer)
      glfwSwapBuffers(m_wnd);
    else
      glFinish();
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
  std::cout << "resizing " <<  w << ":" << h << std::endl;
  m_info.wnd_width = w;
  m_info.wnd_height = h;
  glViewport(0, 0, w, h);
}

//--------------------------------------------------------------------
void app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  (void)wnd;
  (void)scancode;
  (void)mods;

  m_control.handle(wnd, key, scancode, action, mods);

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
        std::cout << "closing window" << std::endl;
        break;
      case GLFW_KEY_KP_SUBTRACT:
        set_camera_mode(static_cast<camera_mode>((m_camera_mode + 1) % 3));
        break;
      case GLFW_KEY_KP_0: {
        GLint polygon_mode[2];
        glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
        polygon_mode[0] = GL_POINT + (polygon_mode[0] - GL_POINT + 1) % 3;
        glPolygonMode(GL_FRONT_AND_BACK, polygon_mode[0]);
        std::cout << "polygon mode : "
                  << gl_polygon_mode_to_string(polygon_mode[0]) << std::endl;
      } break;
      case GLFW_KEY_KP_2: {
        glfwSwapInterval(m_swap_interval ^= 1);
        std::cout << "swap interval : " << m_swap_interval << std::endl;
      } break;

      case GLFW_KEY_F9:
        m_pause = !m_pause;
        break;

      case GLFW_KEY_F10:
        ++m_update_count;
        break;

      case GLFW_KEY_SPACE:
        if(mods & GLFW_MOD_CONTROL)
          toggle_full_screen();
        break;

      case GLFW_KEY_LEFT_BRACKET:
        m_control.index(m_control.index() - 1);
        break;

      case GLFW_KEY_RIGHT_BRACKET:
        m_control.index(m_control.index() + 1);
        break;

      default:
        break;
    }
  }

  // camera
  if (m_v_mat == 0) return;
  if (action == GLFW_PRESS) {
    switch (key) {
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
      case GLFW_KEY_C:
        if (m_v_mat) {
          mat4 v_mat_i = glm::inverse(*m_v_mat);
          vec3 eye = v_mat_i[3].xyz();
          vec3 center = eye + 10.0f * -glm::row(*m_v_mat, 2).xyz();
          *m_v_mat = glm::lookAt(eye, center, pza);
        }
        break;
      case GLFW_KEY_F8:
        m_time_update ^= 1;
        break;
      case GLFW_KEY_LEFT:
        m_move_dir |= MD_LEFT;
        m_camera_moving = 1;
        break;

      case GLFW_KEY_RIGHT:
        m_move_dir |= MD_RIGHT;
        m_camera_moving = 1;
        break;

      case GLFW_KEY_UP:
        m_move_dir |= MD_FOWARD;
        m_camera_moving = 1;
        break;

      case GLFW_KEY_DOWN:
        m_move_dir |= MD_BACK;
        m_camera_moving = 1;
        break;
    }
  } else if (action == GLFW_RELEASE) {
    switch (key) {
      case GLFW_KEY_LEFT:
        m_move_dir &= ~MD_LEFT;
        m_camera_moving = m_move_dir == 0 ? 0 : 1;
        break;
      case GLFW_KEY_RIGHT:
        m_move_dir &= ~MD_RIGHT;
        m_camera_moving = m_move_dir == 0 ? 0 : 1;
        break;
      case GLFW_KEY_UP:
        m_move_dir &= ~MD_FOWARD;
        m_camera_moving = m_move_dir == 0 ? 0 : 1;
        break;
      case GLFW_KEY_DOWN:
        m_move_dir &= ~MD_BACK;
        m_camera_moving = m_move_dir == 0 ? 0 : 1;
        break;
    }
  }
}

//--------------------------------------------------------------------
void app::glfw_mouse_button(GLFWwindow *wnd, int button, int action, int mods) {
  //if (action == GLFW_PRESS && GLFW_MOUSE_BUTTON_MIDDLE == button) {
    glfwGetCursorPos(
      m_wnd, &m_last_cursor_position[0], &m_last_cursor_position[1]);
    m_last_cursor_position[1] = glfw_to_gl(m_last_cursor_position[1]);
  //}
}

//--------------------------------------------------------------------
void app::glfw_mouse_move(GLFWwindow *wnd, double x, double y) {

  y = glfw_to_gl(y);
  GLdouble dtx = x - m_last_cursor_position[0];
  GLdouble dty = y - m_last_cursor_position[1];
  m_last_cursor_position[0] = x;
  m_last_cursor_position[1] = y;

  if (!m_v_mat) return;

  if (m_camera_mode == CM_YAW_PITCH) {
    if (glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS)
      return;

    m_dirty_view = GL_TRUE;
    // yaw world, assume z up
    if (dtx != 0) {
      *m_v_mat *= glm::rotate(static_cast<GLfloat>(dtx * 0.02), vec3(0, 0, 1));
    }

    // pitch camera, but reserve center
    if (dty != 0) {
      glm::vec3 translation = glm::column(*m_v_mat, 3).xyz();

      // translate world to camera
      set_col(*m_v_mat, 3, zp);

      // rotate, translate world back
      *m_v_mat = glm::translate(translation) *
                 glm::rotate(static_cast<GLfloat>(-dty * 0.02), vec3(1, 0, 0)) *
                 *m_v_mat;
    }
  } else if (m_camera_mode == CM_ARCBALL) {
    if (glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS)
      return;

    m_dirty_view = GL_TRUE;
    if (dtx != 0 || dty != 0) {
      mat4 w_mat_i = zxd::compute_window_mat_i(
        0, 0, m_info.wnd_width, m_info.wnd_height, 0, 1);
      mat4 m = zxd::arcball(m_last_cursor_position, glm::vec2(x, y), w_mat_i);

      glm::vec3 translation = glm::column(*m_v_mat, 3).xyz();
      // translate world to camera
      set_col(*m_v_mat, 3, zp);

      // rotate, translate world back
      *m_v_mat = glm::translate(translation) * m * *m_v_mat;
    }
  } else if (m_camera_mode == CM_FREE) {
    // fix cursor
    glfwSetCursorPos(m_wnd, m_info.wnd_width / 2.0, m_info.wnd_height / 2.0);

    *m_v_mat = glm::rotate(static_cast<GLfloat>(-dty) * 0.002f, vec3(1, 0, 0)) *
               glm::rotate(static_cast<GLfloat>(dtx) * 0.002f, vec3(0, 1, 0)) *
               *m_v_mat;

    // use lookat to fix camera up, reserve camera position.
    // vec3 eye = eye_pos(*m_v_mat);
    // vec3 center = eye + 10.0f * -glm::row(*m_v_mat, 2).xyz();
    //*m_v_mat = glm::lookAt(eye, center, pza);

    vec3 forward = -glm::row(*m_v_mat, 2).xyz();
    vec3 right = cross(forward, pza);
    if (glm::length(right) < 0.0001f) {
      return;
    }
    m_dirty_view = GL_TRUE;
    right = glm::normalize(right);

    vec3 fixed_up = normalize(cross(right, forward));

    // reserve eye position, use old eye position to deduce new world
    // translation
    *m_v_mat = glm::translate(
      make_mat4_row(right, fixed_up, -forward), -eye_pos(*m_v_mat));
  }
}

//--------------------------------------------------------------------
void app::glfw_mouse_wheel(GLFWwindow *wnd, double xoffset, double yoffset) {
  // yoffset is negative if you scroll toward yourself
  if (m_v_mat) {
    m_dirty_view = GL_TRUE;
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

//--------------------------------------------------------------------
vec2 app::current_mouse_position()
{
  dvec2 p;
  glfwGetCursorPos(m_wnd, &p[0], &p[1]);
  return glfw_to_gl(p);
}

//--------------------------------------------------------------------
vec3 app::unproject(vec3 p, const mat4& m)
{
  vec4 sp(p, 1);
  mat4 w_mat = zxd::compute_window_mat(0, 0, m_info.wnd_width, m_info.wnd_height);
  mat4 xw_mat = w_mat * m;
  vec4 res = glm::inverse(xw_mat) * sp;
  return res.xyz() / res.w;
}

//--------------------------------------------------------------------
GLdouble app::glfw_to_gl(GLdouble y) { return m_info.wnd_height - 1 - y; }

//--------------------------------------------------------------------
vec2 app::glfw_to_gl(vec2 p)
{
  return vec2(p.x, m_info.wnd_height - 1 - p.y);
}

}
