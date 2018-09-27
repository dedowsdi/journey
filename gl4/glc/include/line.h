#ifndef LINE_H
#define LINE_H
#include "geometry.h"

namespace zxd 
{

class line : public geometry_base
{
  
public:
  using geometry_base::build_mesh; // avoid warning

  GLenum m_mode;

  line(GLenum mode);

  void build_mesh(const vec3& v0, const vec3& v1, const vec4& color);
  void build_mesh(const vec3& v0, const vec3& v1);
  void build_mesh(const vec2& v0, const vec2& v1, const vec4& color);
  void build_mesh(const vec2& v0, const vec2& v1);

  void draw(GLuint primcount = -1);

};

}


#endif /* LINE_H */
