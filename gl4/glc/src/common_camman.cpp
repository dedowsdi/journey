#include "common_camman.h"

#include "glmath.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>

namespace zxd
{

//--------------------------------------------------------------------
blend_camman::blend_camman():
  orbit_camman()
{
}

//--------------------------------------------------------------------
void blend_camman::on_key(
  GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (key)
    {
    case GLFW_KEY_KP_1:
      set_rotation(rotate(mods & GLFW_MOD_CONTROL ? fpi : 0, vec3(0, 0, 1)));
      break;
    case GLFW_KEY_KP_3:
      set_rotation(rotate(mods & GLFW_MOD_CONTROL ? -fpi2 : fpi2, vec3(0, 1, 0)));
      break;
    case GLFW_KEY_KP_7:
      set_rotation(rotate(mods & GLFW_MOD_CONTROL ? fpi2 : -fpi2, vec3(1, 0, 0)));
    break;
    case GLFW_KEY_C:
      set_rotation(mat4(1));
      set_distance(100);
      break;
    }
  }
  else if (action == GLFW_RELEASE)
  {
  }
}
//--------------------------------------------------------------------
void blend_camman::perform_mouse_move(
  GLFWwindow* wnd, const vec2& p0, const vec2& p1)
{
  // v_r_mat = pitch(-dy) * v_r_mat * yaw(dx)
  auto offset = p1 - p0;
  auto yaw = rotate(-offset.x * get_move_scale(), vec3(0, 0, 1));
  auto pitch = rotate(offset.y * get_move_scale(), vec3(1, 0, 0));
  set_rotation(yaw * get_rotation() * pitch);
}

//--------------------------------------------------------------------
trackball_camman::trackball_camman() : orbit_camman(), _ball_radius(0.6)
{
  _ball_center = vec2(glfw_win_size()) * 0.5f;
}

//--------------------------------------------------------------------
void trackball_camman::perform_mouse_move(
  GLFWwindow* wnd, const vec2& p0, const vec2& p1)
{
  auto r = glfw_min_wh(wnd) * _ball_radius;
  auto ball_rotate = trackball_rotate(p1, p0, _ball_center, r);
  set_rotation(get_rotation() * ball_rotate);
}

//--------------------------------------------------------------------
free_camman::free_camman() : camman()
{
  set_move_scale(0.002);
  set_wheel_scale(0.01);
  auto wnd = glfwGetCurrentContext();
  auto size = glfw_win_size();
  // hide, fix cursor
  glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(wnd, size.x * 0.5f, size.y * 0.5f);
}

//--------------------------------------------------------------------
free_camman::~free_camman()
{
  // TODO test if glfw wnd has shut down
  auto wnd = glfwGetCurrentContext();
  if (wnd)
  {
    glfwSetInputMode(wnd, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
}

//--------------------------------------------------------------------
void free_camman::on_key(
  GLFWwindow* wnd, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (key)
    {
    case GLFW_KEY_LEFT:
      _dir |= MD_LEFT;
      _moving = true;
      break;

    case GLFW_KEY_RIGHT:
      _dir |= MD_RIGHT;
      _moving = true;
      break;

    case GLFW_KEY_UP:
      _dir |= MD_FOWARD;
      _moving = true;
      break;

    case GLFW_KEY_DOWN:
      _dir |= MD_BACK;
      _moving = true;
      break;
    }
  }
  else if (action == GLFW_RELEASE)
  {
    switch (key)
    {
    case GLFW_KEY_LEFT:
      _dir &= ~MD_LEFT;
      _moving = _dir == 0 ? false : true;
      break;
    case GLFW_KEY_RIGHT:
      _dir &= ~MD_RIGHT;
      _moving = _dir == 0 ? false : true;
      break;
    case GLFW_KEY_UP:
      _dir &= ~MD_FOWARD;
      _moving = _dir == 0 ? false : true;
      break;
    case GLFW_KEY_DOWN:
      _dir &= ~MD_BACK;
      _moving = _dir == 0 ? false : true;
      break;
    }
  }
}

//--------------------------------------------------------------------
void free_camman::on_mouse_move(GLFWwindow* wnd, double x, double y)
{
  auto p = glfw2gl(vec2(x, y));

  // fix cursor
  auto center = glfw_win_size(wnd) * 0.5f;
  glfwSetCursorPos(wnd, center.x, center.y);

  vec2 offset = p - center;
  auto v_mat =
    glm::rotate<GLfloat>(-offset.y * get_move_scale(), vec3(1, 0, 0)) *
    glm::rotate<GLfloat>(offset.x * get_move_scale(), vec3(0, 1, 0)) *
    get_v_mat();

  auto forward = vec3(-row(v_mat, 2));
  auto right = cross(forward, pza);
  if (glm::length(right) < 0.00001f)
  {
    set_v_mat(v_mat);
    return;
  }
  right = glm::normalize(right);
  vec3 fixed_up = normalize(cross(right, forward));

  // reserve eye position, use old eye position to deduce new world
  // translation, eye position is in world space, so it should be rotated
  // first, then translate
  v_mat =
    glm::translate(make_mat4_row(right, fixed_up, -forward), -eye_pos(v_mat));
  set_v_mat(v_mat);
}

//--------------------------------------------------------------------
void free_camman::update(GLfloat dt)
{
  if (!_moving) return;

  vec3 dir(0);
  if (_dir & MD_LEFT) dir.x = 1;
  if (_dir & MD_RIGHT) dir.x = -1;
  if (_dir & MD_FOWARD) dir.z = 1;
  if (_dir & MD_BACK) dir.z = -1;

  dir = normalize(dir);

  auto v_mat = get_v_mat();
  v_mat[3] += vec4(dir * dt * _move_speed, 0);
  set_v_mat(v_mat);
}

//--------------------------------------------------------------------
const mat4& free_camman::get_v_mat() const 
{
  return _v_mat;
}
//--------------------------------------------------------------------
void free_camman::set_v_mat(const mat4& v_mat) 
{
  _v_mat = v_mat;
}

//--------------------------------------------------------------------
void free_camman::lookat(const vec3& eye, const vec3& center, const vec3& up) 
{
  _v_mat = lookAt(eye, center, up);
}

}
