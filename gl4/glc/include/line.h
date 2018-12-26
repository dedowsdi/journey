#ifndef GL_GLC_LINE_H
#define GL_GLC_LINE_H
#include "geometry.h"

namespace zxd 
{

class line : public geometry_base
{

public:
  using geometry_base::build_mesh; // avoid warning

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
  clear_primitive_sets();
  using value_type = typename std::iterator_traits<It>::value_type;
  auto vertices = std::make_shared<template_array<value_type>>();
  vertices->assign(beg, end);
  attrib_array(0, vertices);
  m_mode = mode;
  create_primitive_set();
}

}


#endif /* GL_GLC_LINE_H */
