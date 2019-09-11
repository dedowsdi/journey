#ifndef GL4_GLC_GLFW_HANDLER_H
#define GL4_GLC_GLFW_HANDLER_H

#include <glm.h>
#include <GLFW/glfw3.h>

namespace zxd
{

glm::vec2 glfw2gl(const glm::vec2& pos, GLFWwindow* wnd = 0);
glm::vec2 current_cursor_pos(GLFWwindow* wnd = 0);
glm::vec2 glfw_win_size(GLFWwindow* wnd = 0);
GLfloat glfw_min_wh(GLFWwindow* wnd = 0);

class glfw_handler
{
public:
  virtual ~glfw_handler() = default;

  virtual void on_key(
    GLFWwindow* wnd, int key, int scancode, int action, int mods){};

  virtual void on_mouse_button(
    GLFWwindow* wnd, int button, int action, int mods){};

  virtual void on_mouse_move(GLFWwindow* wnd, double x, double y){};

  virtual void on_mouse_wheel(
    GLFWwindow* wnd, double xoffset, double yoffset){};

  virtual void update(GLfloat dt){};
};

class glfw_gl_handler : public glfw_handler
{
public:
  void on_mouse_button(
    GLFWwindow* wnd, int button, int action, int mods) override;

  void on_mouse_move(GLFWwindow* wnd, double x, double y) override;

  void on_mouse_wheel(
    GLFWwindow* wnd, double xoffset, double yoffset) override;

  const vec2& get_cursor0() const { return _cursor0; }

private:

  virtual void perform_mouse_move(
    GLFWwindow* wnd, const vec2& p0, const vec2& p1){};
  virtual void perform_mouse_wheel(GLFWwindow* wnd, const vec2& offset){};

  vec2 _cursor0 ;// same as p0 in perform_mouse_move
};

}

#endif /* GL4_GLC_GLFW_HANDLER_H */
