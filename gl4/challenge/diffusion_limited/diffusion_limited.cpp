// http://paulbourke.net/fractals/dla/
#include "app.h"
#include "bitmap_text.h"
#include "common_program.h"
#include "debugger.h"
#include <sstream>

#define WIDTH 600
#define HEIGHT 600
#define RADIUS 3
#define NUM_WALKERS 1500
#define ITERATIONS 20
#define WALK_SPEED 1
#define STICKINESS 0.5
#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd
{

lightless_program prg;

glm::mat4 v_mat;
glm::mat4 p_mat;

struct node
{
  bool leaf;
  node* node0 = 0; // lb
  node* node1 = 0; // rb
  node* node2 = 0; // rt
  node* node3 = 0; // lt

  vec2 min_ext;
  vec2 max_ext;
  vec2_vector vertices;

  node(){}
  node(const vec2& _min_ext, const vec2& _max_ext, bool _leaf):
    min_ext(_min_ext),
    max_ext(_max_ext),
    leaf(_leaf)
  {
  }

  ~node()
  {
    //std::cout << "destruct" << std::endl;
    if(node0) delete(node0);
    if(node1) delete(node1);
    if(node2) delete(node2);
    if(node3) delete(node3);
  }

  void debug_draw(const mat4& m)
  {
    debugger::draw_line(min_ext,  vec2(max_ext.x, min_ext.y), m);
    debugger::draw_line(vec2(max_ext.x, min_ext.y), max_ext, m);
    debugger::draw_line(max_ext, vec2(min_ext.x, max_ext.y), m);
    debugger::draw_line(vec2(min_ext.x, max_ext.y), min_ext, m);

    if(node0) node0->debug_draw(m);
    if(node1) node1->debug_draw(m);
    if(node2) node2->debug_draw(m);
    if(node3) node3->debug_draw(m);
  }

  bool ext_contain(const vec2& vertex)
  {
    return glm::all(glm::lessThanEqual(min_ext, vertex)) && glm::all(glm::lessThan(vertex, max_ext)); 
  }

  void build_leaves(const vec2& leaf_size)
  {
    if(leaf)
      return;

    vec2 quad_size = (max_ext - min_ext) * 0.5f;
    bool is_leaf = glm::all(glm::lessThanEqual(quad_size*0.5f,leaf_size));

    node0 = new node(min_ext, min_ext + quad_size, is_leaf);
    node1 = new node(min_ext + vec2(quad_size.x, 0), max_ext - vec2(0, quad_size.y), is_leaf);
    node2 = new node(min_ext + quad_size, max_ext, is_leaf);
    node3 = new node(min_ext + vec2(0, quad_size.y), max_ext - vec2(quad_size.x, 0), is_leaf);

    node0->build_leaves(leaf_size);
    node1->build_leaves(leaf_size);
    node2->build_leaves(leaf_size);
    node3->build_leaves(leaf_size);
  }

  // get leaf that might contain this vertex, create if not exist
  // actual leaf size will be 1st power of n that's greater than leaf_size
  node* get_leaf(const vec2& vertex, const vec2& leaf_size)
  {
    if(!ext_contain(vertex))
      return 0;

    if(leaf)
      return this;

    node* l = 0;
    if(l) return l; else l = node0->get_leaf(vertex, leaf_size);
    if(l) return l; else l = node1->get_leaf(vertex, leaf_size);
    if(l) return l; else l = node2->get_leaf(vertex, leaf_size);
    if(l) return l; else l = node3->get_leaf(vertex, leaf_size);

    return l;
  }

  std::vector<node*> get_leaves(const vec2& vertex, const vec2& leaf_size, float radius)
  {
    if(radius > leaf_size.x || radius > leaf_size.y)
      throw std::runtime_error("radius shoule be smaller than leaf size");

    node* n0 = get_leaf(vertex + vec2(-radius), leaf_size);
    node* n1 = get_leaf(vertex + vec2(radius, -radius), leaf_size);
    node* n2 = get_leaf(vertex + vec2(radius), leaf_size);
    node* n3 = get_leaf(vertex + vec2(-radius, radius), leaf_size);
    std::vector<node*> leaves;

    if(n0) leaves.push_back(n0);
    if(n1) leaves.push_back(n1);
    if(n2) leaves.push_back(n2);
    if(n3) leaves.push_back(n3);
    return leaves;
  }

};

typedef std::vector<node*> node_vector;

class quad_tree
{
protected:
  node m_root;
  // this is the desired leaf size, actural leaf size is the last 
  // (size / 2^n) that's greater than this size
  vec2 m_leaf_size;

public:
  quad_tree(){}
  quad_tree(const vec2& min_ext, const vec2& max_ext, const vec2& leaf_size):
    m_leaf_size(leaf_size)
  {
    m_root.min_ext = min_ext;
    m_root.max_ext = max_ext;
    m_root.leaf = false;
  }

  void push(const vec2& vertex)
  {
    node* leaf = m_root.get_leaf(vertex, m_leaf_size);
    if(!leaf)
    {
      std::cout <<"vertex " << vertex << "doesn't belong to tree " << m_root.min_ext << " : " << m_root.max_ext << std::endl;
    }
    leaf->vertices.push_back(vertex);
    std::cout << "push " << vertex << " to leaf " << leaf->min_ext << ", " << leaf->max_ext << std::endl;
  }

  void build_leaves()
  {
    m_root.build_leaves(m_leaf_size);
  }

  std::vector<node*> get_leaves(const vec2& vertex, float radius)
  {
    return m_root.get_leaves(vertex, m_leaf_size, radius);
  }

  void debug_draw(const mat4& m)
  {
    m_root.debug_draw(m);
  }

};

// diffusion limited tree
class dl_tree
{
protected:
  GLuint m_vao;
  GLuint m_vbo;
  GLfloat m_radius = 2;
  GLfloat m_walk_speed = 1;
  GLfloat m_stickiness = 0.5;
  vec2 m_min_ext = vec2(0, 0);
  vec2 m_max_ext = vec2(1000, 1000);
  vec2_vector m_vertices;
  vec2_vector m_lines;
  vec2_vector m_walkers;
  quad_tree* m_quad_tree = 0;
public:
  dl_tree() {}
  ~dl_tree()
  {
    if(m_quad_tree)
      delete m_quad_tree;
  }

  //void build()
  //{
    
  //}

  void walk(GLuint num_walkers, GLuint iterations)
  {
    if(m_vertices.size() == 0)
    {
      std::cout << "you must add some seed before you build diffusion limited tree ";
      return;
    }

    if(!m_quad_tree)
    {
      // make sure leaf size is greater than radius size
      m_quad_tree = new quad_tree(m_min_ext, m_max_ext, vec2(m_radius)*2.0f);
      m_quad_tree->build_leaves();
      for (int i = 0; i < m_vertices.size(); ++i)
      {
        m_quad_tree->push(m_vertices[i]);
      }
    }

    m_vertices.reserve(width() * height() / (50 * m_radius)); // guess

    for (int i = m_walkers.size(); i < num_walkers; ++i)
    {
      m_walkers.push_back(glm::linearRand(m_min_ext, m_max_ext));
      //std::cout << "add new walker" << std::endl;
    }

    GLfloat w = width();
    GLfloat h = height();

    GLfloat radius2 = m_radius * m_radius;

    while(!m_walkers.empty() && iterations--)
    {
      for(auto iter = m_walkers.begin(); iter != m_walkers.end(); )
      {
        vec2& walker = *iter;

        // approximation of brownian motion
        walker += glm::circularRand(m_walk_speed);
        if(walker.x < m_min_ext.x)
          walker.x += w;
        else if(walker.x > m_max_ext.x)
          walker.x -= w;
        if(walker.y < m_min_ext.y)
          walker.y += h;
        else if(walker.y > m_max_ext.y)
          walker.y -= h;

        // stick test
        bool stuck = false;
        auto leaves = m_quad_tree->get_leaves(walker, m_radius);
        if(leaves.size() > 4)
          std::cout << "illegal leveas, size " << leaves.size() << std::endl;

        for (int i = 0; i < leaves.size() && !stuck; ++i)
        {

          vec2_vector& leaf_vertices = leaves[i]->vertices;
          for (int j = 0; j < leaf_vertices.size(); ++j)
          {

            if(glm::length2(leaf_vertices[j] - walker) <= radius2 && glm::linearRand(0.0f, 1.0f) < m_stickiness)
            {
              std::cout << "stick " << walker << " to " << leaf_vertices[j] << std::endl;
              m_vertices.push_back(walker);
              m_lines.push_back(leaf_vertices[j]);
              m_lines.push_back(walker);

              bool add_to_leaf = false;
              for (int k = 0; k < leaves.size(); ++k)
              {
                if(leaves[k]->ext_contain(walker))
                {
                  leaves[k]->vertices.push_back(walker);
                  add_to_leaf = true;
                  break;
                }
              }

              if(!add_to_leaf)
                std::cout << "vertex " << walker << "failed to add_to_leaf" << std::endl;
              //leaf_vertices.push_back(walker);
              iter = m_walkers.erase(iter);
              stuck = true;
              break;
            }
          }
        }

        if(!stuck)
          ++iter;
      }

    }

  }

  void compile()
  {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);
    
    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vec2),
      value_ptr(m_vertices.front()), GL_STATIC_DRAW);
    
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  void draw()
  {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_POINTS, 0, m_vertices.size());
  }

  void add_seed(const vec2& v)
  {
    m_vertices.push_back(glm::clamp(v, m_min_ext, m_max_ext));
  }

  GLfloat width() const { return m_max_ext.x - m_min_ext.x; }

  GLfloat height() const { return m_max_ext.y - m_min_ext.y; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  const vec2& min_ext() const { return m_min_ext; }
  void min_ext(const vec2& v){ m_min_ext = v; }

  const vec2& max_ext() const { return m_max_ext; }
  void max_ext(const vec2& v){ m_max_ext = v; }

  const vec2_vector& vertices() const { return m_vertices; }
  void vertices(const vec2_vector& v){ m_vertices = v; }

  const vec2_vector& walkers() const { return m_walkers; }
  void walkers(const vec2_vector& v){ m_walkers = v; }

  const vec2_vector& lines() const { return m_lines; }
  void lines(const vec2_vector& v){ m_lines = v; }

  GLfloat walk_speed() const { return m_walk_speed; }
  void walk_speed(GLfloat v){ m_walk_speed = v; }

  GLfloat stickiness() const { return m_stickiness; }
  void stickiness(GLfloat v){ m_stickiness = v; }

  quad_tree* get_quad_tree(){return m_quad_tree;}
};

class diffusion_limited_app : public app
{
protected:
  dl_tree m_tree;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "diffusion_limited_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    prg.init();
    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());

    m_tree.min_ext(vec2(0));
    m_tree.max_ext(vec2(WIDTH, HEIGHT));
    m_tree.radius(RADIUS);
    m_tree.walk_speed(WALK_SPEED);
    m_tree.stickiness(STICKINESS);

    m_tree.add_seed(vec2(WIDTH*0.5f, HEIGHT*0.5f));
    //m_tree.build();
    //m_tree.compile();
  }

  virtual void update()
  {
    m_tree.walk(NUM_WALKERS, ITERATIONS);
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat));
    glUniform4f(prg.ul_color, 0,0,0,0);

    //debugger::draw_point(m_tree.vertices(), p_mat, m_tree.radius(), vec4(1,0,0,1));
    debugger::draw_point(m_tree.walkers(), p_mat, m_tree.radius(), vec4(0,0,0,1));
    debugger::draw_line(GL_LINES, m_tree.lines(), p_mat);
    //m_tree.get_quad_tree()->debug_draw(p_mat);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
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
  zxd::diffusion_limited_app app;
  app.run();
}
