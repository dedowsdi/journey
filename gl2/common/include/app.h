/*
 * changed from superbible.
 */
#include "glad/glad.h"
#include "glad/glad_glx.h"
#ifndef GL_VERSION_3_0
#include <GL/glu.h>
#endif
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <string>
#include "glm.h"
#include "common.h"

namespace zxd {

void gl_debug_output(GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar *message, const void *user_param);

class app {
public:
  enum camera_mode {
    CM_YAW_PITCH,  // yaw world, pitch camera
    CM_ARCBALL,
    CM_FREE  // yaw, pitch camera
  };
  enum move_dir {
    MD_LEFT = 1 << 0,
    MD_RIGHT = 1 << 1,
    MD_FOWARD = 1 << 2,
    MD_BACK = 1 << 3,
  };

  struct app_info {
    std::string title;
    GLuint wnd_width;
    GLuint wnd_height;
    GLuint major_version;
    GLuint minor_version;
    GLuint samples;
    GLboolean fullscreen;
    GLboolean vsync;
    GLboolean cursor;
    GLboolean stereo;
    GLboolean debug;
    GLint display_mode;  // glut display mode
  };

protected:
  GLboolean m_reading;
  GLboolean m_camera_moving;
  GLboolean m_lmb_down;
  GLboolean m_rmb_down;
  GLboolean m_mmb_down;
  GLuint m_move_dir;
  GLuint m_swap_interval{1};
  GLuint m_frame_number;
  GLdouble m_fps;
  GLdouble m_last_time;
  GLdouble m_current_time;
  GLdouble m_delta_time;

  GLdouble m_adtx;
  GLdouble m_adty;

  std::string m_input;
  camera_mode m_camera_mode;
  app_info m_info;
  mat4 *m_v_mat;
  mat4 m_start_v_mat;  // used to record init rotation in CM_FREE.

  dvec2
    m_last_cursor_position;   // used to rotate camera when mid button pressed
  vec3 m_camera_translation;  // translation during CM_FREE mode

public:
  app()
      : m_camera_mode(CM_YAW_PITCH),
        m_v_mat(0),
        m_reading(GL_FALSE),
        m_camera_moving(GL_FALSE),
        m_move_dir(0),
        m_frame_number(0),
        m_adtx(0),
        m_adty(0),
        m_fps(0),
        m_last_time(0),
        m_current_time(0),
        m_delta_time(0),
        m_lmb_down(GL_FALSE),
        m_rmb_down(GL_FALSE),
        m_mmb_down(GL_FALSE)
  {}

  void init(int argc, char **argv);
  virtual void run(int argc, char **argv);
  GLfloat wnd_aspect() {
    return static_cast<GLfloat>(m_info.wnd_width) / m_info.wnd_height;
  }

  mat4 * get_v_mat() const { return m_v_mat; }
  void set_v_mat(mat4 *v) { m_v_mat = v; }

  camera_mode get_camera_mode() const { return m_camera_mode; }
  void set_camera_mode(camera_mode v);

  void start_reading();
  void stop_reading();
  void finishe_reading();

protected:
  virtual void init_info();
  virtual void init_gl();
  virtual void init_callback();
  virtual void debug_message_control();
  virtual void init_wnd(int argc, char **argv);
  virtual void create_scene() {}
  virtual void update() {}
  virtual void update_time();
  virtual void update_fps();
  virtual void update_camera();
  virtual void drawText(const std::string& text);
  virtual void drawFpsText();

  virtual void loop();
  virtual void shutdown() {}

  virtual void display(void) {}
  virtual void reshape(int w, int h);
  virtual void keyboard(unsigned char key, int x, int y);
  virtual void keyboard_up(unsigned char key, int x, int y);
  virtual void special(int key, int x, int y);
  virtual void special_up(int key, int x, int y);
  virtual void mouse(int button, int state, int x, int y);
  virtual void mouse_wheel(int wheel, int direction, int x, int y);
  virtual void passive_motion(int x, int y);
  virtual void motion(int x, int y);
  virtual void idle(void);
  GLuint glut_to_gl(GLuint y);
};

}
