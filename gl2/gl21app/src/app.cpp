#include <app.h>
#include <stdexcept>
#include <iostream>
#include <glenumstring.h>

namespace zxd {

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
void app::init(int argc, char **argv) {
  init_info();
  init_wnd(argc, argv);
  init_gl();
  init_callback();
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
  m_info.vsync = GL_TRUE;
  m_info.cursor = GL_TRUE;
  m_info.stereo = GL_FALSE;
  m_info.debug = GL_TRUE;
  m_info.display_mode = GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH;
}

//--------------------------------------------------------------------
void app::init_gl() {
  if (!gladLoadGL()) {
    std::cerr << "glad failed to load gl" << std::endl;
    return;
  }

  Display *dpy = XOpenDisplay(NULL);
  if (!gladLoadGLX(dpy, 0)) {
    std::cerr << "glad failed to load glx" << std::endl;
    return;
  }

  GLXDrawable drawable = glXGetCurrentDrawable();
  glXSwapIntervalEXT(dpy, drawable, m_swap_interval);

  unsigned int swap, maxSwap;
  if (drawable) {
    glXQueryDrawable(dpy, drawable, GLX_SWAP_INTERVAL_EXT, &swap);
    glXQueryDrawable(dpy, drawable, GLX_MAX_SWAP_INTERVAL_EXT, &maxSwap);
    printf(
      "current swap interval is %u and the max swap interval is "
      "%u\n",
      swap, maxSwap);
  }
  if (GLVersion.major < 4) {
    std::cerr << "your system doesn't support OpenGL >= 4!" << std::endl;
  }
  printf(
    "GL_VERSION : %s\n"
    "GL_SHADING_LANGUAGE_VERSION : %s\n"
    "GL_RENDERER : %s\n"
    "GL_VENDOR : %s\n"
    "GL_SHADING_LANGUAGE_VERSION : %s\n"
    "GLU VERSION : %s\n",
    glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION),
    glGetString(GL_RENDERER), glGetString(GL_VENDOR),
    glGetString(GL_SHADING_LANGUAGE_VERSION),
    gluGetString(GLU_VERSION)
    );

  // init debugger
  glEnable(GL_DEBUG_OUTPUT);
  glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
  glDebugMessageCallback(gl_debug_output, this);
  debug_message_control();
}

//--------------------------------------------------------------------
void app::init_callback() {
  // lambda with capture can't be converted to c func pointer, so i have to use
  // static variable
  static app *instance = this;

  auto display_callback = []() { instance->display(); };
  auto reshape_callback = [](int w, int h) { instance->reshape(w, h); };
  auto keyboard_callback = [](
    unsigned char key, int x, int y) { instance->keyboard(key, x, y); };
  auto keyboard_up_callback = [](
    unsigned char key, int x, int y) { instance->keyboard_up(key, x, y); };
  auto special_callback = [](
    int key, int x, int y) { instance->special(key, x, y); };
  auto special_up_callback = [](
    int key, int x, int y) { instance->special_up(key, x, y); };
  auto mouse_callback = [](int button, int state, int x, int y) {
    instance->mouse(button, state, x, y);
  };
  auto mouse_wheel_callback = [](int wheel, int direction, int x, int y) {
    instance->mouse_wheel(wheel, direction, x, y);
  };
  auto passive_motion_callback = [](
    int x, int y) { instance->passive_motion(x, y); };
  auto motion_callback = [](int x, int y) { instance->motion(x, y); };
  auto idle_callback = []() { instance->idle(); };

  glutDisplayFunc(display_callback);
  glutReshapeFunc(reshape_callback);
  glutKeyboardFunc(keyboard_callback);
  glutKeyboardUpFunc(keyboard_up_callback);
  glutSpecialFunc(special_callback);
  glutSpecialUpFunc(special_up_callback);
  glutMouseFunc(mouse_callback);
  glutMouseWheelFunc(mouse_wheel_callback);
  glutPassiveMotionFunc(passive_motion_callback);
  glutMotionFunc(motion_callback);
  glutIdleFunc(idle_callback);

}

//--------------------------------------------------------------------
void app::debug_message_control() {
  GLuint ids[] = {131185};

  glDebugMessageControl(GL_DEBUG_SOURCE_API, GL_DEBUG_TYPE_OTHER, GL_DONT_CARE,
    sizeof(ids) / sizeof(GLuint), ids, GL_FALSE);
}

//--------------------------------------------------------------------
void app::init_wnd(int argc, char **argv) {
  glutInit(&argc, argv);
  if(m_info.samples > 1)
  {
    glutSetOption(GLUT_MULTISAMPLE, m_info.samples);
    m_info.display_mode |= GLUT_MULTISAMPLE;
  }

  glutInitDisplayMode(m_info.display_mode);
  glutInitWindowSize(m_info.wnd_width, m_info.wnd_height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(m_info.title.c_str());

}

//--------------------------------------------------------------------
void app::update_time() {
  m_last_time = m_current_time;
  m_current_time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
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
    m_camera_translation += dir * static_cast<GLfloat>(m_delta_time);
    (*m_v_mat)[3] = vec4(vec3(m_start_v_mat[3]) + m_camera_translation, 1);
  }
}

//--------------------------------------------------------------------
void app::drawText(const std::string& text)
{
  glWindowPos2i(10, m_info.wnd_height - 10);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)text.c_str());
}

//--------------------------------------------------------------------
void app::drawFpsText()
{
  GLchar info[50];
  sprintf(info, "fps : %.2f", m_fps);
  drawText(info);
}

//--------------------------------------------------------------------
void app::run(int argc, char **argv) {
  srand(time(0));
  init(argc, argv);
  create_scene();
  loop();
}

//--------------------------------------------------------------------
void app::set_camera_mode(camera_mode v) {
  static std::string modes[] = {"CM_YAW_PITCH", "CM_ARCBALL", "CM_FREE"};
  m_camera_mode = v;
  std::cout << "current camera mode : " << modes[m_camera_mode] << std::endl;

  if (m_camera_mode == CM_FREE) {
    m_start_v_mat = *m_v_mat;
    m_camera_translation = vec3(0);

    // hide, fix cursor
    glutSetCursor(GLUT_CURSOR_CROSSHAIR);
    glutWarpPointer(m_info.wnd_width / 2.0, m_info.wnd_height / 2.0);
    m_adtx = m_adty = 0;
    m_last_cursor_position =
      vec2(m_info.wnd_width / 2.0, m_info.wnd_height / 2.0);
  } else {
    glutSetCursor(GLUT_CURSOR_LEFT_ARROW);
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
void app::loop() {
  // while(true){
  // glutMainLoopEvent();
  //}
  glutMainLoop();
}

//--------------------------------------------------------------------
void app::reshape(int w, int h) {
  m_info.wnd_width = w;
  m_info.wnd_height = h;
  glViewport(0, 0, w, h);
}

//--------------------------------------------------------------------
void app::keyboard(unsigned char key, int x, int y) {
  if (m_reading) {
    switch (key) {
      case 8:  // back space
        m_input.pop_back();
        break;
      case 27:  // escape
        stop_reading();
      default:
        m_input.push_back(key);
        break;
    }
    return;
  }

  switch (key) {
    case 27:  // escape
      std::cout << "closing window" << std::endl;
      exit(0);
      break;
    case '-':
      set_camera_mode(static_cast<camera_mode>((m_camera_mode + 1) % 3));
      break;
    case '0': {
      GLint polygon_mode[2];
      glGetIntegerv(GL_POLYGON_MODE, polygon_mode);
      polygon_mode[0] = GL_POINT + (polygon_mode[0] - GL_POINT + 1) % 3;
      glPolygonMode(GL_FRONT_AND_BACK, polygon_mode[0]);
      std::cout << "polygon mode : "
                << gl_polygon_mode_to_string(polygon_mode[0]) << std::endl;
    } break;
    case '2': {
      m_swap_interval ^= 1;
      std::cout << "swap interval in glut? : " << m_swap_interval << std::endl;

      Display *dpy = glXGetCurrentDisplay();
      GLXDrawable drawable = glXGetCurrentDrawable();
      glXSwapIntervalEXT(dpy, drawable, m_swap_interval);

      unsigned int swap, maxSwap;
      if (drawable) {
        glXQueryDrawable(dpy, drawable, GLX_SWAP_INTERVAL_EXT, &swap);
        glXQueryDrawable(dpy, drawable, GLX_MAX_SWAP_INTERVAL_EXT, &maxSwap);
        printf(
          "The swap interval is %u and the max swap interval is "
          "%u\n",
          swap, maxSwap);
      }
    } break;

    default:
      break;
  }

  // camera
  if (m_v_mat == 0) return;
  switch (key) {
    case '1': {
      if (m_v_mat) {
        GLfloat distance = glm::length(vec3((*m_v_mat)[3]));
        GLfloat factor = glutGetModifiers() & GLUT_KEY_CTRL_L ? -1.0f : 1.0f;
        *m_v_mat = glm::lookAt(glm::vec3(0.0f, -distance * factor, 0.0f),
          glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      }
    } break;
    case '3': {
      if (m_v_mat) {
        GLfloat distance = glm::length(vec3((*m_v_mat)[3]));
        GLfloat factor = glutGetModifiers() & GLUT_KEY_CTRL_L ? -1.0f : 1.0f;
        *m_v_mat = glm::lookAt(glm::vec3(-distance * factor, 0.0f, 0.0f),
          glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
      }
    } break;

    case '7': {
      if (m_v_mat) {
        GLfloat distance = glm::length(vec3((*m_v_mat)[3]));
        GLfloat factor = glutGetModifiers() & GLUT_KEY_CTRL_L ? -1.0f : 1.0f;
        *m_v_mat = glm::lookAt(glm::vec3(0.0f, 0.0f, -distance * factor),
          glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
      }
    } break;
  }
}

//--------------------------------------------------------------------
void app::keyboard_up(unsigned char key, int x, int y) {}

//--------------------------------------------------------------------
void app::special(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_LEFT:
      m_move_dir |= MD_LEFT;
      m_camera_moving = 1;
      break;

    case GLUT_KEY_RIGHT:
      m_move_dir |= MD_RIGHT;
      m_camera_moving = 1;
      break;

    case GLUT_KEY_UP:
      m_move_dir |= MD_FOWARD;
      m_camera_moving = 1;
      break;

    case GLUT_KEY_DOWN:
      m_move_dir |= MD_BACK;
      m_camera_moving = 1;
      break;
    default:
      break;
  }
}

//--------------------------------------------------------------------
void app::special_up(int key, int x, int y) {
  switch (key) {
    case GLUT_KEY_LEFT:
      m_move_dir &= ~MD_LEFT;
      m_camera_moving = m_move_dir == 0 ? 0 : 1;
      break;
    case GLUT_KEY_RIGHT:
      m_move_dir &= ~MD_RIGHT;
      m_camera_moving = m_move_dir == 0 ? 0 : 1;
      break;
    case GLUT_KEY_UP:
      m_move_dir &= ~MD_FOWARD;
      m_camera_moving = m_move_dir == 0 ? 0 : 1;
      break;
    case GLUT_KEY_DOWN:
      m_move_dir &= ~MD_BACK;
      m_camera_moving = m_move_dir == 0 ? 0 : 1;
      break;
  }
}

//--------------------------------------------------------------------
void app::mouse(int button, int state, int x, int y) {
  if (button == GLUT_LEFT_BUTTON) m_lmb_down = state == GLUT_DOWN;
  if (button == GLUT_RIGHT_BUTTON) m_rmb_down = state == GLUT_DOWN;
  if (button == GLUT_MIDDLE_BUTTON) m_mmb_down = state == GLUT_DOWN;

  if (state == GLUT_DOWN && GLUT_MIDDLE_BUTTON == button) {
    m_last_cursor_position = glm::vec2(x, glut_to_gl(y));
  }
}

//--------------------------------------------------------------------
void app::mouse_wheel(int wheel, int direction, int x, int y) {
  // direction is -1 if you scroll toward yourself
  if (m_v_mat) {
    GLfloat scale = 1 - 0.1 * direction;
    (*m_v_mat)[3][0] *= scale;
    (*m_v_mat)[3][1] *= scale;
    (*m_v_mat)[3][2] *= scale;
  }
}

//--------------------------------------------------------------------
void app::passive_motion(int x, int y) {
  y = glut_to_gl(y);
  GLdouble dtx = x - m_last_cursor_position[0];
  GLdouble dty = y - m_last_cursor_position[1];

  if (m_camera_mode == CM_FREE) {
    m_adtx += dtx;
    m_adty += dty;
    *m_v_mat =
      glm::rotate(static_cast<GLfloat>(m_adtx) * 0.002f, vec3(0, 1, 0)) *
      glm::rotate(static_cast<GLfloat>(-m_adty) * 0.002f, vec3(1, 0, 0)) *
      m_start_v_mat;

    (*m_v_mat)[3] = vec4(vec3(m_start_v_mat[3]) + m_camera_translation, 1);

    glutWarpPointer(m_info.wnd_width / 2.0, m_info.wnd_height / 2.0);
  }
}

//--------------------------------------------------------------------
void app::motion(int x, int y) {
  if (!m_v_mat || !m_mmb_down) return;

  y = glut_to_gl(y);
  GLdouble dtx = x - m_last_cursor_position[0];
  GLdouble dty = y - m_last_cursor_position[1];
  if (m_camera_mode == CM_YAW_PITCH) {
    // yaw world, assume z up
    if (dtx != 0) {
      *m_v_mat *= glm::rotate(static_cast<GLfloat>(dtx * 0.02), vec3(0, 0, 1));
    }

    // pitch camera, but reserve center
    if (dty != 0) {
      glm::vec3 translation = vec3(glm::column(*m_v_mat, 3));

      // translate world to camera
      (*m_v_mat)[3][0] = 0;
      (*m_v_mat)[3][1] = 0;
      (*m_v_mat)[3][2] = 0;

      // rotate, translate world back
      *m_v_mat = glm::translate(translation) *
                 glm::rotate(static_cast<GLfloat>(-dty * 0.02), vec3(1, 0, 0)) *
                 *m_v_mat;
    }
    m_last_cursor_position[0] = x;
    m_last_cursor_position[1] = y;
  } else if (m_camera_mode == CM_ARCBALL) {
    if (dtx != 0 || dty != 0) {
      mat4 m = zxd::trackball_rotate(m_last_cursor_position, vec2(x, y),
        vec2(m_info.wnd_width, m_info.wnd_height) * 0.5f,
        min(m_info.wnd_width, m_info.wnd_height) * 0.6f);

      glm::vec3 translation = vec3(glm::column(*m_v_mat, 3));
      // translate world to camera
      (*m_v_mat)[3][0] = 0;
      (*m_v_mat)[3][1] = 0;
      (*m_v_mat)[3][2] = 0;

      // rotate, translate world back
      *m_v_mat = glm::translate(translation) * m * *m_v_mat;
    }
    m_last_cursor_position[0] = x;
    m_last_cursor_position[1] = y;
  }
}

//--------------------------------------------------------------------
void app::idle(void) {
  update_time();
  update_fps();
  update_camera();
  update();
  glutPostRedisplay();
  ++m_frame_number;
  // std::cout << m_frame_number<< std::endl;
}

//--------------------------------------------------------------------
GLuint app::glut_to_gl(GLuint y) { return m_info.wnd_height - 1 - y; }
}
