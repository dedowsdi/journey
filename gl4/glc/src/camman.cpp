#include "camman.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "glmath.h"

namespace zxd
{

//--------------------------------------------------------------------
camman::camman()
{
}

//--------------------------------------------------------------------
orbit_camman::orbit_camman():
  camman()
{
}

//--------------------------------------------------------------------
void orbit_camman::on_mouse_button(
  GLFWwindow* wnd, int button, int action, int mods)
{
  if (button != _button)
    return;

  camman::on_mouse_button(wnd, button, action, mods);
}

//--------------------------------------------------------------------
void orbit_camman::on_mouse_move(GLFWwindow* wnd, double x, double y)
{
  if (glfwGetMouseButton(wnd, _button) != GLFW_PRESS) return;
  camman::on_mouse_move(wnd, x, y);
}

//--------------------------------------------------------------------
const mat4& orbit_camman::get_v_mat() const
{
  if(_dirty)
  {
    auto v_mat_i =
      translate(_center) * _rotation * translate(vec3(0, 0, _distance));
    _v_mat = inverse(v_mat_i);
    _dirty = false;
  }
  return _v_mat;
}

//--------------------------------------------------------------------
void orbit_camman::set_v_mat(const mat4& v_mat)
{
  assert(_distance > 0);

  auto v_mat_i = inverse(v_mat);
  set_rotation( erase_translation(v_mat_i) );
  auto center = v_mat_i * vec4(0, 0, -get_distance(), 1);
  set_center(vec3(center));
}

//--------------------------------------------------------------------
void orbit_camman::lookat(const vec3& eye, const vec3& center, const vec3& up) 
{
  set_distance(distance(eye, center));
  set_v_mat(lookAt(eye, center, up));
}

//--------------------------------------------------------------------
void orbit_camman::perform_mouse_wheel(GLFWwindow* wnd, const vec2& offset)
{
  GLfloat scale = 1 - get_wheel_scale() * offset.y;
  _distance *= scale;
  dirty();
}

}
