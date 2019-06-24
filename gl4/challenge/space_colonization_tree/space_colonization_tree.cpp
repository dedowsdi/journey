/*
 * spread leaves, use leaves to attract branch to grow to them.
 *
 * 1. spread leaves
 * 2. create tree, create root branch, grow from root branch until it's in
 * attract range of a leaf
 * 3. loop every leaf to find closest branch in it's attract range, remove leaf if
 * it's too close to a branch. grow new branch from closest branch, use it's
 * original dir + leaf-branch as new dir
 *
 */
#include "app.h"
#include "bitmap_text.h"
#include "common_program.h"
#include <list>
#include <unordered_set>
#include <unordered_map>
#include "points.h"

#define WIDTH 800
#define HEIGHT 800

#define NUM_LEAVES 1000
#define MIN_ATTRACT 10
#define MAX_ATTRACT 50
#define BRANCH_STEP 5
#define ANIM_TIME 5

namespace zxd {

lightless_program prg;

glm::mat4 p_mat;

struct leaf
{
  vec2 pos;
  leaf(const vec2& p):
    pos(p)
  {
  }
};

typedef std::list<leaf> leaf_list;

struct branch
{
  vec2 pos;
  vec2 dir;
  branch* parent;

  branch(branch* _parent, const vec2& _pos, const vec2& d):
    parent(_parent),
    pos(_pos),
    dir(d)
  {
  }

  GLfloat distance2(const leaf& l) const
  {
    return glm::length2(l.pos - pos);
  }
};

/*
 * branch needs to store parent branch pointer, if i used std::vector<branch>,
 * all pointer will be invalid if vector is reallocated.
 */
typedef std::vector<branch*> branch_vector;

class space_colonization_tree
{
protected:
  GLuint m_vao = -1;
  GLuint m_vbo = -1;
  GLfloat m_min_attract;
  GLfloat m_max_attract;
  GLfloat m_branch_step;

  branch_vector m_branches;

  vec2_vector m_vertices;

public:

  ~space_colonization_tree()
  {
    for(auto item : m_branches)
    {
      delete item;
    }
    m_branches.clear();
  }

  void accept(leaf_list& leaves)
  {
    grow_to_reach(leaves);
    attracted_to_leaves(leaves);
  }

  //create root,  grow from root until in attract range of a leaf
  void grow_to_reach(leaf_list& leaves)
  {
    m_branches.clear();
    m_branches.reserve(1024); // 1024 means nothing

    //GLfloat min2 = m_min_attract * m_min_attract;
    GLfloat max2 = m_max_attract * m_max_attract;

    // create root
    m_branches.push_back(new branch(0, vec2(WIDTH*0.5, 0), vec2(0,1)));

    // current might cause problem if branches changed(space reallocated)
    branch* current = m_branches.front();
    bool found = false;
    while(true)
    {
      for(const auto& leaf : leaves)
      {
        if(current->distance2(leaf) < max2)
        {
          std::cout << "branch " << current->pos << " reached " << leaf.pos << std::endl;
          found = true;
          break;
        }
      }

      if(found)
        break;

      // grow
      m_branches.push_back(new branch(current, current->pos + current->dir * m_branch_step, current->dir));
      current = m_branches.back();
      //std::cout << "grow to " << current->pos << std::endl;

      if(current->pos.y > HEIGHT)
      {
        std::cout << "wrong leaves, grown out of space";
        break;
      }
    }

  }

  void attracted_to_leaves(leaf_list& leaves)
  {
    GLfloat min2 = m_min_attract * m_min_attract;
    GLfloat max2 = m_max_attract * m_max_attract;

    // grow from last one in current branch
    GLuint start_index = m_branches.size() - 1;

    while(true)
    {
      // loop every leaf, search it's attract branch
      std::unordered_map<branch*, vec2> new_branches;
      for(auto iter = leaves.begin(); iter != leaves.end();)
      {
        branch* ab = 0;
        GLfloat min_d2 = 0;
        for (int i = start_index; i < m_branches.size(); ++i) {

          branch* b = m_branches[i];
          
          GLfloat d2 = b->distance2(*iter);

          // skip if it's too far away
          if(d2 > max2)
            continue;

          // remove reached
          if(d2 < min2)
          {
            //std::cout << "remove leave " << iter->pos << std::endl;
            iter = leaves.erase(iter);
            break;
          }

          if(ab == 0 || d2 < min_d2)
          {
            ab = b;
            min_d2 = d2;
          }
        }

        if(ab == 0)
        {
          ++iter;
          continue;
        }

        //std::cout << "branch " << ab->pos << " attracted to leaf " << iter->pos << std::endl;

        // create new attracted branch
        // all force are normalized, so the densed area attract more branches
        vec2 force = glm::normalize(iter->pos - ab->pos);
        auto it = new_branches.find(ab);
        if(it == new_branches.end())
          new_branches.insert(std::make_pair(ab, ab->dir + force));
        else
          it->second += force;

        ++iter;
      }

      // update branch grow start index. swap finished branch with start branch.
      for (int i = start_index+1; i < m_branches.size(); ++i) {
        if(new_branches.find(m_branches[i]) == new_branches.end())
        {
          std::swap(m_branches[i], m_branches[start_index]);
          ++start_index;
        }
      }

      if(new_branches.empty())
      {
        std::cout << "stop growing, " << leaves.size() << " leaves left" << std::endl;
        break;
      }

      // add new attracted branch
      for(auto iter = new_branches.begin(); iter != new_branches.end(); ++iter){
        vec2 dir = glm::normalize(iter->second);
        vec2 pos = iter->first->pos + m_branch_step * dir;
        m_branches.push_back(new branch(iter->first, pos, dir));
        //std::cout << "create new branch " << (*m_branches.back()).pos << std::endl;
      }
    }
  }

  void compile()
  {
    m_vertices.clear();
    m_vertices.reserve(m_branches.size() * 2);

    for(auto b : m_branches)
    {
      if(!b->parent)
        continue;

      m_vertices.emplace_back(b->parent->pos);
      m_vertices.emplace_back(b->pos);
    }

    if(m_vao == -1)
      glGenVertexArrays(1, &m_vao);

    if(m_vbo == -1)
      glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vec2), glm::value_ptr(m_vertices.front()), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  void draw(GLuint vertex_count)
  {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_LINES, 0, vertex_count);
  }

  GLuint num_vertices(){return m_vertices.size();}

  GLfloat min_attract() const { return m_min_attract; }
  void min_attract(GLfloat v){ m_min_attract = v; }

  GLfloat max_attract() const { return m_max_attract; }
  void max_attract(GLfloat v){ m_max_attract = v; }

  GLfloat branch_step() const { return m_branch_step; }
  void branch_step(GLfloat v){ m_branch_step = v; }
};

class space_colonization_tree_app : public app {
protected:
  GLuint m_draw_count = 0;
  GLfloat m_time = 0;
  bitmap_text m_text;
  space_colonization_tree m_tree;
  leaf_list m_leaves;
  points<vec2> m_points;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "space_colonization_tree_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    p_mat = glm::ortho<GLfloat>(0, WIDTH, 0, HEIGHT);

    m_tree.min_attract(MIN_ATTRACT);
    m_tree.max_attract(MAX_ATTRACT);
    m_tree.branch_step(BRANCH_STEP);

    vec2_vector vertices;
    for (GLuint i = 0; i < NUM_LEAVES; ++i) {
      vec2 pos = glm::linearRand(vec2(0, 300), vec2(WIDTH, HEIGHT));
      vertices.push_back(pos);
      m_leaves.emplace_back(pos);
    }

    m_tree.accept(m_leaves);
    m_tree.compile();

    m_points.build_mesh(vertices);
  }

  virtual void update() {
    m_time += m_delta_time;
    m_draw_count = m_tree.num_vertices() * m_time / ANIM_TIME;
    m_draw_count &= ~(1); // even
    m_draw_count = glm::min(m_tree.num_vertices(), m_draw_count);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniform4f(prg.ul_color, 0.5,0,0,1);
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat));

    glPointSize(5);
    m_points.draw();

    glUniform4f(prg.ul_color, 1,1,1,1);
    m_tree.draw(m_draw_count);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_text.print("q : replay", 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_Q:
          m_time = 0;
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::space_colonization_tree_app app;
  app.run();
}
