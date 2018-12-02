#include "mother_graph.h"

namespace zxd
{

//--------------------------------------------------------------------
GLfloat mother_graph::max_height()
{
  //return m_child ? radius() + m_child->max_height() : radius() * (1 + m_origin_scale);
  GLfloat h = radius();
  auto g = m_child;
  while(g)
  {
    h += g->radius() * (1 + g->origin_scale());
    g = g->child();
  }
  return h;
}

//--------------------------------------------------------------------
mother_graph* mother_graph::get_graph_at(int level)
{
  if(level == 0)
    return this;
  else
    return get_graph_at(--level);
}

//--------------------------------------------------------------------
void mother_graph::angular_speed(GLfloat v, bool broad_cast/* = false*/)
{ 
  m_angular_speed = v;
  if(broad_cast && m_child)
    m_child->angular_speed(v, broad_cast);
}

//--------------------------------------------------------------------
void mother_graph::radius_scale(GLfloat v, bool broad_cast/* = false*/)
{ 
  m_radius_scale = v;
  if(broad_cast && m_child)
    m_child->radius_scale(v, broad_cast);
}

//--------------------------------------------------------------------
void mother_graph::origin_scale(GLfloat v, bool broad_cast/* = false*/)
{
  m_origin_scale = v;
  if(broad_cast && m_child)
    m_child->radius_scale(v, broad_cast);
}

//--------------------------------------------------------------------
void mother_graph::remove_pen()
{
  auto pen = get_pen();
  pen->parent()->remove_child();
}

//--------------------------------------------------------------------
void mother_graph::remove_child()
{
  if(m_child)
    m_child.reset();
}

//--------------------------------------------------------------------
void mother_graph::add_child()
{
  if(m_child)
    throw std::runtime_error("only 1 graph allowed");

  auto graph = std::make_shared<mother_graph>();
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
void mother_graph::update(GLint resolution)
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
void mother_graph::update_pos()
{
  m_last_pos = m_pos;
  vec2 touch_point = m_parent->m_lisa.get_at_angle(m_angle);
  vec2 normal = m_parent->m_lisa.normal_at_angle(m_angle);
  GLfloat r = m_origin_scale * radius();
  vec2 origin = touch_point + r * normal;
  // TODO, rotate angle broke in rose, need to get actural arc length of lissa rose
  GLfloat parent_rotate_angle = m_parent->m_angle / m_parent->radius_scale();
  origin = zxd::rotate(parent_rotate_angle, origin);
  pos(m_parent->pos() + origin);
}

//--------------------------------------------------------------------
void mother_graph::reset()
{
  if(m_parent)
  {
    m_angle = fpi2;
    update_scale();
    update_pos();
    m_last_pos = pos();
  }

  m_lisa.clear();
  m_lisa.slices(1024);
  m_lisa.build_mesh();

  if(m_child)
    m_child->reset();
}

//--------------------------------------------------------------------
void mother_graph::update_scale()
{
  if(!m_parent)
    return;

  angular_speed(m_angular_scale * m_parent->angular_speed());
  radius(m_radius_scale * m_parent->radius());
}

//--------------------------------------------------------------------
mother_graph* mother_graph::get_child_at(int depth)
{
  if(depth == 0)
    return this;

  return m_child->get_child_at(--depth);
}

}

