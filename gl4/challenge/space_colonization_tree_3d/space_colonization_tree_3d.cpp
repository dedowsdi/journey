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
 * @TODO the lighting is wrong, normal must be wrong.
 *
 */
#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include <list>
#include <unordered_set>
#include <unordered_map>
#include "points.h"
#include <algorithm>
#include "geometry_util.h"

#define WIDTH 800
#define HEIGHT 800

#define NUM_LEAVES 300
#define MIN_ATTRACT 10
#define MAX_ATTRACT 150
#define BRANCH_STEP 8
#define ANIM_TIME 5
#define BRANCH_SCALE 0.96f

#define NUM_CYLINDER_FACE 8
#define TRUNK_RADIUS 2.5f
#define BRANCH_RADIUS 0.35f

namespace zxd {

blinn_program prg;
light_vector lights;
light_model lm;
material mtl;

lightless_program prg1;

struct leaf
{
  vec3 pos;
  leaf(const vec3& p):
    pos(p)
  {
  }
};

typedef std::list<leaf> leaf_list;

struct branch
{
  vec3 pos;
  vec3 dir;
  GLuint num_children = 0;
  GLfloat size = 1.0;
  branch* parent;

  branch(branch* _parent, const vec3& _pos, const vec3& d):
    parent(_parent),
    pos(_pos),
    dir(d)
  {
    if(parent)
    {
      size = parent->size * BRANCH_SCALE;
      ++parent->num_children;
    }
  }

  inline GLfloat distance2(const leaf& l) const
  {
    return glm::length2(l.pos - pos);
  }
};

/*
 * branch needs to store parent branch pointer, if i used std::vector<branch>,
 * all pointer will be invalid if vector is reallocated.
 */
typedef std::vector<branch*> branch_vector;

class space_colonization_tree : public geometry_base
{
protected:
  GLuint m_vao = -1;
  GLuint m_vbo = -1;
  GLfloat m_min_attract;
  GLfloat m_max_attract;
  GLfloat m_branch_step;
  GLfloat m_trunk_radius;

  branch_vector m_branches;

  vec4_vector m_vertices;
  vec3_vector m_normals;

public:

  ~space_colonization_tree()
  {
    for(auto item : m_branches)
    {
      delete item;
    }
    m_branches.clear();
  }

  void accept_leaves(leaf_list& leaves)
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
    m_branches.push_back(new branch(0, vec3(0, 0, 0), vec3(0,0,1)));

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

    std::unordered_map<branch*, vec3> last_new_branches;
    while(true)
    {
      // loop every leaf, search it's attract branch
      std::unordered_map<branch*, vec3> new_branches;
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
        vec3 force = glm::normalize(iter->pos - ab->pos);
        auto it = new_branches.find(ab);
        if(it == new_branches.end())
          new_branches.insert(std::make_pair(ab, ab->dir + force));
        else
          it->second += force;

        ++iter;
      }

      // update branch grow start index. swap finished branch(no branch grown
      // from this one) with start branch.
      for (int i = start_index; i < m_branches.size(); ++i) {
        if(new_branches.find(m_branches[i]) == new_branches.end())
        {
          if(i != start_index)
            std::swap(m_branches[i], m_branches[start_index]);
          ++start_index;
        }
      }
      //++start_index;
      
      // handle oscillate problems
      if(!last_new_branches.empty())
      {
        for(auto iter = last_new_branches.begin(); iter != last_new_branches.end(); ++iter)
        {
          auto it = std::find_if(new_branches.begin(), new_branches.end(), 
              [&](const std::pair<branch*, vec3>& item)->bool {
                return item.first == iter->first && item.second == iter->second;
              });
          if(it != new_branches.end())
          {
            std::cout << "found duplicated branch, deleted" << std::endl;
            new_branches.erase(it);
          }
        }
      }

      last_new_branches = new_branches;

      if(new_branches.empty())
      {
        std::cout << "stop growing, " << leaves.size() << " leaves left" << std::endl;
        break;
      }
      else
      {
        //std::cout << "generate " << new_branches.size() << " branches " << std::endl;
      }

      // add new attracted branch
      for(auto iter = new_branches.begin(); iter != new_branches.end(); ++iter){
        vec3 dir = glm::normalize(iter->second);
        vec3 pos = iter->first->pos + m_branch_step * dir;
        m_branches.push_back(new branch(iter->first, pos, dir));
        //std::cout << "create new branch " << iter->first << " " << dir  <<  " " <<   (*m_branches.back()).pos << std::endl;
      }

    }
  }

  std::vector<vec3> createCircle(branch* b)
  {
    static std::vector<vec4> unitCircle;
    if(unitCircle.empty())
    {
      unitCircle.reserve(NUM_CYLINDER_FACE * 6);

      vec4 next(1, 0, 0, 1);
      glm::mat4 step_rotate = glm::rotate(f2pi/NUM_CYLINDER_FACE, pza);
      for (int i = 0; i < NUM_CYLINDER_FACE; ++i) 
      {
        unitCircle.push_back(next);
        next = step_rotate * next;
      }
      unitCircle.push_back(unitCircle.front());
    }

    std::vector<vec3> circle;

    const vec3& bottomCenter = b->parent ? b->parent->pos : b->pos - vec3(0,0,1);
    const vec3& topCenter = b->pos;
    vec3 cylinderAxis = glm::normalize(topCenter- bottomCenter);

    GLfloat xyscale = b->num_children == 0 ? 
      BRANCH_RADIUS : std::max(b->size * TRUNK_RADIUS, BRANCH_RADIUS);

    glm::mat4 m = zxd::rotate_to(pza, cylinderAxis);
    m = glm::translate(topCenter) * m * glm::scale(vec3(xyscale, xyscale, 1));

    std::for_each(unitCircle.begin(), unitCircle.end(), 
        [&](const vec4& v)->void {
          circle.push_back((m * v).xyz());
          //std::cout << circle.back() << std::endl;
        });

    return circle;
  }

  void compile()
  {
    vec3_array_ptr vertices(new vec3_array());
    attrib_array(0, vertices);
    vertices->reserve(m_branches.size() * NUM_CYLINDER_FACE * 6);

    // there will be gaps between branch and branch
    for (int i = 0; i < m_branches.size(); ++i) {
      branch* b = m_branches[i];
      
      if(!b->parent)
        continue;

      std::vector<vec3> bottom_circle = createCircle(b->parent);
      std::vector<vec3> top_circle = createCircle(b);

      // need a better way to control xyscale
      //GLfloat xyscale = glm::mix(trunkRadius, BRANCH_RADIUS,
          //glm::smoothstep(0.0, 1.0, static_cast<GLdouble>(i)/m_branches.size()));
      //OSG_NOTICE << xyscale << std::endl;

      for (int i = 0; i < bottom_circle.size() - 1; ++i) {
        vertices->push_back(bottom_circle[i]);
        vertices->push_back(bottom_circle[i+1]);
        vertices->push_back(top_circle[i+1]);

        vertices->push_back(bottom_circle[i]);
        vertices->push_back(top_circle[i+1]);
        vertices->push_back(top_circle[i]);
      }
    }

    bind_vao();
    vertices->bind(0);
    vertices->update_array_buffer();

    remove_primitive_sets(0, get_num_primitive_set());
    add_primitive_set(new draw_arrays(GL_TRIANGLES, 0, vertices->size()));

    geometry_util::smooth(*this, 1);

    std::cout << "generate " << vertices->size() << " vertices" << std::endl;
  }

  void draw_branches(GLuint branch_count)
  {
    draw_arrays* da = static_cast<draw_arrays*>(get_primitive_set(0));
    da->count(branch_count * NUM_CYLINDER_FACE * 6);
    geometry_base::draw();
  }

  GLuint num_vertices(){return m_vertices.size();}
  GLuint num_branches(){return m_branches.size() - 1;} // real branch count, not point

  GLfloat min_attract() const { return m_min_attract; }
  void min_attract(GLfloat v){ m_min_attract = v; }

  GLfloat max_attract() const { return m_max_attract; }
  void max_attract(GLfloat v){ m_max_attract = v; }

  GLfloat branch_step() const { return m_branch_step; }
  void branch_step(GLfloat v){ m_branch_step = v; }

  GLfloat trunk_radius() const { return m_trunk_radius; }
  void trunk_radius(GLfloat v){ m_trunk_radius = v; }
};

class space_colonization_tree_app : public app {
protected:
  GLuint m_draw_branch_count = 0;
  GLfloat m_time = 0;
  bitmap_text m_text;
  space_colonization_tree m_tree;
  leaf_list m_leaves;
  points<vec3> m_points;

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
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    prg1.init();

    prg.init();
    prg.v_mat = glm::lookAt(vec3(0, -500, 500), vec3(0), vec3(0,0,1));
    prg.p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 2000.0f);
    set_v_mat(&prg.v_mat);

    // setup light
    light_source l0;
    l0.diffuse = vec4(1);
    l0.position = vec4(0, 0, 1, 0);
    l0.specular = vec4(1);
    lights.push_back(l0);
    
    mtl.diffuse = vec4(0.8);
    mtl.specular = vec4(0.2);
    mtl.shininess = 50;
    lm.local_viewer = true;

    prg.bind_lighting_uniform_locations(lights, lm, mtl);

    m_tree.min_attract(MIN_ATTRACT);
    m_tree.max_attract(MAX_ATTRACT);
    m_tree.branch_step(BRANCH_STEP);

    vec3_vector vertices;
    for (GLuint i = 0; i < NUM_LEAVES; ++i) {
      vec4 pos = glm::scale(vec3(0.65, 0.65, 1)) *  vec4(glm::ballRand(200.0f), 1);
      pos.z = glm::abs(pos.z) + 100;
      vertices.push_back(pos.xyz());
      m_leaves.emplace_back(pos.xyz());
    }

    m_tree.accept_leaves(m_leaves);
    m_tree.compile();

    m_points.build_mesh(vertices);
  }

  virtual void update() {
    m_time += m_delta_time;
    m_draw_branch_count = m_tree.num_branches() * m_time / ANIM_TIME;
    m_draw_branch_count = glm::min(m_tree.num_branches(), m_draw_branch_count);
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    mat4 mvp_mat = prg.p_mat * prg.v_mat;
    prg1.use();
    glUniform4f(prg1.ul_color, 0.5,0,0,1);
    glUniformMatrix4fv(prg1.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));

    glPointSize(5);
    //m_points.draw();

    prg.use();
    prg.update_model(glm::translate(vec3(0, 0, -100)));
    prg.update_lighting_uniforms(lights, lm, mtl);
    m_tree.draw_branches(m_draw_branch_count);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_text.print("q : replay", 10, m_info.wnd_height - 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
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
