/*
 * https://castle-engine.io/vrml_engine_doc/output/xsl/html/section.how_octree_works.html
 */
#include <glm.h>
#include <list>
#include <array>
#include <set>

namespace zxd
{
struct onode;

class box
{
protected:
  vec3 m_corner0 = vec3(-0.5);
  vec3 m_corner1 = vec3(0.5);
public:
  box(){}
  box(const vec3& corner0, const vec3& corner1);
  vec3 size() const{ return m_corner1 - m_corner0;}
  vec3 center() const{ return (m_corner1 + m_corner0) * 0.5f;}
  bool intersect(const box& b) const;

  vec3 corner0() const { return m_corner0; }
  void corner0(vec3 v){ m_corner0 = v; }

  vec3 corner1() const { return m_corner1; }
  void corner1(vec3 v){ m_corner1 = v; }

  vec3 pos() {return center();}
  void set_pos(const vec3& p);

  std::array<box, 8> subdivide();

  void collect_debug_geometry(vec3_vector& lines);
};

class fruit
{
protected:
  box m_boundingbox;
public:
  const box& boundingbox() const { return m_boundingbox; }
  void boundingbox(const box& v){ m_boundingbox = v; }
};

class octree
{
protected:
  onode* m_root = 0;
  GLint m_capacity = 4; // leaf capacity
  GLfloat m_min_size = 1; // minimal divided size
  std::vector<fruit*> m_data; // need random access

public:
  octree(const vec3& corner0, const vec3& corner1);

  ~octree();

  void insert(fruit* f);

  void debug_draw(const mat4& mvp_mat);

  std::vector<fruit*> get(const box& b);

  GLint capacity() const { return m_capacity; }
  void capacity(GLint v){ m_capacity = v; }

  GLfloat min_size() const { return m_min_size; }
  void min_size(GLfloat v){ m_min_size = v; }

  fruit* get_fruit(GLint index);
};


// regeion type octnode, only leaf has data, data might resides in more than 1 leaf.
// real data stored in main tree, leaf only store index.
struct onode
{
  bool leaf;
  octree* tree;
  onode* children[8] = {0,0,0,0,0,0,0,0};
  box bb;
  vec3 ext;
  int_vector data;

  onode(){}
  onode(const box& b, octree* t);
  ~onode();

  bool divided() {return children[0] != 0;}
  bool dividable();

  GLint capacity() { return tree->capacity();}
  GLfloat min_size() { return tree->min_size();}

  bool insert(fruit* item, GLint index);
  void subdivide() ;
  void collect_debug_geometry(vec3_vector& lines);

  void get(std::set<GLint>& fruits, const box& b);
  fruit* get_fruit(GLint index){return tree->get_fruit(index);}

};

}
