#ifndef GL4_GLC_OBJMAN_H
#define GL4_GLC_OBJMAN_H

#include <glfw_handler.h>

namespace zxd
{

class trackball_objman : public glfw_gl_handler
{

public:

  trackball_objman(mat4* target, mat4* v_mat, const vec2& ball_center,
    GLfloat ball_radius = 0.6);

  void perform_mouse_move(
    GLFWwindow* wnd, const vec2& p0, const vec2& p1) override;

  int get_button() const { return _button; }
  void set_button(int v){ _button = v; }

private:

  int _button;
  mat4* _target;
  mat4* _v_mat;
  vec2 _ball_center;
  GLfloat _ball_radius;
};
}

#endif /* GL4_GLC_OBJMAN_H */
