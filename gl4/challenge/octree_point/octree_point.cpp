#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "debugger.h"


#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 vp_mat;

lightless_program prg;

bool draw_tree = false;
bool draw_point = true;
GLuint num_points = 10000;
vec3_vector points;
GLfloat boundary = 100;

void collect_corner_lines(vec3_vector& lines, const vec3& corner, const vec3& diagnal)
{
  lines.push_back(corner);
  lines.push_back(corner + vec3(diagnal.x, 0, 0));
  lines.push_back(corner);
  lines.push_back(corner + vec3(0, diagnal.y, 0));
  lines.push_back(corner);
  lines.push_back(corner + vec3(0, 0, diagnal.z));
}

void collect_lines(vec3_vector& lines, const vec3& corner0, const vec3& corner1)
{
  vec3 diag = corner1 - corner0;
  // get 4 * 3 = 12 lines
  collect_corner_lines(lines, corner0, diag);
  collect_corner_lines(lines, corner0 + vec3(diag.x, diag.y, 0), vec3(-diag.x, -diag.y, diag.z));
  collect_corner_lines(lines, corner1 - vec3(diag.x, 0, 0), vec3(diag.x, -diag.y, -diag.z));
  collect_corner_lines(lines, corner1 - vec3(0, diag.y, 0), vec3(-diag.x, diag.y, -diag.z));
}

bool contain(const vec3& corner0, const vec3& corner1, const vec3& p)
{
  return glm::all(glm::lessThanEqual(corner0, p)) && glm::all(glm::lessThanEqual(p, corner1));
}

bool intersect(GLfloat x0, GLfloat x1, GLfloat x2, GLfloat x3)
{
  return (x1 >= x2 && x1 <= x3) || (x3 >= x0 && x3 <= x1);
}

bool intersect(const vec3& c00, const vec3& c01, const vec3& c10, const vec3& c11)
{
  return intersect(c00.x, c01.x, c10.x, c11.x) &&
    intersect(c00.y, c01.y, c10.y, c11.y) &&
    intersect(c00.z, c01.z, c10.z, c11.z);
}

// bouncing cube
struct cube
{
  vec3 pos = vec3(0); // center
  vec3 vel;
  vec3 diag; // full diagnal

  cube(const vec3& diag_):
    diag(diag_)
  {
    vel = glm::sphericalRand(0.25);
  }

  vec3 corner0()
  {
    return pos - diag * 0.5f;
  }
  vec3 corner1()
  {
    return pos + diag * 0.5f;
  }

  void update()
  {
    pos += vel;
    if(pos.x > boundary)
    {
      pos.x = boundary;
      vel.x *= -1;
    }
    else if(pos.x < -boundary)
    {
      pos.x = -boundary;
      vel.x *= -1;
    }

    if(pos.y > boundary)
    {
      pos.y = boundary;
      vel.y *= -1;
    }
    else if(pos.y < -boundary)
    {
      pos.y = -boundary;
      vel.y *= -1;
    }

    if(pos.z > boundary)
    {
      pos.z = boundary;
      vel.z *= -1;
    }
    else if(pos.z < -boundary)
    {
      pos.z = -boundary;
      vel.z *= -1;
    }
  }
  
  void draw(const mat4& mvp_mat)
  {
    vec3_vector lines;
    lines.reserve(24);
    collect_lines(lines, pos -diag*0.5f, pos + diag*0.5f);
    debugger::draw_line(GL_LINES, lines, mvp_mat);
  }
};

// point type octnode, only leaf has data, no data resides in more than 1 leaf
struct onode
{
  bool leaf;
  short capacity = 4; // number of data before subdivide
  GLfloat min_size = 1; // minimum subdivided edge size
  onode* children[8] = {0,0,0,0,0,0,0,0};
  vec3 corner0;
  vec3 corner1;
  vec3 ext;
  vec3_vector data;

  onode(){}
  onode(const vec3& corner0_, const vec3& corner1_):
    corner0(corner0_),
    corner1(corner1_)
  {
    data.reserve(capacity);
  }

  ~onode()
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


  bool divided() {return children[0] != 0;}
  bool dividable()
  {
    vec3 ext = corner1 - corner0;
    return glm::all(glm::greaterThan(ext, vec3(min_size)));
  }

  bool insert(const vec3& item)
  {
    if(!contain(corner0, corner1, item))
      return false;

    if(!divided())
    {
      if(data.size() < capacity || !dividable())
      {
        data.push_back(item);
        return true;
      }

      // only divide if it's full and dividable
      subdivide();
    }

    assert(divided());

    for(auto child : children)
    {
      if(child->insert(item))
        return true;
    }
    throw std::runtime_error("failed to insert");
  }

  void subdivide()
  {
    assert(!divided());

    vec3 center = (corner0 + corner1) * 0.5f;
    vec3 size = center - corner0;
    // top 4, +x+y+z as 1st quadrant, ccw
    vec3 start = center;
    children[0] = new onode(start, start + size);
    start.x -= size.x;
    children[1] = new onode(start, start + size);
    start.y -= size.y;
    children[2] = new onode(start, start + size);
    start.x += size.x;
    children[3] = new onode(start, start + size);
    // bottom 4, -x-y-z as 5th quadrant, ccw
    start = center + vec3(0, 0, -size.z);
    children[4] = new onode(start, start + size);
    start.x -= size.x;
    children[5] = new onode(start, start + size);
    start.y -= size.y;
    children[6] = new onode(start, start + size);
    start.x += size.x;
    children[7] = new onode(start, start + size);

    // move data after subdivid, so only leaf has data for point type
    for(const auto& item : data)
    {
      for(auto& child : children)
      {
        if(child->insert(item))
          break;
      }
    }
    data.clear();
  }

  void collect_debug_geometry(vec3_vector& lines)
  {
    if(!divided())
    {
      collect_lines(lines, corner0, corner1);
    }
    else
    {
      for(auto& child : children)
      {
        child->collect_debug_geometry(lines);
      }
    }
  }

  void get(vec3_vector& points, const vec3& c0, const vec3& c1)
  {
    if(!intersect(corner0, corner1, c0, c1))
      return;

    if(divided())
    {
      for(auto child : children)
      {
        child->get(points, c0, c1);
      }
    }
    else
    {
      for(auto& item : data)
      {
        if(contain(c0, c1, item))
        {
          points.push_back(item);
        }
      }
    }
  }


};

class octree
{
protected:
  onode* m_root = 0;

public:
  octree(const vec3& corner0, const vec3& corner1)
  {
    m_root = new onode(corner0, corner1);
  }

  ~octree()
  {
    if(m_root)
      delete m_root;
  }

  void insert(const vec3& p)
  {
    if(!m_root->insert(p))
      throw std::runtime_error("failed to insert point to octree");
  }

  void debug_draw(const mat4& mvp_mat)
  {
    vec3_vector lines;
    lines.reserve(1024);
    m_root->collect_debug_geometry(lines);

    debugger::draw_line(GL_LINES, lines, mvp_mat);
  }

  void get(vec3_vector& points, const vec3& c0, const vec3& c1)
  {
    m_root->get(points, c0, c1);
  }
};

class octree_app : public app
{
protected:
  bitmap_text m_text;
  octree* m_tree = 0;
  cube m_cube = cube(vec3(20));
  vec3_vector m_cube_points;

public:
  ~octree_app()
  {
    if(m_tree)
      delete m_tree;
  }

  virtual void init_info()
  {
    app::init_info();
    m_info.title = "octree_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    m_tree = new octree(vec3(-100), vec3(100));

    points.reserve(num_points);
    for (int i = 0; i < num_points; ++i)
    {
      points.push_back(glm::linearRand(vec3(-100), vec3(100)));
      m_tree->insert(points.back());
    }

    prg.init();
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
    v_mat = zxd::isometric_projection(200);
    vp_mat = p_mat * v_mat;
    set_v_mat(&v_mat);

  }

  virtual void update()
  {
    vp_mat = p_mat * v_mat;
    m_cube.update();

    m_cube_points.clear();
    m_tree->get(m_cube_points, m_cube.corner0(), m_cube.corner1());
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();

    if(draw_point)
      debugger::draw_point(points, vp_mat, 2, vec4(0));
    if(draw_tree)
      m_tree->debug_draw(vp_mat);

    m_cube.draw(vp_mat);
    debugger::draw_point(m_cube_points, vp_mat, 4, vec4(0, 0, 1, 1));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps << std::endl;
    ss << "q : draw tree : " << draw_tree << std::endl;
    ss << "w : draw cube : " << draw_point << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_Q:
          draw_tree = !draw_tree;
          break;
        case GLFW_KEY_W:
          draw_point = !draw_point;
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[])
{
  assert(zxd::intersect(1, 10, 2, 8));
  assert(zxd::intersect(39, 20, 10, 22));

  zxd::octree_app app;
  app.run();
}
