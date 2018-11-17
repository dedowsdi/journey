#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include <sstream>
#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

#define WIDTH 800.0f
#define HEIGHT 800.0f
#define RADIUS 20

namespace zxd {

struct points
{
  GLuint vao;
  GLuint vbo;
  GLuint draw_count = 1;
  GLuint buffer_count = 1;

  void init()
  {
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, buffer_count * sizeof(vec2), 0, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  void update_buffer(const vec2_vector& vertices)
  {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    draw_count = vertices.size();
    if(vertices.size() <= buffer_count)
    {
      glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vec2), glm::value_ptr(vertices.front()));
    }
    else
    {
      buffer_count = vertices.size();
      glBufferData(GL_ARRAY_BUFFER, buffer_count * sizeof(vec2), glm::value_ptr(vertices.front()), GL_STATIC_DRAW);
    }
  }

  void draw()
  {
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, draw_count);
  }

}geometry;

struct dot_program : public program
{
  GLint ul_color;
  void attach_shaders() override
  {
    attach(GL_VERTEX_SHADER, "shader/dot.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader/dot.fs.glsl");
  }

  void bind_attrib_locations() override
  {
    bind_attrib_location(0, "vertex");
  }

  void bind_uniform_locations() override
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_color, "color");
  }
}prg;

// generate uniformly distributed sample in the input extent, no samples exists
// in radius r of any sample.
std::vector<vec2> poisson_disk_sample2(const vec2& min_ext, const vec2& max_ext, GLfloat radius, GLubyte k = 30)
{
  typedef std::vector<int> int_grid;
  std::vector<vec2> samples;
  std::vector<int> actives;
  int_grid grid;

  // step0 : Initialize an n-dimensional background grid for storing samples and
  // accelerating spatial searches. We pick the cell size to  be bounded by
  // r/sqrt(N), so that each grid cell will contain at most one sample
  vec2 ext = max_ext - min_ext;
  GLfloat cell_size = radius / glm::sqrt(2.0);
  GLuint rows = glm::ceil(ext.y / cell_size);
  GLuint cols = glm::ceil(ext.x / cell_size);
  grid.assign(rows*cols, -1);
  actives.reserve(grid.size());
  samples.reserve(grid.size());
  std::cout << "init cell size as " << cell_size << ", init " << grid.size() << " grid cells" << std::endl;

  // step1 : Select the initial sample, s0 , randomly chosen uniformly from the
  // domain. Insert it into the background grid, and initialize the “active
  // list”
  vec2 s0 = glm::linearRand(min_ext, max_ext);
  samples.push_back(s0);
  ivec2 cr0 = (s0 - min_ext) / cell_size;
  grid[cr0.y*cols + cr0.x] = 0;
  actives.push_back(0);
  std::cout << "create first sample " << s0 << std::endl;


  // step 2 : While the active list is not empty, choose a random active_index from it
  // (say i). Generate up to k points chosen uniformly from the spherical
  // annulus between radius r and 2r around x i . For each point in turn, check
  // if it is within distance r of existing samples (using the background grid
  // to only test nearby samples). If a point is adequately far from existing
  // samples, emit it as the next sample and add it to the active list. If after
  // k attempts no such point is found, instead remove i from the active list.
  GLint neighbour_radius = glm::ceil(sqrt(2));
  GLfloat two_radius = radius * 2;
  GLfloat radius2 = radius * radius;
  while(!actives.empty())
  {
    GLuint active_index = glm::linearRand(0.0, 1.0) * actives.size();
    const vec2& sample = samples[actives[active_index]];
    bool new_sample_emitted = false;
    for (GLubyte kill = k; kill > 0; --kill) {
      vec2 new_sample = sample + glm::circularRand(1.0f) * glm::linearRand(radius, two_radius);
      while(new_sample.x < min_ext.x || new_sample.y < min_ext.y || new_sample.x > max_ext.x || new_sample.y > max_ext.y)
         new_sample = sample + glm::circularRand(1.0f) * glm::linearRand(radius, two_radius);
      GLint row = (new_sample.y - min_ext.y) / cell_size;
      GLint col = (new_sample.x - min_ext.x) / cell_size;

      // check neighbour of new sample to make sure there has no sample in r
      // radius
      bool valid_new_sample = true;
      for (int i = -neighbour_radius; i <= neighbour_radius && valid_new_sample; ++i) {

        GLint nb_row = row + i;
        if(nb_row < 0 || nb_row >= rows)
          continue;

        for (int j = -neighbour_radius; j <= neighbour_radius; ++j) {
          GLint nb_col = col + j;
          if(nb_col < 0 || nb_col >= cols)
            continue;

          GLuint neighbour_sample_index = grid[nb_row * cols + nb_col];
          if(neighbour_sample_index != -1 && glm::length2(samples[neighbour_sample_index] - new_sample) <= radius2)
          {
            valid_new_sample = false;
            break;
          }
        }
      }

      if(!valid_new_sample)
        continue;

      // debug check
      //for (int i = 0; i < samples.size(); ++i) {
        //if(glm::length2(new_sample - samples[i]) <= radius2)
        //{
          //GLint c0 = (new_sample.x - min_ext.x) / cell_size;
          //GLint r0 = (new_sample.y - min_ext.y) / cell_size;
          //GLint c1 = (samples[i].x - min_ext.x) / cell_size;
          //GLint r1 = (samples[i].y - min_ext.y) / cell_size;
          //std::cout << "found illegal new_sample " << new_sample  << " " << r0 << ":" << c0 
          //<< ", it's too close to " 
          //<< samples[i]  << " " << r1 << ":" << c1 
          //<< ", it's grid value is " << grid[r1 * cols + c1]
          //<< std::endl;
        //}
      //}

      GLuint new_sample_index = row * cols + col;
      grid[new_sample_index] = samples.size();
      actives.push_back(samples.size());
      samples.push_back(new_sample);
      new_sample_emitted = true;
      if(samples.size() > grid.size())
        throw std::runtime_error("something wrong happened, sample size should not be bigger than gird size");
      break;
    }

    if(!new_sample_emitted)
      actives.erase(actives.begin() + active_index);
  }

  std::cout << "generate " << samples.size() << " samples" << std::endl;
  return samples;
};

class app_name : public app {
protected:
  bitmap_text m_text;
  vec2_vector m_vertices;

public:
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

    geometry.init();
    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    reset_samples();
  }

  void reset_samples()
  {
    m_vertices = poisson_disk_sample2(vec2(0), vec2(WIDTH,HEIGHT), RADIUS);
    geometry.update_buffer(m_vertices);
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat));
    glUniform4f(prg.ul_color, 1, 1, 1, 1);
    glPointSize(RADIUS); // point radius is RADIUS/2
    geometry.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : regenerate possion disk sample";
    m_text.print(ss.str(), 10, m_info.wnd_height - 20, vec4(1, 0, 0, 1));
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
          reset_samples();
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
