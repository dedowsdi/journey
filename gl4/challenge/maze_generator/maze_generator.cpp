#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include "quad.h"
#include "line.h"
#include <stack>
#include <sstream>

#define WIDTH 800.0f
#define HEIGHT 800.0f

#define NUM_ROWS 20
#define NUM_COLS 20
#define CELL_WIDTH WIDTH/NUM_COLS
#define CELL_HEIGHT HEIGHT/NUM_ROWS
#define HALF_CELL_WIDTH CELL_WIDTH * 0.5
#define HALF_CELL_HEIGHT CELL_HEIGHT * 0.5
#define CELL_COLOR vec4(0,0,0,1)
#define PATH_COLOR vec4(0.5,0,1,1)
#define LINE_COLOR vec4(1)
#define HEAD_COLOR vec4(1,0,0,1)
#define STEP_TIME 0.02
#define QUAD_INDEX 0
#define LINE_INDEX 1

namespace zxd {

twod_program0 prg;

quad quad0;
line line_h(GL_LINES);
line line_v(GL_LINES);

class cell;
cell* current_cell;

class cell
{
public:
  GLuint row;
  GLuint col;
  GLboolean left_wall = GL_TRUE;
  GLboolean right_wall = GL_TRUE;
  GLboolean top_wall = GL_TRUE;
  GLboolean bottom_wall = GL_TRUE;
  GLboolean visited = GL_FALSE;

  cell(GLuint r, GLuint c):
    row(r),
    col(c)
  {
  }

  void draw()
  {
    mat4 m = prg.vp_mat * m_mat();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(m));
    const vec4 color = this == current_cell ? HEAD_COLOR : (visited ? PATH_COLOR : CELL_COLOR);
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(color));
    quad0.draw();

    glUniform4fv(prg.ul_color, 1, glm::value_ptr(LINE_COLOR));
    if(left_wall)
    {
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, 
          glm::value_ptr(m * glm::translate(vec3(-HALF_CELL_WIDTH,0,0))));
      line_v.draw();
    }

    if(right_wall)
    {
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, 
          glm::value_ptr(m * glm::translate(vec3(HALF_CELL_WIDTH,0,0))));
      line_v.draw();
    }

    if(bottom_wall)
    {
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, 
          glm::value_ptr(m * glm::translate(vec3(0,-HALF_CELL_HEIGHT,0))));
      line_h.draw();
    }

    if(top_wall)
    {
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, 
          glm::value_ptr(m * glm::translate(vec3(0,HALF_CELL_HEIGHT,0))));
      line_h.draw();
    }
  }

  mat4 m_mat()
  {
    return glm::translate(vec3(col * CELL_WIDTH + HALF_CELL_WIDTH, row * CELL_HEIGHT + HALF_CELL_HEIGHT, 0));
  }

};

class cell;
typedef std::vector<cell> cell_vector;
typedef std::stack<cell*> cell_stack;

class maze
{
public:
  GLuint m_num_row;
  GLuint m_num_col;
  GLuint m_num_cell_color;
  GLuint m_num_path_color;
  GLuint m_num_head_color;
  GLuint m_num_lines;

  cell_vector m_cells;
  vec2_vector m_quad_vertices;
  vec2_vector m_line_vertices;

  GLuint vaos[2];
  GLuint vbos[2];

  maze(GLuint num_row, GLuint num_col):
    m_num_row(num_row),
    m_num_col(num_col)
  {
    m_cells.reserve(m_num_row * m_num_col);
    for (int row = 0; row < m_num_row; ++row) {
      for (int col = 0; col < m_num_col; ++col) {
        m_cells.push_back(cell(row, col));
      }
    }

    m_quad_vertices.resize(size() * 6);
    m_line_vertices.resize(size() * 8);

    glGenVertexArrays(2, vaos);
    glGenBuffers(2, vbos);
    
    glBindVertexArray(vaos[QUAD_INDEX]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[QUAD_INDEX]);
    glBufferData(GL_ARRAY_BUFFER, m_quad_vertices.size() * sizeof(vec2) , 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glBindVertexArray(vaos[LINE_INDEX]);
    glBindBuffer(GL_ARRAY_BUFFER, vbos[LINE_INDEX]);
    glBufferData(GL_ARRAY_BUFFER, m_line_vertices.size() * sizeof(vec2), 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  GLuint index(GLuint row, GLuint col)
  {
    if(row == -1 || col == -1 || row >= m_num_row || col >= m_num_col )
      return -1;
    return row * m_num_col + col;
  }

  GLuint size()
  {
    return m_cells.size();
  }

  cell* get_cell(GLuint row, GLuint col)
  {
    return get_cell(index(row, col));
  }

  cell* get_cell(GLuint idx)
  {
    return idx == -1 ? 0 : &m_cells[idx];
  }

  cell* get_unvisited_neighbour(cell& c)
  {
    cell* left   = get_cell(c.row, c.col - 1);
    cell* right  = get_cell(c.row, c.col + 1);
    cell* bottom = get_cell(c.row - 1, c.col);
    cell* top    = get_cell(c.row + 1, c.col);
    std::vector<cell*> candidates;
    if(left && !left->visited)
      candidates.push_back(left);
    if(right && !right->visited)
      candidates.push_back(right);
    if(bottom && !bottom->visited)
      candidates.push_back(bottom);
    if(top && !top->visited)
      candidates.push_back(top);

    if(candidates.empty())
      return 0;

    return candidates[glm::linearRand(0.0f, static_cast<GLfloat>(candidates.size()))];
  }

  void remove_wall(cell& lhs, cell& rhs)
  {
    if(rhs.col - lhs.col == 1)
      rhs.left_wall = lhs.right_wall = GL_FALSE;
    else if(rhs.col - lhs.col == -1)
      lhs.left_wall = rhs.right_wall = GL_FALSE;
    else if(rhs.row - lhs.row == 1)
      rhs.bottom_wall = lhs.top_wall = GL_FALSE;
    else if(rhs.row - lhs.row == -1)
      lhs.bottom_wall = rhs.top_wall = GL_FALSE;
    else
      std::cout << "error, illegal remove wall between  " 
          << lhs.row << ":" << lhs.col << "  and  " << rhs.row << ":" << rhs.col << std::endl;
  }

  void draw()
  {
    // draw quads
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat));
    glBindVertexArray(vaos[QUAD_INDEX]);

    GLuint next = 0;
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(HEAD_COLOR));
    glDrawArrays(GL_TRIANGLES, next, m_num_head_color * 6);
    next += m_num_head_color * 6;

    glUniform4fv(prg.ul_color, 1, glm::value_ptr(PATH_COLOR));
    glDrawArrays(GL_TRIANGLES, next, m_num_path_color * 6);
    next += m_num_path_color * 6;

    glUniform4fv(prg.ul_color, 1, glm::value_ptr(CELL_COLOR));
    glDrawArrays(GL_TRIANGLES, next, m_num_cell_color * 6);
    next += m_num_cell_color * 6;

    if(next != m_quad_vertices.size())
      std::cout << "error, illegal draw " << next << ":" << m_quad_vertices.size() << std::endl;

    // draw lines
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat));
    glBindVertexArray(vaos[LINE_INDEX]);

    glUniform4fv(prg.ul_color, 1, glm::value_ptr(LINE_COLOR));
    glDrawArrays(GL_LINES, 0, m_num_lines * 2);
  }

  void update_buffer()
  {
    m_num_cell_color = 0;
    m_num_path_color = 0;
    m_num_head_color = 0;
    m_num_lines = 0;

    vec2_vector::iterator path_iter = m_quad_vertices.begin();
    vec2_vector::reverse_iterator cell_iter = m_quad_vertices.rbegin();
    m_line_vertices.clear();

    {
      // place head in the very beginning
      vec2 lb = vec2(CELL_WIDTH * current_cell->col, CELL_HEIGHT * current_cell->row);
      vec2 rt = lb + vec2(CELL_WIDTH, CELL_HEIGHT);
      vec2 lt = lb + vec2(0, CELL_HEIGHT);
      vec2 rb = lb + vec2(CELL_WIDTH, 0);
      *path_iter++ = lb;
      *path_iter++ = rt;
      *path_iter++ = lt;
      *path_iter++ = lb;
      *path_iter++ = rb;
      *path_iter++ = rt;
      ++m_num_head_color;
    }


    for (size_t i = 0; i < m_cells.size(); ++i) 
    {
      cell* c = &m_cells[i];
      vec2 lb = vec2(CELL_WIDTH * c->col, CELL_HEIGHT * c->row);
      vec2 rt = lb + vec2(CELL_WIDTH, CELL_HEIGHT);
      vec2 lt = lb + vec2(0, CELL_HEIGHT);
      vec2 rb = lb + vec2(CELL_WIDTH, 0);

      if(c->left_wall)
      {
        m_line_vertices.push_back(lb);
        m_line_vertices.push_back(lt);
        ++m_num_lines;
      }
      if(c->right_wall)
      {
        m_line_vertices.push_back(rb);
        m_line_vertices.push_back(rt);
        ++m_num_lines;
      }
      if(c->bottom_wall)
      {
        m_line_vertices.push_back(lb);
        m_line_vertices.push_back(rb);
        ++m_num_lines;
      }
      if(c->top_wall)
      {
        m_line_vertices.push_back(lt);
        m_line_vertices.push_back(rt);
        ++m_num_lines;
      }

      if(c == current_cell)
        continue;

      if(c->visited)
      {
        // add path from buffer beginning
        ++m_num_path_color;
        *path_iter++ = lb;
        *path_iter++ = rt;
        *path_iter++ = lt;
        *path_iter++ = lb;
        *path_iter++ = rb;
        *path_iter++ = rt;
      }
      else
      {
        // add cell from buffer end in reverse order
        ++m_num_cell_color;
        *cell_iter++ = rt;
        *cell_iter++ = rb;
        *cell_iter++ = lb;
        *cell_iter++ = lt;
        *cell_iter++ = rt;
        *cell_iter++ = lb;
      }
    }

    if(m_num_head_color + m_num_cell_color + m_num_path_color != size())
      std::cout << "illegal buffer : " << m_num_head_color << " "
        << m_num_path_color << " " <<  m_num_cell_color  << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, vbos[QUAD_INDEX]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
        m_quad_vertices.size() * sizeof(vec2), glm::value_ptr(m_quad_vertices.front()));

    glBindBuffer(GL_ARRAY_BUFFER, vbos[LINE_INDEX]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 
        m_num_lines * 2 * sizeof(vec2), glm::value_ptr(m_line_vertices.front()));

    //for (size_t i = 0; i < m_quad_vertices.size(); ++i) {
      //std::cout << m_quad_vertices.at(i) << std::endl;
    //}
    //std::cout << "------------------------"<< std::endl;
    //for (size_t i = 0; i < m_line_vertices.size(); ++i) {
      //std::cout << m_line_vertices.at(i) << std::endl;
    //}
    //std::cout << "------------------------"<< std::endl;
    //std::cout << "------------------------"<< std::endl;

  }

};


class app_name : public app {
protected:
  bitmap_text m_text;
  maze* m_maze = 0;
  cell_stack m_stack;

public:
  ~app_name()
  {
    if(m_maze)
      delete m_maze;
  }
  virtual void init_info() {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    quad0.width(CELL_WIDTH);
    quad0.height(CELL_HEIGHT);
    quad0.build_mesh();

    line_h.build_mesh(vec2(-HALF_CELL_WIDTH, 0), vec2(HALF_CELL_WIDTH, 0));
    line_v.build_mesh(vec2(0, -HALF_CELL_HEIGHT), vec2(0, HALF_CELL_HEIGHT));

    prg.init();
    prg.fix2d_camera(0,  WIDTH, 0, HEIGHT);

    m_maze = new maze(NUM_ROWS, NUM_COLS);

    current_cell = m_maze->get_cell(0);
    current_cell->visited = GL_TRUE;
    m_maze->update_buffer();
  }

  virtual void update() 
  {
    // https://en.wikipedia.org/wiki/Maze_generation_algorithm
    //static GLint num_visited = 1;
    static GLfloat time = STEP_TIME;
    time -= m_delta_time;
    if(time > 0)
      return;

    time += STEP_TIME;
    //if(num_visited == m_maze->size())
      //return;

    cell* neighbour = m_maze->get_unvisited_neighbour(*current_cell);
    if(neighbour != 0)
    {
      m_stack.push(current_cell);
      m_maze->remove_wall(*current_cell, *neighbour);
      current_cell = neighbour;
      current_cell->visited = true;
      m_maze->update_buffer();
    }
    else if(!m_stack.empty())
    {
      current_cell = m_stack.top();
      m_stack.pop();
      m_maze->update_buffer();
    }
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    m_maze->draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << m_fps << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20);
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
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::app_name app;
  app.run();
}
