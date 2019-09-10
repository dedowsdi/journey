#ifndef GL_GLC_LINE_H
#define GL_GLC_LINE_H
#include "geometry.h"

namespace zxd 
{

class line : public geometry_base
{

public:

  template <typename It>
  void build_line(GLenum mode, It beg, It end);

protected:
  void create_primitive_set();

private:
  GLenum m_mode;
};


//--------------------------------------------------------------------
template <typename It>
void line::build_line(GLenum mode, It beg, It end)
{
  set_vao(std::make_unique<vao>());
  clear_primitive_sets();
  using value_type = typename std::iterator_traits<It>::value_type;
  auto vertices = std::make_unique<template_array<value_type>>();
  vertices->assign(beg, end);
  m_mode = mode;
  add_array_attrib(get_vao(), 0, std::move(vertices));
  create_primitive_set();
}

}


#endif /* GL_GLC_LINE_H */
