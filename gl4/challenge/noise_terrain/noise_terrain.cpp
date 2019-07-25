#include "app.h"
#include "bitmap_text.h"

#define WIDTH 800
#define HEIGHT 800

#define TERRAIN_WIDTH 100.0f
#define TERRAIN_HEIGHT 100.0f
#define NUM_ROWS 80
#define NUM_COLS 80
#define NOISE_STEP 0.1
#define NOISE_HEIGHT 3.5

namespace zxd
{

GLuint vao;
GLuint vbo;
vec3_vector vertices;

glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 vp_mat;

class program_name : public program
{
public:
  GLint ul_mvp_mat;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/noise_terrain.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/noise_terrain.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
  }

} prg;

class app_name : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    GLfloat row_step = TERRAIN_HEIGHT  / NUM_ROWS;
    GLfloat col_step = TERRAIN_WIDTH / NUM_COLS;

    vec3 start_pos(-row_step * NUM_ROWS * 0.5, -col_step * NUM_COLS * 0.5, 0);
    for (int i = 0; i < NUM_ROWS; ++i)
    {
      GLfloat y1 = i * row_step;
      GLfloat y0 = y1 + row_step;

      for (int j = 0; j <= NUM_COLS; ++j)
      {
        GLfloat x = col_step * j;
        vertices.push_back(vec3(x, y0, 0) + start_pos);
        vertices.push_back(vec3(x, y1, 0) + start_pos);
      }
    }

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), value_ptr(vertices.front()), GL_STREAM_DRAW);

    update_buffer(vec2(0));

    //glVertexAttribPointer( location, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    //glEnableVertexAttribArray(location);

    prg.init();
    v_mat = glm::lookAt(vec3(0, -100, 100), vec3(0), pza);
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
    vp_mat = p_mat * v_mat;
    set_v_mat(&v_mat);
  }

  void update_buffer(vec2 noise_offset)
  {
    vec2 noise_pos = noise_offset;

    float_vector heights;
    heights.reserve((NUM_ROWS+1) * (NUM_COLS+1));

    for (int y = 0; y <= NUM_ROWS; ++y) 
    {
      noise_pos.y += NOISE_STEP;
      noise_pos.x = noise_offset.x;
      for (int x = 0; x <= NUM_COLS; ++x) 
      {
        noise_pos.x += NOISE_STEP;
        heights.push_back(NOISE_HEIGHT * glm::perlin(noise_pos));
      }
    }

    GLint row_size = (NUM_COLS + 1);

    for (int y = 0; y < NUM_ROWS; ++y) 
    {
      GLint row_start = row_size * 2 * y;
      for (int x = 0; x <= NUM_COLS; ++x) 
      {
        vertices.at(row_start + x*2).z = heights[row_size * y + row_size + x];
        vertices.at(row_start + x*2 + 1).z = heights[row_size * y + x];
      }
    }


    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(vec3), glm::value_ptr(vertices.front()));
  }

  virtual void update() 
  {
    update_buffer(vec2(m_current_time * 0.25, m_current_time * 0.25));
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    prg.use();
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(vp_mat));

    GLint next = 0;
    GLint count = (NUM_COLS + 1) * 2;
    for (int i = 0; i < NUM_ROWS; ++i)
    {
      glDrawArrays(GL_TRIANGLE_STRIP, next, count);
      next += count;
    }
    
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_text.print("noise terrain", 10, wnd_height()- 20);
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
  zxd::app_name app;
  app.run();
}
