#include "kci.h"
#include <GLFW/glfw3.h>
#include "glm.h"

namespace zxd
{

//--------------------------------------------------------------------
kci::kci(GLint key, GLfloat ctrl_scale, GLfloat alt_scale)
    : m_loop(false), m_key(key), m_ctrl_scale(ctrl_scale),
      m_alt_scale(alt_scale), m_callback(nullptr)
{
}

//--------------------------------------------------------------------
void kci::apply_step(GLfloat step_scale, GLuint index /* = 0*/)
{
  do_apply_step(step_scale, index);

  if (m_callback)
    m_callback(this);
}

//--------------------------------------------------------------------
void key_control::handle(GLFWwindow *wnd, int key, int scancode, int action,
                         int mods)
{
  if (action != GLFW_PRESS)
    return;

  auto iter = m_key_map.find(key);
  if (iter == m_key_map.end())
    return;

  auto item = iter->second;

  auto scale = 1.0f;
  if (mods & GLFW_MOD_CONTROL)
    scale *= item->ctrl_scale();

  if (mods & GLFW_MOD_ALT)
    scale *= item->alt_scale();

  if (mods & GLFW_MOD_SHIFT)
    scale *= -1;

  item->apply_step(scale, m_index);
}

//--------------------------------------------------------------------
std::shared_ptr<kci_template<bool>>
key_control::add_bool(int key, bool init_value, key_control_callback callback)
{
  auto item =
    std::make_shared<kci_template<bool>>(key, init_value, false, true, true);
  item->callback(callback);
  m_key_map[key] = item;
  return item;
}

//--------------------------------------------------------------------
std::shared_ptr<kci_template<gl_cap>>
key_control::add_cap_ctl(int key, GLenum cap, GLuint index,
                         key_control_callback callback)
{
  auto ctl = gl_cap{cap, index};
  auto item =
    std::make_shared<kci_template<gl_cap>>(key, ctl, ctl, ctl, ctl);
  item->callback(callback);
  m_key_map[key] = item;
  return item;
}
}
