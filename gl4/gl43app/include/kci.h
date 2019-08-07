#ifndef GL4_COMMON_KCI_H
#define GL4_COMMON_KCI_H

#include <vector>
#include <map>
#include <memory>
#include <functional>

#include "gl.h"
#include <glm/glm.hpp>

struct GLFWwindow;

namespace zxd
{

class kci;
using key_control_callback = std::function<void(const kci *kci)>;
using kcip = std::shared_ptr<kci>;

struct gl_cap
{
  GLenum cap;
  GLuint index;

  bool is_enabled() const
  {
    return index != -1 ? glIsEnabledi(cap, index) : glIsEnabled(cap);
  }

  void enable() { return index != -1 ? glEnablei(cap, index) : glEnable(cap); }

  void disable()
  {
    return index != -1 ? glDisablei(cap, index) : glDisable(cap);
  }

  void toggle() { return is_enabled() ? disable() : enable(); }
};

// kci_template
class kci
{
public:
  kci(GLint key, GLfloat ctrl_scale = 10, GLfloat alt_scale = 0.1);
  virtual ~kci(){};

  // any style get
  template <typename T>
  const T &get() const;

  template <typename T>
  void set(const T &value);

  template <typename T>
  const T &min_value() const;

  template <typename T>
  void min_value(const T &value);

  template <typename T>
  const T &max_value() const;

  template <typename T>
  void max_value(const T &value);

  template <typename T>
  const T &step() const;

  template <typename T>
  void step(const T &value);

  // common handy types
  GLint get_uint() const { return get<GLuint>(); }
  GLint get_int() const { return get<GLint>(); }
  GLfloat get_float() const { return get<GLfloat>(); };
  GLdouble get_double() const { return get<GLdouble>(); };

  bool get_cap_enabled() const { return get<gl_cap>().is_enabled(); }

  // index is used for vector type
  void apply_step(GLfloat step_scale, GLuint index = 0);

  key_control_callback callback() const { return m_callback; }
  void callback(key_control_callback v) { m_callback = v; }

  int key() const { return m_key; }
  void key(int v) { m_key = v; }

  GLfloat ctrl_scale() const { return m_ctrl_scale; }
  void ctrl_scale(GLfloat v) { m_ctrl_scale = v; }

  GLfloat alt_scale() const { return m_alt_scale; }
  void alt_scale(GLfloat v) { m_alt_scale = v; }

  bool get_loop() const { return m_loop; }
  void set_loop(bool v) { m_loop = v; }

private:
  virtual void do_apply_step(GLfloat step_scale, GLuint index) = 0;

  bool m_loop;
  int m_key; // the same key as glfw_key
  GLfloat m_ctrl_scale;
  GLfloat m_alt_scale;
  key_control_callback m_callback;
};

template <typename T>
class kci_template : public kci
{
public:
  kci_template(int key, const T &value, const T &min_value, const T &max_value,
               const T &step);

  const T &get() const { return m_value; }
  void set(const T &value) { m_value = value; }

  void do_apply_step(GLfloat step_scale, GLuint index) override;

  const T &value() const { return m_value; }
  void value(const T &v) { m_value = v; }

  const T &min_value() const { return m_min_value; }
  void min_value(const T &v) { m_min_value = v; }

  const T &max_value() const { return m_max_value; }
  void max_value(const T &v) { m_max_value = v; }

  const T &step() const { return m_step; }
  void step(const T &v) { m_step = v; }

private:
  T m_value;
  T m_min_value;
  T m_max_value;
  T m_step;
};

struct kci_t_primitive_tag {};
struct kci_t_vec_tag {};
struct kci_t_bool_tag {};
struct kci_t_cap_tag {};

template <typename T>
struct kci_type_traits
{
  using tag = kci_t_primitive_tag;
};

template <>
struct kci_type_traits<bool>
{
  using tag = kci_t_bool_tag;
};

template <>
struct kci_type_traits<gl_cap>
{
  using tag = kci_t_cap_tag;
};

template<glm::length_t L, typename T, glm::qualifier Q>
struct kci_type_traits<glm::vec<L, T, Q>>
{
  using tag = kci_t_vec_tag;
};

template <typename T>
void apply_step_imp(kci_template<T>& kci, GLfloat step_scale, GLint index,
                    kci_t_primitive_tag)
{
  T value = kci.value() + step_scale * kci.step() ;
  if (!kci.get_loop())
    value = glm::clamp<T>(value, kci.min_value(), kci.max_value());
  else
  {
    if (value > kci.max_value())
      value = kci.min_value();
    if (value < kci.min_value())
      value = kci.max_value();
  }
  kci.value(value);
}

template <typename T>
void apply_step_imp(kci_template<T>& kci, GLfloat step_scale, GLint index,
                    kci_t_bool_tag)
{
  kci.value(!kci.value());
}

template <typename T>
void apply_step_imp(kci_template<T>& kci, GLfloat step_scale, GLint index,
                    kci_t_cap_tag)
{
  auto value = kci.value();
  value.toggle();
}

template <typename T>
void apply_step_imp(kci_template<T>& kci, GLfloat step_scale, GLint index,
                    kci_t_vec_tag)
{
  // no throw
  if (index > T::length())
    return;

  T value = kci.value();
  value[index] = glm::clamp(value[index] + kci.step()[index] * step_scale,
                            kci.min_value[index], kci.max_value[index]);
  kci.value(value);
};

template <typename T>
const T &kci::get() const
{
  return dynamic_cast<const kci_template<T> &>(*this).get();
}

template <typename T>
void kci::set(const T &value)
{
  dynamic_cast<kci_template<T> &>(*this).set(value);
}

template <typename T>
const T &kci::min_value() const
{
  return dynamic_cast<const kci_template<T> &>(*this).min_value();
}

template <typename T>
void kci::min_value(const T &value)
{
  dynamic_cast<kci_template<T> &>(*this).min_value(value);
}

template <typename T>
const T &kci::max_value() const
{
  return dynamic_cast<const kci_template<T> &>(*this).max_value();
}

template <typename T>
void kci::max_value(const T &value)
{
  dynamic_cast<kci_template<T> &>(*this).max_value(value);
}

template <typename T>
const T &kci::step() const
{
  return dynamic_cast<const kci_template<T> &>(*this).step();
}

template <typename T>
void kci::step(const T &value)
{
  dynamic_cast<kci_template<T> &>(*this).step(value);
}

//--------------------------------------------------------------------
template <typename T>
kci_template<T>::kci_template(int key, const T& value, const T& min_value,
                              const T& max_value, const T& step)
    : kci(key), m_value(value), m_min_value(min_value), m_max_value(max_value),
      m_step(step)
{
}

//--------------------------------------------------------------------
template <typename T>
void kci_template<T>::do_apply_step(GLfloat step_scale, GLuint index)
{
  apply_step_imp(*this, step_scale, index, typename kci_type_traits<T>::tag());
}

typedef std::map<int, std::shared_ptr<kci>> kci_map;

class key_control
{
  friend class app;

public:
  template <typename T>
  std::shared_ptr<kci_template<T>>
  add(int key, const T &init_value, const T &min_value, const T &max_value,
      const T &step, key_control_callback callback = nullptr,
      GLfloat ctrl_scale = 0.1f, GLfloat alt_scale = 10.0f);

  std::shared_ptr<kci_template<bool>>
  add_bool(int key, bool init_value, key_control_callback callback = nullptr);

  std::shared_ptr<kci_template<gl_cap>>
  add_cap_ctl(int key, GLenum cap, GLuint index = -1,
              key_control_callback callback = nullptr);

  GLuint index() const { return m_index; }
  void index(GLuint v) { m_index = v; }

private:
  void handle(GLFWwindow *wnd, int key, int scancode, int action, int mods);

  GLuint m_index = 0; // index for vec type
  kci_map m_key_map;
};

//--------------------------------------------------------------------
template <typename T>
std::shared_ptr<kci_template<T>>
key_control::add(int key, const T& init_value, const T& min_value,
                 const T& max_value, const T& step,
                 key_control_callback callback /* = nullptr*/,
                 GLfloat ctrl_scale /* = 0.1f*/, GLfloat alt_scale /* = 10.0f*/)
{
  auto item = std::make_shared<kci_template<T>>(key, init_value, min_value,
                                                max_value, step);
  item->ctrl_scale(ctrl_scale);
  item->alt_scale(alt_scale);
  item->callback(callback);
  m_key_map[key] = item;
  return item;
}
}
#endif /* GL4_COMMON_KCI_H */
