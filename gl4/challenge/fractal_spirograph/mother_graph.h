#ifndef GL4_CHALLENGE_MOTHER_GRAPH_H
#define GL4_CHALLENGE_MOTHER_GRAPH_H value

#include "gl.h"
#include "lissajous.h"

namespace zxd
{
class mother_graph
{
protected:

  GLfloat m_angular_speed;
  GLfloat m_angle = fpi2; // current orbit angle around parent
  GLfloat m_radius_scale;
  GLfloat m_origin_scale;
  GLfloat m_angular_scale;
  lissajous m_lisa;

  vec2 m_pos = vec2(0); // current position
  vec2 m_last_pos = vec2(0);

  mother_graph* m_parent;
  std::shared_ptr<mother_graph> m_child;

public:

  int size() { return m_child ? m_child->size() + 1 : 1; }

  GLfloat max_height() ;

  mother_graph* get_graph_at(int level) ;


  GLfloat angular_speed() const { return m_angular_speed; }
  void angular_speed(GLfloat v, bool broad_cast = false);

  GLfloat radius_scale() const { return m_radius_scale; }
  void radius_scale(GLfloat v, bool broad_cast = false);

  GLfloat origin_scale() const { return m_origin_scale; }
  void origin_scale(GLfloat v, bool broad_cast = false);

  GLfloat angle() const { return m_angle; }
  void angle(GLfloat v){ m_angle = v; }

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

  void remove_pen() ;
  void remove_child() ;
  void add_child() ;
  void update(GLint resolution) ;
  void update_pos() ;

  mother_graph* get_pen() { return m_child ? m_child->get_pen() : this; }

  void debug_draw() { m_lisa.draw(); }

  void reset() ;
  void update_scale() ;

  mother_graph* get_child_at(int depth) ;

  //const mat4& scale_mat() const { return m_scale_mat; }
  //void scale_mat(const mat4& v){ m_scale_mat = v; }
  vec3 scale() {return vec3(radius()); }

  mother_graph* parent() const { return m_parent; }
  void parent(mother_graph* v){ m_parent = v; }

  std::shared_ptr<mother_graph> child() const { return m_child; }
  void child(std::shared_ptr<mother_graph> v){ m_child = v; }

};

}

#endif /* ifndef GL4_CHALLENGE_MOTHER_GRAPH_H */
