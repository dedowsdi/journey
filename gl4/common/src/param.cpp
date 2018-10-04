#include "param.h"
#include <GLFW/glfw3.h>
#include "glm.h"

namespace zxd
{

//--------------------------------------------------------------------
key_control_item* key_control::add_control(int key, GLfloat init_value, GLfloat min_value,
    GLfloat max_value,GLfloat step, key_control_callback callback/* = nullptr*/,
    GLfloat ctrl_scale/* = 0.1*/, GLfloat alt_scale/* = 10*/)
{
  key_control_item item;
  item.key = key;
  item.value = init_value;
  item.min_value = min_value;
  item.max_value = max_value;
  item.step = step;
  item.callback = callback;
  item.ctrl_scale = ctrl_scale;
  item.alt_scale = alt_scale;

  m_key_map[key] = item;

  return &m_key_map[key];
}

//--------------------------------------------------------------------
void key_control::handle(GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
  if(action != GLFW_PRESS)
    return;

  auto iter = m_key_map.find(key);
  if(iter == m_key_map.end())
    return;

  key_control_item& item = iter->second;

  GLfloat change = item.step;
  if(mods & GLFW_MOD_CONTROL)
    change *= item.ctrl_scale;

  if(mods & GLFW_MOD_ALT)
    change *= item.alt_scale;

  if(mods & GLFW_MOD_SHIFT)
    change *= -1;

  item.value = glm::clamp(item.value + change, item.min_value, item.max_value);
  
  if(item.callback)
    item.callback(&item);

}
}
