#ifndef GL4_GLC_FRACTAL_SPIRAL_H
#define GL4_GLC_FRACTAL_SPIRAL_H

#include <gl.h>
#include <glm.h>
#include <lissajous.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

namespace zxd
{
class spiral_seed
{
public:

  int size() { return m_child ? m_child->size() + 1 : 1; }

  GLfloat approximate_height(GLuint times = 360) ;

  spiral_seed* get_graph_at(int level) ;

  GLfloat angular_speed() const { return m_angular_speed; }
  void angular_speed(GLfloat v, bool broad_cast = false);

  GLfloat radius_scale() const { return m_radius_scale; }
  void radius_scale(GLfloat v, bool broad_cast = false);

  GLfloat origin_scale() const { return m_origin_scale; }
  void origin_scale(GLfloat v, bool broad_cast = false);

  GLfloat angle() const { return m_angle; }
  void angle(GLfloat v){ m_angle = v; }

  GLfloat rotate_angle() const { return m_rotate_angle; }
  void rotate_angle(GLfloat v){ m_rotate_angle = v; }

  GLfloat angular_scale() const { return m_angular_scale; }
  void angular_scale(GLfloat v){ m_angular_scale = v; }

  GLfloat radius() const { return m_lisa.radius(); }
  void radius(GLfloat v){ m_lisa.radius(v);}

  GLuint rose_d() const { return m_lisa.rose_d(); }
  void rose_d(GLuint v){ m_lisa.rose_d(v); }

  GLuint rose_n() const { return m_lisa.rose_n(); }
  void rose_n(GLuint v){ m_lisa.rose_n(v); }

  GLfloat rose_offset() const { return m_lisa.rose_offset(); }
  void rose_offset(GLfloat v){ m_lisa.rose_offset(v); }

  GLuint xscale() const {return m_lisa.xscale();}
  void xscale(GLuint v){m_lisa.xscale(v);}

  GLuint yscale() const {return m_lisa.yscale();}
  void yscale(GLuint v){m_lisa.yscale(v);}

  lissajous::LISSA_TYPE type(){return m_lisa.type();}
  void type(lissajous::LISSA_TYPE type){m_lisa.type(type);}

  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }

  const vec2& last_pos() const { return m_last_pos; }
  void last_pos(const vec2& v){ m_last_pos = v; }

  const lissajous& lisa() {return m_lisa;}

  void remove_pen() ;
  void remove_child() ;
  void add_child() ;
  void update(GLint resolution) ;

  spiral_seed* get_pen() { return m_child ? m_child->get_pen() : this; }

  void debug_draw() { m_lisa.draw(); }

  void reset(GLfloat angle = fpi2) ;

  spiral_seed* get_child_at(int depth) ;

  //const mat4& scale_mat() const { return m_scale_mat; }
  //void scale_mat(const mat4& v){ m_scale_mat = v; }
  vec3 scale() {return vec3(radius()); }

  spiral_seed* parent() const { return m_parent; }
  void parent(spiral_seed* v){ m_parent = v; }

  const std::shared_ptr<spiral_seed> child() const { return m_child; }
  void child(std::shared_ptr<spiral_seed> v){ m_child = v; }

private:

  void update_pos() ;
  void update_scale() ;

  GLfloat m_angular_speed;
  GLfloat m_angle = 0; // current orbit angle around parent
  GLfloat m_rotate_angle = 0;
  GLfloat m_radius_scale;
  GLfloat m_origin_scale;
  GLfloat m_angular_scale;
  lissajous m_lisa;

  vec2 m_pos = vec2(0); // current position
  vec2 m_last_pos = vec2(0);

  spiral_seed* m_parent;
  std::shared_ptr<spiral_seed> m_child;

};

}

#endif /* ifndef GL4_GLC_FRACTAL_SPIRAL_H */
