#ifndef GLC_CAMMAN_H
#define GLC_CAMMAN_H

#include "glm.h"
#include <GLFW/glfw3.h>

namespace zxd
{

vec2 glfw2gl(const vec2& pos);
vec2 current_cursor_pos();
vec2 glfw_win_size();

class camman
{
public:

  camman();
  virtual ~camman(){}

  virtual void on_key(
    GLFWwindow* wnd, int key, int scancode, int action, int mods){};
  virtual void on_mouse_button(
    GLFWwindow* wnd, int button, int action, int mods){};
  virtual void on_mouse_move(GLFWwindow* wnd, double x, double y){};
  virtual void on_mouse_wheel(
    GLFWwindow* wnd, double xoffset, double yoffset){};
  virtual void update(GLfloat dt){};

  virtual const mat4& get_v_mat() const = 0;
  virtual void set_v_mat(const mat4& v_mat) = 0;
  virtual void lookat(const vec3& eye, const vec3& center, const vec3& up) = 0;

  GLfloat get_move_scale() const { return _move_scale; }
  void set_move_scale(GLfloat v){ _move_scale = v; }

  GLfloat get_wheel_scale() const { return _wheel_scale; }
  void set_wheel_scale(GLfloat v){ _wheel_scale = v; }

private:

  GLfloat _move_scale{0.02};
  GLfloat _wheel_scale{0.1};
};

class orbit_camman : public camman
{
public:

  orbit_camman();

  void on_mouse_button(
    GLFWwindow* wnd, int button, int action, int mods) override;
  void on_mouse_move(GLFWwindow* wnd, double x, double y) override;
  void on_mouse_wheel(
    GLFWwindow* wnd, double xoffset, double yoffset) override;

  const mat4& get_v_mat() const override;

  // v_mat = translate(0, 0, -d) * R * translate(-word_center) , there is no way
  // to deduce world_center unless d is known, this function assume d is known.
  void set_v_mat(const mat4& v_mat) override;
  void lookat(const vec3& eye, const vec3& center, const vec3& up) override;

  int get_button() const { return _button; }
  void set_button(int v){ _button = v; }

  const mat4& get_rotation() const { return _rotation; }
  void set_rotation(const mat4& v){ _rotation = v; dirty(); }

  const vec3& get_center() const { return _center; }
  void set_center(const vec3& v){ _center = v; dirty(); }

  GLfloat get_distance() const { return _distance; }
  void set_distance(GLfloat v){ _distance = v; dirty(); }

  bool get_need_button() const { return _need_button; }
  void set_need_button(bool v){ _need_button = v; }

protected:
  const vec2& get_last_cursor() const { return _last_cursor; }

private:

  virtual void perform_mouse_move(const vec2& p0, const vec2& p1){};
  virtual void perform_mouse_wheel(const vec2& offset);
  void dirty(){ _dirty = true;}

  int _button{GLFW_MOUSE_BUTTON_MIDDLE};
  mutable bool _dirty{true};
  bool _need_button{true};
  GLfloat _distance{-1};
  vec2 _last_cursor{0};
  vec3 _center{0}; // world center
  mat4 _rotation{1};
  mutable mat4 _v_mat;
};

}

#endif /* GLC_CAMMAN_H */
