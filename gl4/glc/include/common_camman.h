#ifndef GL4_GLC_COMMON_CAMMAN_H
#define GL4_GLC_COMMON_CAMMAN_H

#include "camman.h"

namespace zxd
{

// pitch-yaw style
class blend_camman : public orbit_camman
{
public:
  blend_camman();

  void on_key(
    GLFWwindow* wnd, int key, int scancode, int action, int mods) override;

private:
  void perform_mouse_move(
    GLFWwindow* wnd, const vec2& p0, const vec2& p1) override;
};

class trackball_camman : public orbit_camman
{
public:
  trackball_camman();

private:
  void perform_mouse_move(
    GLFWwindow* wnd, const vec2& p0, const vec2& p1) override;

  GLfloat _ball_radius; // normalized
  vec2 _ball_center;

};

class free_camman : public camman
{
public:
  free_camman();
  ~free_camman();

  enum move_dir {
    MD_LEFT = 1 << 0,
    MD_RIGHT = 1 << 1,
    MD_FOWARD = 1 << 2,
    MD_BACK = 1 << 3,
  };

  void on_key(
    GLFWwindow* wnd, int key, int scancode, int action, int mods) override;

  void on_mouse_move(GLFWwindow* wnd, double x, double y) override;

  void update(GLfloat dt) override;

  const mat4& get_v_mat() const override;

  void set_v_mat(const mat4& v_mat) override;

  void lookat(const vec3& eye, const vec3& center, const vec3& up) override;

private:
  bool _moving{false};
  GLuint _dir{0};
  GLfloat _move_speed{1};
  mat4 _v_mat{1};
};
}

#endif /* GL4_GLC_COMMON_CAMMAN_H */
