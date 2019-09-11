#include <objman.h>

#include <glm/mat4x4.hpp>
#include <glmath.h>

namespace zxd
{

//--------------------------------------------------------------------
trackball_objman::trackball_objman(
  mat4* target, mat4* v_mat, const vec2& ball_center, GLfloat ball_radius)
    : _target(target), _v_mat(v_mat), _ball_center(ball_center),
      _ball_radius(ball_radius), _button(GLFW_MOUSE_BUTTON_RIGHT)
{
}

//--------------------------------------------------------------------
void trackball_objman::perform_mouse_move(
  GLFWwindow* wnd, const vec2& p0, const vec2& p1) 
{
  if(glfwGetMouseButton(wnd, _button) != GLFW_PRESS)
    return;

  auto r = glfw_min_wh(wnd) * _ball_radius;
  auto ball_rotate = trackball_rotate(p0, p1, _ball_center, r);

  auto mv_mat = *_v_mat * *_target;
  mv_mat[3] = vec4(0, 0, 0, 1);
  auto mv_mat_i = inverse(mv_mat);

  *_target *= mv_mat_i * ball_rotate * mv_mat;
}

}
