#include <app.h>

#include <sstream>
#include <cuboid.h>
#include <axes.h>
#include <algorithm>
#include <common_program.h>

namespace zxd {

const auto width = 800;
const auto height = 800;
mat4 p_mat;
mat4 v_mat;

std::shared_ptr<kci> target;
std::shared_ptr<kci> radius;
std::array<std::string, 3> targets{"scene", "cube0", "cube1"};
vec2 mouse_p0;

struct trackball
{
  GLfloat radius = 0.5 * width;
  vec2 center;
  mat4* target_mat;
  mat4 target_to_view_mat;
};

trackball ball;

struct node
{
  std::shared_ptr<geometry_base> drawable;
  mat4 transform;
};

node cube0, cube1, axes0;

lightless_program prg;

class app_trackball : public app
{

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(GLFWwindow *wnd, int key, int scancode, int action,
                int mods) override;

  void glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                         int mods) override;

  void glfw_mouse_move(GLFWwindow *wnd, double x, double y) override;
};

void app_trackball::init_info()
{
  app::init_info();
  m_info.title = "app_trackball :";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

void updateTrackball(const kci* p)
{
  auto itarget = p->get_int_enum();
  if (itarget == 0)
  {
    ball.center = vec2(width, height) * 0.5f;
    ball.target_mat = &v_mat;
    ball.target_to_view_mat = mat4(1);
  }
  else
  {
    auto& m_mat = itarget == 1 ? cube0.transform : cube1.transform;
    auto mvpw =
      compute_window_mat(0, 0, width, height) * p_mat * v_mat * m_mat;
    auto pos = mvpw[3];
    ball.center = vec2(pos / pos.w);
    ball.target_mat = &m_mat;
    ball.target_to_view_mat = v_mat;
  }
}

void app_trackball::create_scene()
{
  glEnable(GL_CULL_FACE);
  p_mat = perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
  v_mat = lookAt(vec3(0, -16, 0), vec3(0), vec3(0, 0, 1));

  auto cube_model = std::make_shared<cuboid>(2, cuboid::type::CT_24);
  cube_model->include_color(true);
  cube_model->build_mesh();

  cube0.drawable = cube_model;
  cube0.transform = translate(vec3(3, 0, 0));

  cube1.drawable = cube_model;
  cube1.transform = translate(vec3(-3, 0, 0));

  auto axes_model = std::make_shared<axes>();
  axes_model->include_color(true);
  axes_model->build_mesh();

  axes0.drawable = axes_model;

  prg.with_color = true;
  prg.init();

  target = m_control.add_enum('Q', {0, 1, 2}, 0, updateTrackball);
  radius = m_control.add_normalized('W', 0.5, [](const auto* kci) -> void {
    ball.radius = width * 0.5f * kci->get_float();
  });
  updateTrackball(target.get());
}

void app_trackball::update() {}

void app_trackball::display()
{
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  prg.use();

  auto vp_mat = p_mat * v_mat;

  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(vp_mat * cube0.transform));
  cube0.drawable->draw();

  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(vp_mat * cube1.transform));
  cube1.drawable->draw();

  glDisable(GL_DEPTH_TEST);
  auto itarget = target->get_int_enum();
  auto axes_transform =
    itarget == 0 ? mat4(1) : itarget == 1 ? cube0.transform : cube1.transform;
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(vp_mat * axes_transform));
  axes0.drawable->draw();
  glEnable(GL_DEPTH_TEST);

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "q : target : " << targets[itarget] << std::endl;
  ss << "w : radius : " << radius->get_float() << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);

  glDisable(GL_BLEND);
}

void app_trackball::glfw_resize(GLFWwindow *wnd, int w, int h)
{
  app::glfw_resize(wnd, w, h);
  m_text.reshape(m_info.wnd_width, m_info.wnd_height);
}

void app_trackball::glfw_key(GLFWwindow *wnd, int key, int scancode, int action,
                      int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (key)
    {
    default:
      break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void app_trackball::glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                               int mods)
{
  app::glfw_mouse_button(wnd, button, action, mods);
  if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS)
  {
    mouse_p0 = current_mouse_position();
  }
}

// https://www.khronos.org/opengl/wiki/Object_Mouse_Trackball
vec3 wnd2ball(const vec2& p, const vec2& center, GLfloat radius)
{
  auto pos = p - center;
  auto l2 = length2(pos);
  auto r2 = radius * radius;
  if (l2 <= r2 * 0.5f)
    return vec3(pos, sqrt(r2 - l2));
  else
    return vec3(pos, r2 * 0.5f / sqrt(l2));
}

void app_trackball::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  if (glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS)
    return;

  auto p1 = current_mouse_position();

  auto sp0 = wnd2ball(mouse_p0, ball.center, ball.radius);
  auto sp1 = wnd2ball(p1, ball.center, ball.radius);
  auto ball_mat = rotate_to(normalize(sp0), normalize(sp1));
  mouse_p0 = p1;

  auto itarget = target->get_int_enum();

  // ball_rot works on view space(no translation), to apply it on other space,
  // we must transform to view space, apply rotation, then transform back. If
  // you want to apply it to the entire scene, use v_mat as target_mat, 
  // mat4(1) as target_to_vew_mat.
  auto to_v_mat = ball.target_to_view_mat * *ball.target_mat;
  to_v_mat[3] = vec4(0, 0, 0, 1);
  *ball.target_mat *= inverse(to_v_mat) * ball_mat * to_v_mat;
}

}

int main(int argc, char *argv[])
{
  zxd::app_trackball app;
  app.run();
}
