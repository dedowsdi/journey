#ifndef GL4_COMMON_PARAM_H
#define GL4_COMMON_PARAM_H
#include "gl.h"
#include <functional>
#include <vector>
#include <map>

class GLFWwindow;

namespace zxd
{

struct key_control_item;
typedef std::function<void(const key_control_item* kci)> key_control_callback;

struct key_control_item
{
  int key; // the same key as glfw_key
  GLfloat value;
  GLfloat min_value;
  GLfloat max_value;
  GLfloat step;
  GLfloat ctrl_scale = 0.1;
  GLfloat alt_scale = 10;
  key_control_callback callback;

  //key_control_item(int _key, GLfloat step, key_control_callback _callback);
};

typedef std::map<int, key_control_item> kci_map;

class key_control
{
  friend class app;

protected:
  kci_map m_key_map;

public:
  key_control_item* add_control(int key, GLfloat init_value, GLfloat min_value, GLfloat
      max_value, GLfloat step, key_control_callback callback = nullptr, GLfloat
      ctrl_scale = 0.1, GLfloat alt_scale = 10);

protected:
  void handle(GLFWwindow *wnd, int key, int scancode, int action, int mods);

};

}



#endif /* GL4_COMMON_PARAM_H */
