#include "fractal_spiral.h"

namespace zxd
{

//--------------------------------------------------------------------
GLfloat spiral_seed::approximate_height(GLuint times)
{
  GLfloat original_angle = m_child->angle();
  auto pen = get_pen();

  GLfloat step_angle = f2pi / times;
  GLfloat l2 = 0;
  for (int i = 0; i < times; ++i)
  {
    reset(step_angle * i);
    GLfloat l = glm::length2(pen->pos());
    if(l > l2)
      l2 = l;
  }

  reset(original_angle);

  return sqrt(l2);
}

//--------------------------------------------------------------------
spiral_seed* spiral_seed::get_graph_at(int level)
{
  if(level == 0)
    return this;
  else
    return get_graph_at(--level);
}

//--------------------------------------------------------------------
void spiral_seed::angular_speed(GLfloat v, bool broad_cast/* = false*/)
{ 
  m_angular_speed = v;
  if(broad_cast && m_child)
    m_child->angular_speed(v, broad_cast);
}

//--------------------------------------------------------------------
void spiral_seed::radius_scale(GLfloat v, bool broad_cast/* = false*/)
{ 
  m_radius_scale = v;
  if(broad_cast && m_child)
    m_child->radius_scale(v, broad_cast);
}

//--------------------------------------------------------------------
void spiral_seed::origin_scale(GLfloat v, bool broad_cast/* = false*/)
{
  m_origin_scale = v;
  if(broad_cast && m_child)
    m_child->radius_scale(v, broad_cast);
}

//--------------------------------------------------------------------
void spiral_seed::remove_pen()
{
  auto pen = get_pen();
  pen->parent()->remove_child();
}

//--------------------------------------------------------------------
void spiral_seed::remove_child()
{
  if(m_child)
    m_child.reset();
}

//--------------------------------------------------------------------
void spiral_seed::add_child()
{
  if(m_child)
    throw std::runtime_error("only 1 graph allowed");

  auto graph = std::make_shared<spiral_seed>();
  child(graph);
  graph->parent(this);
  graph->angular_scale(m_angular_scale);
  graph->radius_scale(m_radius_scale);
  graph->origin_scale(m_origin_scale);
  graph->rose_n(m_lisa.rose_n());
  graph->rose_d(m_lisa.rose_d());
  graph->rose_offset(m_lisa.rose_offset());
  graph->xscale(m_lisa.xscale());
  graph->yscale(m_lisa.yscale());
  graph->type(m_lisa.type());
  graph->update_scale();
  graph->update_pos();
  graph->last_pos(graph->pos());
}

//--------------------------------------------------------------------
void spiral_seed::update(GLint resolution)
{
  if(m_parent)
  {
    m_angle += m_angular_speed / resolution;
    update_pos();
  }

  if(m_child)
    m_child->update(resolution);
}

//--------------------------------------------------------------------
void spiral_seed::update_pos()
{
  m_rotate_angle = m_parent->rotate_angle() + m_angle / radius_scale();
  m_last_pos = m_pos;
  vec2 touch_point = m_parent->m_lisa.get_at_angle(m_angle);
  vec2 normal = m_parent->m_lisa.normal_at_angle(m_angle);
  GLfloat r = m_origin_scale * radius();
  vec2 origin = touch_point + r * normal;
  // TODO, rotate angle broke in rose, need to get actural arc length of lissa rose
  origin = zxd::rotate(m_parent->rotate_angle(), origin);
  pos(m_parent->pos() + origin);
}

//--------------------------------------------------------------------
void spiral_seed::reset(GLfloat angle)
{
  if(m_parent)
  {
    m_angle = angle;
    update_scale();
    update_pos();
    m_last_pos = pos();
  }
  else
  {
    m_angle = m_rotate_angle = 0;
  }

  m_lisa.clear();
  m_lisa.slices(1024);
  m_lisa.build_mesh();

  if(m_child)
    m_child->reset(angle);
}

//--------------------------------------------------------------------
void spiral_seed::update_scale()
{
  if(!m_parent)
    return;

  angular_speed(m_angular_scale * m_parent->angular_speed());
  radius(m_radius_scale * m_parent->radius());
}

//--------------------------------------------------------------------
spiral_seed* spiral_seed::get_child_at(int depth)
{
  if(depth == 0)
    return this;

  return m_child->get_child_at(--depth);
}

}
