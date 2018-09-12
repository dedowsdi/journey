#include "app.h"
#include "program.h"
#include <algorithm>
#include <list>

#define WIDTH 800.0f
#define HEIGHT 800.0f
#define CELL_SIZE 80
#define HALF_SIZE CELL_SIZE*0.5
#define CX_CELL WIDTH/CELL_SIZE
#define CY_CELL HEIGHT/CELL_SIZE
#define COOLDOWN 0.15

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))
#define COLOR vec3(1)

namespace  zxd
{

// geometry
class rect
{
public:

  GLuint vao;
  GLuint vbo;
  GLfloat size;

  vec2_vector vertices;

  rect(GLfloat s):
    size(s)
  {
  };

  void build_mesh()
  {
    GLfloat hz = size * 0.5 * 0.9;
    vertices.push_back(vec2(-hz, hz));
    vertices.push_back(vec2(-hz, -hz));
    vertices.push_back(vec2(hz, hz));
    vertices.push_back(vec2(hz, -hz));
    
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec2),
      value_ptr(vertices[0]), GL_STATIC_DRAW);

  }

  void bind(GLint vertex_location)
  {
    glVertexAttribPointer(vertex_location, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(vertex_location);
  }

  void draw()
  {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  }
} geometry(CELL_SIZE);

struct snake_program : public program
{
public:
  GLint ul_color;
  GLint al_vertex;

protected:
  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/snake.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/snake.fs.glsl");
  };
  virtual void bind_uniform_locations()
  {
    uniform_location(&ul_color, "color");
    uniform_location(&ul_mvp_mat, "mvp_mat");
  };
  virtual void bind_attrib_locations()
  {
    al_vertex = attrib_location("vertex");
  };
} prg;

struct cell
{
  cell(){}
  cell(const vec2& p):
    gpos(p)
  {
  }
  ivec2 gpos; //grid position
};

typedef std::vector<cell> cell_vector;

class snake
{
protected:
  GLboolean m_eating;
  cell_vector m_body;
  cell m_cell;        // newly spawned cell
  cell m_eating_cell; // current eating cell
  GLfloat m_cool_down;
  ivec2 m_dir;
  ivec2 m_next_dir;

public:

  snake()
  {
  }

  void next_dir(const ivec2& v)
  {
    m_next_dir = v;
  }

  void reset()
  {
    std::cout << "reset" << std::endl;
    m_eating = GL_FALSE;
    m_cool_down = COOLDOWN;

    m_body.clear();
    vec2 pos(5,1);
    m_body.push_back(cell(pos));
    pos.x -= 1;
    m_body.push_back(cell(pos));
    pos.x -= 1;
    m_body.push_back(cell(pos));
    pos.x -= 1;
    m_body.push_back(cell(pos));
    m_next_dir = vec2(1, 0);

    spawn_cell();
  }

  cell& head()
  {
    return m_body.front();
  }

  void update(GLfloat dt)
  {
    m_cool_down -= dt;
    if(m_cool_down > 0)
      return;

    m_cool_down = COOLDOWN;

    if(m_next_dir + m_dir != ivec2(0))
      m_dir = m_next_dir;

    if(death())
    {
      reset();
      return;
    }

    // digest to tail
    if(m_eating)
    {
      m_eating = GL_FALSE;
      m_body.push_back(m_eating_cell);
    }

    // shift forward
    for(auto iter = m_body.rbegin(); iter != m_body.rend() - 1; ++iter)
    {
      iter->gpos = (iter+1)->gpos;
    }

    head().gpos += m_dir;

    eat();
  }

  // dead or not in the forth coming move
  bool death()
  {
    ivec2 next_pos = head().gpos + m_dir;
    if(next_pos.x < 0 || next_pos.x >= CX_CELL || next_pos.y < 0 || next_pos.y >= CY_CELL)
    {
      std::cout << "hit border" << std::endl;
      return true;
    }

    if(hit(next_pos, m_body.begin()+1, m_body.end()-1))
    {
      std::cout << "hit self" << std::endl;
      return true;
    }

    return false;
  }

  void draw()
  {
    prg.use();
    geometry.bind(prg.al_vertex);
    glUniform3fv(prg.ul_color, 1, glm::value_ptr(COLOR));

    // draw snake
    for (size_t i = 0; i < m_body.size(); ++i) 
    {
      const cell& c = m_body[i];
      vec2 translation = (vec2(c.gpos) + vec2(0.5)) * vec2(CELL_SIZE);
      mat4 m_mat = glm::translate(vec3(translation, 0.0));
      mat4 mvp_mat = prg.vp_mat * m_mat;
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));

      geometry.draw();
    }

    // draw discrete cell
    vec2 translation = (vec2(m_cell.gpos) + vec2(0.5)) * vec2(CELL_SIZE);
    mat4 m_mat = glm::translate(vec3(translation, 0.0));
    mat4 mvp_mat = prg.vp_mat * m_mat;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    geometry.draw();
  }

  void spawn_cell()
  {
    if(m_body.size() == CX_CELL * CY_CELL)
      return;

    // gather empty area
    std::list<ivec2> pool;
    for (size_t i = 0; i < CY_CELL; ++i) {
      for (size_t j = 0; j < CX_CELL; ++j) {
        pool.push_back(ivec2(j, i)); // store (col, row) as (x, y)
      }
    }
    
    // dig out grids occupied by snake
    std::vector<GLuint> garbage;
    for (size_t i = 0; i < m_body.size(); ++i) 
    {
      GLuint index = m_body[i].gpos.y * CY_CELL + m_body[i].gpos.x; 
      garbage.push_back(index);
    }
    std::sort(garbage.begin(), garbage.end());

    for(auto iter = garbage.rbegin(); iter != garbage.rend(); ++iter){
      auto i = pool.begin();
      std::advance(i, *iter);
      pool.erase(i);
    }

    auto iter = pool.begin(); std::advance(iter, glm::linearRand(0.0, 1.0) * pool.size());
    m_cell.gpos = *iter;
    std::cout << "spawn cell at " << m_cell.gpos.x << ":" << m_cell.gpos.y << std::endl;
  }

  void eat()
  {
    if(m_cell.gpos == head().gpos)
    {
      m_eating = true;
      m_eating_cell = m_cell;
      spawn_cell();
    }
  }

  bool hit(const ivec2& pos, cell_vector::iterator beg, cell_vector::iterator end)
  {
    while(beg != end)
    {
      if((*beg).gpos == pos)
        return true;
      ++beg;
    }

    return false;
  }

};

class snake_app : public app
{
protected:
  snake m_snake;

  void init_info()
  {
    app::init_info();
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  void create_scene()
  {

    glClearColor(0.15, 0.15, 0.15, 1);

    geometry.build_mesh();
    prg.init();

    prg.vp_mat = glm::ortho(0.0f, WIDTH, 0.0f, HEIGHT, -1.0f, 1.0f);

    m_snake.reset();
  }

  void update()
  {
    m_snake.update(m_delta_time);
  }

  void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);
    m_snake.draw();
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods)
  {
    if(action == GLFW_PRESS)
    {
      switch(key)
      {
        case GLFW_KEY_J:
          m_snake.next_dir(vec2(-1, 0));
          break;
        case GLFW_KEY_L:
          m_snake.next_dir(vec2(1, 0));
          break;
        case GLFW_KEY_K:
          m_snake.next_dir(vec2(0, -1));
          break;
        case GLFW_KEY_I:
          m_snake.next_dir(vec2(0, 1));
          break;
      }
    }

    app::glfw_key(wnd, key, scancode, action, mods);
  }

};

}

int main(int argc, char *argv[])
{
  zxd::snake_app app;
  app.run();
  return 0;
}
