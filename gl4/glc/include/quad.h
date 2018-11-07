#ifndef QUAD_H
#define QUAD_H
#include "program.h"
#include "glm.h"
#include <array>
#include "common.h"
#include "geometry.h"

namespace zxd {

// draw single 2d quad
void draw_quad(GLuint tex, GLuint tui = 0);

class quad : public geometry_base {
protected:
  // cc order, starts from lb
  vec3 m_v0;
  vec3 m_v1;
  vec3 m_v2;
  vec3 m_v3;
public:
  //quad(GLfloat size = 2) : m_width(size), m_height(size) {}
  quad(const vec3& corner = vec3(-1,-1,0),
      const vec3& edge0_vec = vec3(2, 0, 0),
      const vec3& edge1_vec = vec3(0, 2, 0));

  void setup(const vec3& corner, const vec3& edge0_vec, const vec3& edge1_vec);
  void setup(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1);

  void build_vertex();
  void build_normal();
  void build_texcoord();
  void build_tangent();

  const vec3& v0() const { return m_v0; }
  void v0(const vec3& v){ m_v0 = v; }

  const vec3& v1() const { return m_v1; }
  void v1(const vec3& v){ m_v1 = v; }

  const vec3& v2() const { return m_v2; }
  void v2(const vec3& v){ m_v2 = v; }

  const vec3& v3() const { return m_v3; }
  void v3(const vec3& v){ m_v3 = v; }

  GLfloat left();
  GLfloat right();
  GLfloat bottom();
  GLfloat top();
};

class billboard_quad : public geometry_base
{
protected:
  vec2 m_v0 = vec2(-5);
  vec2 m_v1 = vec2(5);
  vec2 m_texcoord0 = vec2(0);
  vec2 m_texcoord1 = vec2(1);

public:
  billboard_quad(){}

  void rect(const vec2& v0, const vec2& v1);
  void texcoord(const vec2& tc0, const vec2& tc1);

protected:
  void build_vertex();
  void build_texcoord();

};

}

#endif /* QUAD_H */
