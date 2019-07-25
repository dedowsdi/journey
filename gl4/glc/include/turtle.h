#ifndef GL4_GLC_TURTLE_H
#define GL4_GLC_TURTLE_H

#include <vector>
#include <string>
#include <map>
#include <memory>

#include "gl.h"
#include "mat_stack.h"
#include "glm.h"


namespace zxd
{

using namespace glm;

class turtle;

class turtle_operation
{
public:
  virtual ~turtle_operation() = default;
  virtual void operator()(turtle* t) const = 0;
};

class translate_operation: public turtle_operation
{
public:
  translate_operation(const vec3& translation);

  void operator()(turtle* t) const override;

  const vec3& translation() const { return m_translation; }
  void translation(const vec3& v){ m_translation = v; }

private:
  vec3 m_translation;
};

class translate_only_operation: public turtle_operation
{
public:
  translate_only_operation(const vec3& translation);

  void operator()(turtle* t) const override;

  const vec3& translation() const { return m_translation; }
  void translation(const vec3& v){ m_translation = v; }

private:
  vec3 m_translation;
};

class rotate_operation: public turtle_operation
{
public:
  rotate_operation(GLfloat angle, const vec3& axis);

  void operator()(turtle* t) const override;

  GLfloat angle() const { return m_angle; }
  void angle(GLfloat v){ m_angle = v; }

  const vec3& axis() const { return m_axis; }
  void axis(const vec3& v){ m_axis = v; }

private:
  GLfloat m_angle;
  vec3 m_axis;
};

class push_operation: public turtle_operation
{
public:
  void operator()(turtle* t) const override;
};

class pop_operation: public turtle_operation
{
public:
  void operator()(turtle* t) const override;
};

using spto = std::shared_ptr<turtle_operation>;

// lsystem turtle, not logo
class turtle
{
public:

  using operation_map =  std::map<char, spto>;

  std::vector<vec3> generate(const std::string& pattern, const mat4& start_transform);

  void translate(const vec3& v);
  void translate_only(const vec3& v);
  void rotate(GLfloat angle, const vec3& axis);

  void load_transform(const mat4& m);
  const mat4& get_transform() const;

  void push();
  void pop();

  void register_operation(char c, const spto& operation);

private:
  const turtle_operation* get_operation(char c) const;

  mat_stack m_transform;
  operation_map m_operations;
  // temporary vertices, will be moved into result in generate
  std::vector<vec3> m_vertices;
};

}

#endif /* GL4_GLC_TURTLE_H */
