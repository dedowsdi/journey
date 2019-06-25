#include "otree.h"
#include "debugger.h"

namespace  zxd
{

bool intersect(GLfloat x0, GLfloat x1, GLfloat x2, GLfloat x3)
{
  return (x1 >= x2 && x1 <= x3) || (x3 >= x0 && x3 <= x1);
}

//--------------------------------------------------------------------
box::box(const vec3& corner0, const vec3& corner1):
  m_corner0(corner0),
  m_corner1(corner1)
{
}

//--------------------------------------------------------------------
bool box::intersect(const box& b) const
{
  const vec3& c0 = b.corner0();
  const vec3& c1 = b.corner1();
  return zxd::intersect(m_corner0.x, m_corner1.x, c0.x, c1.x) &&
    zxd::intersect(m_corner0.y, m_corner1.y, c0.y, c1.y) &&
    zxd::intersect(m_corner0.z, m_corner1.z, c0.z, c1.z);
}

//--------------------------------------------------------------------
void box::set_pos(const vec3& p)
{
  vec3 hsize = this->size() * 0.5f;
  m_corner0 = p - hsize;
  m_corner1 = p + hsize;
}

//--------------------------------------------------------------------
std::array<box, 8> box::subdivide()
{
  std::array<box, 8> res;
  vec3 center = (m_corner0 + m_corner1) * 0.5f;
  vec3 size = this->size() * 0.5f;
  // top 4, +x+y+z as 1st quadrant, ccw
  vec3 start = center;
  res[0] = box(start, start + size);
  start.x -= size.x;
  res[1] = box(start, start + size);
  start.y -= size.y;
  res[2] = box(start, start + size);
  start.x += size.x;
  res[3] = box(start, start + size);
  // bottom 4, -x-y-z as 5th quadrant, ccw
  start = center + vec3(0, 0, -size.z);
  res[4] = box(start, start + size);
  start.x -= size.x;
  res[5] = box(start, start + size);
  start.y -= size.y;
  res[6] = box(start, start + size);
  start.x += size.x;
  res[7] = box(start, start + size);

  return res;
}

void collect_corner_lines(vec3_vector& lines, const vec3& corner, const vec3& diagnal)
{
  lines.push_back(corner);
  lines.push_back(corner + vec3(diagnal.x, 0, 0));
  lines.push_back(corner);
  lines.push_back(corner + vec3(0, diagnal.y, 0));
  lines.push_back(corner);
  lines.push_back(corner + vec3(0, 0, diagnal.z));
}

//--------------------------------------------------------------------
void box::collect_debug_geometry(vec3_vector& lines)
{
  vec3 diag = m_corner1 - m_corner0;
  // get 4 * 3 = 12 lines
  collect_corner_lines(lines, m_corner0, diag);
  collect_corner_lines(lines, m_corner0 + vec3(diag.x, diag.y, 0), vec3(-diag.x, -diag.y, diag.z));
  collect_corner_lines(lines, m_corner1 - vec3(diag.x, 0, 0), vec3(diag.x, -diag.y, -diag.z));
  collect_corner_lines(lines, m_corner1 - vec3(0, diag.y, 0), vec3(-diag.x, diag.y, -diag.z));
}
//--------------------------------------------------------------------
octree::octree(const vec3& corner0, const vec3& corner1)
{
  m_root = new onode(box(corner0, corner1), this);
}

//--------------------------------------------------------------------
octree::~octree()
{
  if(m_root)
    delete m_root;
}

//--------------------------------------------------------------------
void octree::insert(fruit* f)
{
  m_data.push_back(f);
  if(!m_root->insert(f, m_data.size() - 1))
    throw std::runtime_error("failed to insert point to octree");
}

//--------------------------------------------------------------------
void octree::debug_draw(const mat4& mvp_mat)
{
  vec3_vector lines;
  lines.reserve(1024);
  m_root->collect_debug_geometry(lines);

  debugger::draw_line(GL_LINES, lines, mvp_mat);
}

//--------------------------------------------------------------------
std::vector<fruit*> octree::get(const box& b)
{
  std::vector<fruit*> res;
  std::set<GLint> indices;
  m_root->get(indices, b);
  for(auto index : indices)
  {
    res.push_back(get_fruit(index));
  }
  return res;
}

//--------------------------------------------------------------------
fruit* octree::get_fruit(GLint index)
{
  if(index >= m_data.size())
    throw std::runtime_error("fruit index overflow");
  return m_data[index];
}

//--------------------------------------------------------------------
onode::onode(const box& b, octree* t)
  :bb(b),
  tree(t)
{
  data.reserve(capacity());
}

//--------------------------------------------------------------------
onode::~onode()
{
  if(divided())
  {
    for(auto child : children)
    {
      delete child;
      child = 0;
    }
  }
}

//--------------------------------------------------------------------
bool onode::dividable()
{
  return glm::all(glm::greaterThan(bb.size(), vec3(min_size())));
}

//--------------------------------------------------------------------
bool onode::insert(fruit* item, GLint index)
{
  if(!bb.intersect(item->boundingbox()))
    return false;

  if(!divided())
  {
    if(data.size() < capacity() || !dividable())
    {
      data.push_back(index);
      return true;
    }

    // only divide if it's full and dividable
    subdivide();
  }

  assert(divided());

  bool inserted = false;
  for(auto child : children)
  {
    if(child->insert(item, index))
      inserted =  true;
  }

  if(!inserted)
    throw std::runtime_error("failed to insert");

  return true;
}

//--------------------------------------------------------------------
void onode::subdivide()
{
  //std::cout << "subdivide " << bb.corner0() << ":" << bb.corner1() << std::endl;
  assert(!divided() && dividable());

  auto boxes = bb.subdivide();
  for (int i = 0; i < 8; ++i)
  {
    children[i] = new onode(boxes[i], tree);
  }

  for(const auto& index : data)
  {
    for(auto& child : children)
    {
      child->insert(tree->get_fruit(index), index);
    }
  }
  data.clear();
}

//--------------------------------------------------------------------
void onode::collect_debug_geometry(vec3_vector& lines)
{
  if(!divided())
  {
    bb.collect_debug_geometry(lines);
  }
  else
  {
    for(auto& child : children)
    {
      child->collect_debug_geometry(lines);
    }
  }
}

//--------------------------------------------------------------------
void onode::get(std::set<GLint>& fruits, const box& b)
{
  if(!bb.intersect(b))
    return;

  if(divided())
  {
    for(auto child : children)
    {
      child->get(fruits, b);
    }
  }
  else
  {
    for(auto index : data)
    {
      if(get_fruit(index)->boundingbox().intersect(b))
      {
        fruits.insert(index);
      }
    }
  }
}

}

