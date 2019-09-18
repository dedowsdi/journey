#ifndef GL4_COMMON_KCI_H
#define GL4_COMMON_KCI_H

#include <vector>
#include <map>
#include <memory>
#include <functional>

#include <gl.h>
#include <glm/glm.hpp>

struct GLFWwindow;

namespace zxd
{

class kci;
using key_control_callback = std::function<void(const kci* kci)>;
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

template <typename T>
class enum_type
{
public:
  using size_type = typename std::vector<T>::size_type;

  enum_type(std::initializer_list<T> l, size_type index = 0):
    m_values(l),
    m_index(index)
  {
  }

  const T& get() const { return m_values.at(m_index); }
  void next(GLint offset)
  {
    GLint index = static_cast<GLint>(m_index) + offset;
    if (index >= 0)
      m_index = index % m_values.size();
    else
      m_index = m_values.size() - abs(index) % m_values.size();
  }

private:
  size_type m_index;
  std::vector<T> m_values;
};

// kci_template
class kci
{
public:
  kci(GLint key, GLfloat ctrl_scale = 10, GLfloat alt_scale = 0.1);
  virtual ~kci(){};

  // any style get
  template <typename T>
  const T& get() const;

  template <typename T>
  void set(const T &value);

  // common handy types
  GLint get_uint() const { return get<GLuint>(); }
  GLint get_int() const { return get<GLint>(); }
  GLfloat get_float() const { return get<GLfloat>(); };
  GLdouble get_double() const { return get<GLdouble>(); };
  GLint get_bool() const { return get<bool>(); }

  bool get_cap() const { return get<gl_cap>().is_enabled(); }

  GLint get_int_enum() const { return get<enum_type<GLint>>().get(); }
  GLint get_float_enum() const { return get<enum_type<GLfloat>>().get(); }

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
  kci_template(int key, const T& value, const T& min_value, const T& max_value,
               const T& step);

  const T& get() const { return m_value; }
  void set(const T& value) { m_value = value; }

  void do_apply_step(GLfloat step_scale, GLuint index) override;

  const T& min_value() const { return m_min_value; }
  void min_value(const T& v) { m_min_value = v; }

  const T& max_value() const { return m_max_value; }
  void max_value(const T& v) { m_max_value = v; }

  const T& step() const { return m_step; }
  void step(const T& v) { m_step = v; }

private:
  T m_value;
  T m_min_value;
  T m_max_value;
  T m_step;
};

template <>
class kci_template<gl_cap> : public kci
{
public:
  kci_template(int key, GLenum cap, GLuint index = -1);

  const gl_cap& get() const { return m_gl_cap; }

  void do_apply_step(GLfloat step_scale, GLuint index) override;

private:
  gl_cap m_gl_cap;
};

template <typename T>
class kci_template<enum_type<T>> : public kci
{
public:
  kci_template(int key, std::initializer_list<T> l, GLuint index = 0);

  const enum_type<T>& get() const { return m_value; }

  void do_apply_step(GLfloat step_scale, GLuint index) override;

private:
  enum_type<T> m_value;
};

struct kci_t_primitive_tag {};
struct kci_t_vec_tag {};
struct kci_t_bool_tag {};

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


template <glm::length_t L, typename T, glm::qualifier Q>
struct kci_type_traits<glm::vec<L, T, Q>>
{
  using tag = kci_t_vec_tag;
};

template <typename T>
void apply_step_imp(kci_template<T>& kci, GLfloat step_scale, GLint index,
                    kci_t_primitive_tag)
{
  T value = kci.get() + step_scale * kci.step();
  if (!kci.get_loop())
    value = glm::clamp<T>(value, kci.min_value(), kci.max_value());
  else
  {
    if (value > kci.max_value())
      value = kci.min_value();
    if (value < kci.min_value())
      value = kci.max_value();
  }
  kci.set(value);
}

template <typename T>
void apply_step_imp(kci_template<T>& kci, GLfloat step_scale, GLint index,
                    kci_t_bool_tag)
{
  kci.set(!kci.get());
}

template <typename T>
void apply_step_imp(kci_template<T>& kci, GLfloat step_scale, GLint index,
                    kci_t_vec_tag)
{
  // no throw
  if (index > T::length() || index < 0)
    return;

  auto value = kci.get();
  value[index] = glm::clamp(value[index] + kci.step()[index] * step_scale,
                            kci.min_value()[index], kci.max_value()[index]);
  kci.set(value);
};

template <typename T>
const T& kci::get() const
{
  return dynamic_cast<const kci_template<T>&>(*this).get();
}

template <typename T>
void kci::set(const T &value)
{
  dynamic_cast<kci_template<T> &>(*this).set(value);
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

template <typename T>
kci_template<enum_type<T>>::kci_template(int key, std::initializer_list<T> l,
                                         GLuint index)
    : kci(key), m_value(l, index)
{
}

template <typename T>
void kci_template<enum_type<T>>::do_apply_step(GLfloat step_scale, GLuint index)
{
  m_value.next(step_scale);
}

class key_control
{
  friend class app;

public:
  template <typename T>
  std::shared_ptr<kci_template<T>>
  add(int key, const T& init_value, const T& min_value, const T& max_value,
      const T& step, key_control_callback callback = nullptr,
      GLfloat ctrl_scale = 0.1f, GLfloat alt_scale = 10.0f);

  std::shared_ptr<kci_template<bool>>
  add_bool(int key, bool init_value, key_control_callback callback = nullptr);

  std::shared_ptr<kci_template<gl_cap>>
  add_cap(int key, GLenum cap, GLuint index = -1,
          key_control_callback callback = nullptr);

  template <typename T>
  std::shared_ptr<kci_template<enum_type<T>>>
  add_enum(int key, std::initializer_list<T> l, int index = 0,
          key_control_callback callback = nullptr);

  std::shared_ptr<kci_template<GLfloat>> add_normalized(
    int key, GLfloat init_value = 0, key_control_callback callback = nullptr);

  GLint index() const { return m_index; }
  void index(GLint v) { m_index = glm::max(0, v); }

private:
  void handle(GLFWwindow* wnd, int key, int scancode, int action, int mods);

  GLint m_index = 0; // index for vec type
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

//--------------------------------------------------------------------
template <typename T>
std::shared_ptr<kci_template<enum_type<T>>>
key_control::add_enum(int key, std::initializer_list<T> l, int index,
                      key_control_callback callback)
{
  auto item = std::make_shared<kci_template<enum_type<T>>>(key, l, index);
  item->callback(callback);
  m_key_map[key] = item;
  return item;
}
}
#endif /* GL4_COMMON_KCI_H */
