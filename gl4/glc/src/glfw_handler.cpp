#include "glfw_handler.h"

namespace zxd
{

//--------------------------------------------------------------------
vec2 glfw2gl(const vec2& pos, GLFWwindow* wnd)
{
  auto size = glfw_win_size(wnd);
  return vec2(pos.x, size.y - 1 - pos.y);
}

//--------------------------------------------------------------------
vec2 current_cursor_pos(GLFWwindow* wnd)
{
  if (!wnd)
    wnd = glfwGetCurrentContext();
  dvec2 pos;
  glfwGetCursorPos(wnd, &pos.x, &pos.y);
  return glfw2gl(pos);
}

//--------------------------------------------------------------------
vec2 glfw_win_size(GLFWwindow* wnd)
{
  ivec2 size;
  if (!wnd)
    wnd = glfwGetCurrentContext();
  glfwGetWindowSize(wnd, &size.x, &size.y);
  return size;
}

//--------------------------------------------------------------------
GLfloat glfw_min_wh(GLFWwindow* wnd)
{
  auto size = glfw_win_size(wnd);
  return std::min(size.x, size.y);
}

//--------------------------------------------------------------------
void glfw_gl_handler::on_mouse_button(
  GLFWwindow* wnd, int button, int action, int mods)
{
  _cursor0 = current_cursor_pos(wnd);
}

//--------------------------------------------------------------------
void glfw_gl_handler::on_mouse_move(GLFWwindow* wnd, double x, double y)
{
  auto cpos = glfw2gl(vec2(x, y));
  perform_mouse_move(wnd, _cursor0, cpos);
  _cursor0 = cpos;
}

//--------------------------------------------------------------------
void glfw_gl_handler::on_mouse_wheel(
  GLFWwindow* wnd, double xoffset, double yoffset) 
{
  perform_mouse_wheel(wnd, vec2(xoffset, yoffset));
}

}
