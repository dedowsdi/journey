/*
 * https://en.wikipedia.org/wiki/A*_search_algorithm
 *
 * If the heuristic function h is admissible, meaning that it never
 * overestimates the actual minimal cost of reaching the goal, then A* is itself
 * admissible (or optimal) if we do not use a closed set. If a closed set is
 * used, then h must also be monotonic (or consistent) for A* to be optimal.
 */
#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "common_program.h"
#include <set>
#include <queue>
#include <map>
#include <cfloat>
#include "quad.h"
#include "string_util.h"
#include <algorithm>

#define WIDTH 800
#define HEIGHT 800
#define COLS 80
#define ROWS 80

namespace zxd {

template <class T, class S, class C>
const S& Container(const std::priority_queue<T, S, C>& q) {
  struct HackedQueue : private std::priority_queue<T, S, C> {
    static const S& Container(const std::priority_queue<T, S, C>& q) {
      //return q.c;
      return q.*&HackedQueue::c;
    }
  };
  return HackedQueue::Container(q);
}

struct cell;

lightless_program prg;
vec4 open_set_color = vec4(0,1,0,1);
vec4 closed_set_color = vec4(1,0,0,1);
vec4 path_color = vec4(0,0,1,1);
vec4 normal_color = vec4(1,1,1,1);
cell* start;
cell* goal;
cell* current;
quad geometry;

typedef std::vector<cell> cell_vector;
typedef std::vector<cell*> cellp_vector;
std::set<cell*> closed_set;
std::set<cell*> open_set; // used to search item in open set

typedef std::map<cell*, GLfloat> cell_score_map;
cell_score_map gscores;
cell_score_map fscores;

std::map<cell*, cell*> came_from;

struct cell_compare
{
  bool operator()(cell* lhs, cell* rhs)
  {
    return fscores[lhs] > fscores[rhs];
  }
};

struct cell
{
  bool wall = false;
  vec2 pos; // col, row

  cell(const vec2& pos_): pos(pos_)
  {
  }

  void draw(const vec2& size)
  {
    if(wall)
      return;

    mat4 m = prg.vp_mat * glm::translate(vec3(pos * size, 0));
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(m));
    geometry.draw();
  }

};

GLfloat heuristic(cell* c0, cell* c1)
{
  vec2 d = c1->pos - c0->pos;
  //return d.x + d.y;
  return glm::length(d);
}

GLfloat g(cell* c0, cell* c1)
{
  vec2 d = c1->pos - c0->pos;
  return glm::length(d);
}

typedef std::priority_queue<cell*, std::vector<cell*>, cell_compare> pq_cell; // used to get item with lowerest f in open set
// use priority_queue to make xue lowerest f in the front
pq_cell open_set_queue;
bool exists(const pq_cell& q, const cell* c)
{
  const std::vector<cell*>& cells = Container(q);
  return std::find(cells.begin(), cells.end(), c) != cells.end();
}

class grid
{
protected:
  GLfloat m_width;
  GLfloat m_height;
  GLint m_rows;
  GLint m_cols;
  cell_vector m_cells;

public:

  void build_cells()
  {
    m_cells.clear();
    m_cells.reserve(m_rows * m_cols);

    for (int y = 0; y < m_rows; ++y) 
    {
      for (int x = 0; x < m_cols; ++x) 
      {
        m_cells.emplace_back(vec2(x, y));
      }
    }

  }

  void draw()
  {
    vec2 cz = cell_size();
    // draw all cells in normal color
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(normal_color));
    for(auto& item : m_cells)
    {
      item.draw(cz);
    }

    // draw closed set
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(closed_set_color));
    for(auto iter = closed_set.begin(); iter != closed_set.end(); ++iter)
    {
      (*iter)->draw(cz);
    }

    // draw open set
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(open_set_color));
    auto cells = Container(open_set_queue);
    for(auto& item : cells)
    {
      item->draw(cz);
    }

    // draw path of current
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(path_color));
    cell* c = current;
    c->draw(cz);
    auto iter = came_from.find(current);
    while(iter != came_from.end())
    {
      iter->second->draw(cz);
      iter = came_from.find(iter->second);
    }

  }

  GLfloat width() const { return m_width; }
  void width(GLfloat v){ m_width = v; }

  GLfloat height() const { return m_height; }
  void height(GLfloat v){ m_height = v; }

  GLint rows() const { return m_rows; }
  void rows(GLint v){ m_rows = v; }

  GLint cols() const { return m_cols; }
  void cols(GLint v){ m_cols = v; }

  cell* get_cell(GLint row, GLint col)
  {
    if(row < 0 || row >= m_rows || col < 0 || col >= m_cols)
      throw std::runtime_error("illegal row:col " + string_util::to(row) + ":" + string_util::to(col));

    return &m_cells.at(row * m_cols + col);
  }

  // cell width, height
  vec2 cell_size()
  {
    return vec2(m_width/m_cols, m_height/m_rows);
  }

  cellp_vector get_neighbours(GLint row, GLint col)
  {
    cellp_vector cells;
    if(row > 0)
      cells.push_back(get_cell(row - 1, col));
    if(row < m_rows - 1)
      cells.push_back(get_cell(row + 1, col));
    if(col > 0)
      cells.push_back(get_cell(row, col -1));
    if(col < m_cols - 1)
      cells.push_back(get_cell(row, col + 1));
    if(row > 0 && col > 0)
      cells.push_back(get_cell(row - 1, col - 1));
    if(row > 0 && col < m_cols - 1)
      cells.push_back(get_cell(row - 1, col + 1));
    if(row < m_cols - 1 && col > 0)
      cells.push_back(get_cell(row + 1, col - 1));
    if(row < m_cols - 1 && col < m_cols - 1)
      cells.push_back(get_cell(row + 1, col + 1));
    return cells;
  }
};

class astar_app : public app {
protected:
  bitmap_text m_text;
  grid m_grid;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "astar_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    m_grid.width(WIDTH);
    m_grid.height(HEIGHT);
    m_grid.cols(COLS);
    m_grid.rows(ROWS);
    m_grid.build_cells();

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);
    prg.vp_mat = prg.vp_mat;

    vec2 cell_size = m_grid.cell_size();
    geometry.setup(pza + vec3(cell_size*0.05f, 0), pxa * cell_size.x * 0.9f, pya * cell_size.y * 0.9f);
    geometry.build_mesh();

    reset();

  }

  void reset()
  {
    start = m_grid.get_cell(0, 0);
    goal = m_grid.get_cell(ROWS - 1, COLS-1);
    current = 0;

    while(!open_set_queue.empty())
      open_set_queue.pop();
    open_set_queue.push(start);
    open_set.insert(start);
    open_set.clear();
    closed_set.clear();
    came_from.clear();


    // init scores
    for (int y = 0; y < ROWS; ++y) 
    {
      for (int x = 0; x < COLS; ++x) 
      {
        cell* c = m_grid.get_cell(y, x);
        gscores[c] = FLT_MAX;
        fscores[c] = FLT_MAX;
        c->wall = glm::linearRand(0.0f, 1.0f) < 0.3f;
      }
    }

    gscores[start] = 0;
    fscores[start] = heuristic(start, goal);
    start->wall = false;
    goal->wall = false;
  }

  virtual void update() 
  {
    // animate
    if(open_set_queue.empty() || current == goal)
      return;

    current = open_set_queue.top();
    open_set_queue.pop();
    open_set.erase(current);

    // reached end
    if(current == goal)
    {
      std::cout << "found solution" << std::endl;
      return;
    }

    // debug check
    if(closed_set.find(current) != closed_set.end())
      throw std::runtime_error("found current also in closed set");

    closed_set.insert(current);
    auto neighbours = m_grid.get_neighbours(current->pos.y, current->pos.x);
    for(auto& nb : neighbours)
    {
      // ignore neighbour which is already evaluated
      if(nb->wall || closed_set.find(nb) != closed_set.end())
        continue;

      // the distance from start to neighbour
      //GLfloat tentative_gscore = gscores[current] + 1;
      GLfloat tentative_gscore = gscores[current] + g(current, nb);

      if(open_set.find(nb) == open_set.end())
      {
        open_set_queue.push(nb); // found new node
        open_set.insert(nb);
      }
      else if(tentative_gscore >= gscores[nb])
        continue; // this is not a better path

      // best path until now, record it
      gscores[nb] = tentative_gscore;
      fscores[nb] = tentative_gscore + heuristic(nb, goal);
      came_from[nb] = current;

      //std::cout << "update score " << nb->pos.y << ":" << nb->pos.x 
                //<< " g:" << gscores[nb] << " f:" << fscores[nb] << std::endl;
    }
    //std::cout << "-------------" << std::endl;
    
    //m_pause = true;
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    m_grid.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps << std::endl;
    ss << "q : reset" << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20, vec4(0.5, 0, 1, 1));
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
          reset();
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
  zxd::astar_app app;
  app.run();
}
