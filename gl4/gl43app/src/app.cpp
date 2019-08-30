#include "app.h"

#include <stdexcept>
#include <iostream>
#include <algorithm>

#include "glm.h"
#include "glenumstring.h"
#include "common_camman.h"

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
app::app()
  :
    m_pause(false),
    m_display_help(true),
    m_update_count(0),
    m_camera_mode(CM_PITCH_YAW),
    m_v_mat(0),
    m_p_mat(0),
    m_reading(GL_FALSE),
    m_frame_number(0),
    m_fps(0),
    m_last_time(0),
    m_current_time(0),
    m_delta_time(0),
    m_camera_move_speed(1.5),
    m_swap_interval(1),
    m_time_update(GL_FALSE)
  {}

//--------------------------------------------------------------------
void app::init() {
  m_shutdown = GL_FALSE;
  m_dirty_view = GL_TRUE;
  init_info();

  m_info.wm.z = wnd_width();
  m_info.wm.w = wnd_height();

  init_wnd();
  init_gl();
  m_text.init();

  set_camera_mode(m_camera_mode);

  // manual resize
  glfw_resize(m_wnd, wnd_width(), wnd_height());
}

//--------------------------------------------------------------------
void app::init_info() {
  m_info.title = "app";
  m_info.wnd_width = 512;
  m_info.wnd_height = 512;
  m_info.wm = uvec4(100, 100, m_info.wnd_width, m_info.wnd_height);
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

  // GL_DEBUG_SEVERITY_MEDIUM : GL_DEBUG_SOURCE_API : GL_DEBUG_TYPE_PERFORMANCE :
  // 131186 : Buffer performance warning: Buffer object 2 (bound to
  // GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING_ARB (0), GL_ARRAY_BUFFER_ARB,
  // GL_SHADER_STORAGE_ BUFFER, and GL_SHADER_STORAGE_BUFFER (1), usage hint is
  // GL_STATIC_DRAW) is being copied/moved from VIDEO memory to HOST memory.

  GLuint ids1[] = {131186};
  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_PERFORMANCE, GL_DONT_CARE,
    sizeof(ids1) / sizeof(GLuint), ids1, GL_FALSE);
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
    m_wnd = glfwCreateWindow(wnd_width(), wnd_height(), m_info.title.c_str(), NULL, NULL);
  }

  if (!m_wnd) {
    std::cerr << "Failed to open window" << std::endl;
    return;
  }

  glfwMakeContextCurrent(m_wnd);
  if(!m_info.fullscreen)
    glfwSetWindowPos(m_wnd, m_info.wm.x, m_info.wm.y);

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
void app::update_camera()
{
  if (m_camman && m_v_mat)
  {
    *m_v_mat = m_camman->get_v_mat();
  }
}

//--------------------------------------------------------------------
void app::update_input_handlers()
{
  for (auto& handler : m_input_handlers)
  {
    handler->update(m_delta_time);
  }

  update_camera();
}

//--------------------------------------------------------------------
void app::run() {
  srand(time(0));
  init();

  glClearColor(0.156863f, 0.156863f, 0.156863f, 1.0f);
  create_scene();
  loop();
}

//--------------------------------------------------------------------
void app::lookat(
  const vec3& eye, const vec3& center, const vec3& up, mat4* v_mat)
{
  m_v_mat = v_mat;
  m_camman->lookat(eye, center, up);
  *v_mat = m_camman->get_v_mat();
}

//--------------------------------------------------------------------
void app::set_v_mat(mat4 *v)
{
  m_v_mat = v;
  if (m_camman)
  {

    auto camman = std::dynamic_pointer_cast<orbit_camman>(m_camman);
    if (camman)
    {
      camman->set_distance(-(*v)[3][2]);
    }

    m_camman->set_v_mat(*v);
  }
}

//--------------------------------------------------------------------
void app::set_camera_mode(camera_mode v) {
  static std::string modes[] = {"CM_PITCH_YAW", "CM_ARCBALL", "CM_FREE"};

  remove_input_handler(m_camman);

  // store original camera info
  mat4 v_mat;
  if (m_camman)
  {
    v_mat = m_camman->get_v_mat();
    auto orbit_camera = std::dynamic_pointer_cast<orbit_camman>(m_camman);
    if (orbit_camera)
    {
      m_orbit_camman_info.distance = orbit_camera->get_distance();
      m_orbit_camman_info.rotation = orbit_camera->get_rotation();
      m_orbit_camman_info.center = orbit_camera->get_center();
    }
  }

  m_camera_mode = v;
  std::cout << "current camera mode : " << modes[m_camera_mode] << std::endl;

  switch (m_camera_mode)
  {
    case CM_FREE:
      m_camman = std::make_shared<free_camman>();
      m_camman->set_v_mat(v_mat);
      break;

    case CM_PITCH_YAW:
      m_camman = std::make_shared<blend_camman>();
      if (m_orbit_camman_info.distance != -1)
      {
        auto camman = std::static_pointer_cast<orbit_camman>(m_camman);
        camman->set_distance(m_orbit_camman_info.distance);
        camman->set_center(m_orbit_camman_info.center);
        camman->set_rotation(m_orbit_camman_info.rotation);
      }
      break;

    case CM_ARCBALL:
      m_camman = std::make_shared<trackball_camman>();
      if (m_orbit_camman_info.distance != -1)
      {
        auto camman = std::static_pointer_cast<orbit_camman>(m_camman);
        camman->set_distance(m_orbit_camman_info.distance);
        camman->set_center(m_orbit_camman_info.center);
        camman->set_rotation(m_orbit_camman_info.rotation);
      }
      break;

    default:
      break;
  }

  add_input_handler(m_camman);
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
const mat4& app::get_view_mat() const
{
  return m_camman->get_v_mat();
}

//--------------------------------------------------------------------
void app::add_input_handler(const std::shared_ptr<glfw_handler>& handler)
{
  auto iter =
    std::find(m_input_handlers.begin(), m_input_handlers.end(), handler);
  if (iter != m_input_handlers.end())
  {
    std::cout << "duplicate input handler ignored" << std::endl;
    return;
  }
  m_input_handlers.push_back(handler);
}

//--------------------------------------------------------------------
void app::remove_input_handler(const std::shared_ptr<glfw_handler>& handler)
{
  m_input_handlers.erase(
    std::remove(m_input_handlers.begin(), m_input_handlers.end(), handler),
    m_input_handlers.end());
}
//--------------------------------------------------------------------
void app::loop() {
  // TODO swap interval not working on my laptop gt635m
  //glfwSwapInterval(m_swap_interval);
  while (!glfwWindowShouldClose(m_wnd)) {
    update_time();
    update_fps();
    // trigger resize for full screen, there are two resize events, don't know
    // why?
    glfwPollEvents();
    update_input_handlers();
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
  m_text.reshape(wnd_width(), wnd_height());
}

//--------------------------------------------------------------------
void app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  (void)wnd;
  (void)scancode;
  (void)mods;

  m_control.handle(wnd, key, scancode, action, mods);
  for (auto& handler : m_input_handlers)
  {
    handler->on_key(wnd, key, scancode, action, mods);
  }

  if (m_reading) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_BACKSPACE:
          m_input.pop_back();
          break;
        case GLFW_KEY_ESCAPE:
          stop_reading();
          break;
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

      case GLFW_KEY_F6:
        reload_shaders();
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
      case GLFW_KEY_H:
        m_display_help ^= 1;
        return;

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
          GLfloat distance = glm::length(vec3((*m_v_mat)[3]));
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *m_v_mat = glm::lookAt(glm::vec3(0.0f, -distance * factor, 0.0f),
            glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
      } break;
      case GLFW_KEY_KP_3: {
        if (m_v_mat) {
          GLfloat distance = glm::length(vec3((*m_v_mat)[3]));
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *m_v_mat = glm::lookAt(glm::vec3(-distance * factor, 0.0f, 0.0f),
            glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
      } break;

      case GLFW_KEY_KP_7: {
        if (m_v_mat) {
          GLfloat distance = glm::length(vec3((*m_v_mat)[3]));
          GLfloat factor = mods & GLFW_MOD_CONTROL ? -1.0f : 1.0f;
          *m_v_mat = glm::lookAt(glm::vec3(0.0f, 0.0f, -distance * factor),
            glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
      } break;
      case GLFW_KEY_C:
        if (m_v_mat) {
          mat4 v_mat_i = glm::inverse(*m_v_mat);
          vec3 eye = vec3(v_mat_i[3]);
          vec3 center = eye + 10.0f * vec3(-glm::row(*m_v_mat, 2));
          *m_v_mat = glm::lookAt(eye, center, pza);
        }
        break;
      case GLFW_KEY_F8:
        m_time_update ^= 1;
        break;
    }
  } else if (action == GLFW_RELEASE) {
  }
}

//--------------------------------------------------------------------
void app::glfw_mouse_button(GLFWwindow *wnd, int button, int action, int mods) {
  //if (action == GLFW_PRESS && GLFW_MOUSE_BUTTON_MIDDLE == button) {
    glfwGetCursorPos(
      m_wnd, &m_last_cursor_position[0], &m_last_cursor_position[1]);
    m_last_cursor_position[1] = glfw_to_gl(m_last_cursor_position[1]);
  //}

    for (auto& handler : m_input_handlers)
    {
      handler->on_mouse_button(wnd, button, action, mods);
    }
}

//--------------------------------------------------------------------
void app::glfw_mouse_move(GLFWwindow* wnd, double x, double y)
{
  m_last_cursor_position = glfw_to_gl(vec2(x, y));
  for (auto& handler : m_input_handlers)
  {
    handler->on_mouse_move(wnd, x, y);
  }
}

//--------------------------------------------------------------------
void app::glfw_mouse_wheel(GLFWwindow *wnd, double xoffset, double yoffset)
{

  for (auto& handler : m_input_handlers)
  {
    handler->on_mouse_wheel(wnd, xoffset, yoffset);
  }

  if(m_camman)
  {
    if(m_v_mat)
      *m_v_mat = m_camman->get_v_mat();
  }

  GLfloat scale = 1 - 0.1 * yoffset;
  if(m_camera_mode == CM_ORTHO)
  {
    if(!m_p_mat)
      return;

    (*m_p_mat)[0][0] /= scale;
    (*m_p_mat)[1][1] /= scale;
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
  mat4 w_mat = zxd::compute_window_mat(0, 0, wnd_width(), wnd_height());
  mat4 xw_mat = w_mat * m;
  vec4 res = glm::inverse(xw_mat) * sp;
  return vec3(res) / res.w;
}

//--------------------------------------------------------------------
GLdouble app::glfw_to_gl(GLdouble y) { return wnd_height()- 1 - y; }

//--------------------------------------------------------------------
vec2 app::glfw_to_gl(vec2 p)
{
  return vec2(p.x, wnd_height()- 1 - p.y);
}

}
