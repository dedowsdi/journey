#include <sstream>
#include <algorithm>
#include <numeric>

#include "app.h"
#include "bitmap_text.h"
#include "timer.h"

#define WIDTH 800
#define HEIGHT 800


namespace zxd
{

std::vector<GLfloat> input_data;
GLint num_data = 1024*1024;

timer clock;
GLuint buffers[2];

class prefix_sum_1d_program : public program
{
public:

protected:

  void attach_shaders()
  {
    attach(GL_COMPUTE_SHADER, "shader4/prefix_sum_1d.cs.glsl");
  }

  void bind_uniform_locations()
  {
  }

  void bind_attrib_locations()
  {
  }

} prg;

class prefix_sum_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "prefix_sum_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);



    input_data.reserve(num_data);
    for (int i = 0; i < num_data; ++i)
    {
      //input_data.push_back(glm::linearRand(0.0f, 100.0f));
      input_data.push_back(i);
    }

    clock.reset();
    std::vector<GLfloat> result;
    result.reserve(num_data);
    std::partial_sum(input_data.begin(), input_data.end(), std::back_inserter(result));
    std::cout << "cpu took " << clock.time_miliseconds() << "ms to partial sum "
              << num_data << " floats" << std::endl;;

    prg.init();

    glGenBuffers(2, buffers);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[0]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, input_data.size() * 4, &input_data.front(), GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, buffers[1]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, num_data *  4, 0, GL_DYNAMIC_COPY);

    glShaderStorageBlockBinding(prg.get_object(), 0, 0);
    glShaderStorageBlockBinding(prg.get_object(), 1, 1);

    prg.use();

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffers[0], 0, 4 * num_data);
    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 1, buffers[1], 0, 4 * num_data);

    glFinish();
    clock.reset();
    glDispatchCompute(1024, 1, 1);
    glMemoryBarrier(GL_SHADER_STORAGE_BUFFER_BINDING);
    glFinish();
    std::cout << "gpu took " << clock.time_miliseconds() << "ms to partial sum "
              << num_data << " floats" << std::endl;;

    glBindBufferRange(GL_SHADER_STORAGE_BUFFER, 0, buffers[1], 0, num_data * 4);
    float* ptr = static_cast<GLfloat*>(glMapBufferRange(
          GL_SHADER_STORAGE_BUFFER, 0, 4*num_data, GL_MAP_READ_BIT));

    //for (int i = 0; i < num_data; ++i) 
    //{
      //std::cout << *ptr++ << std::endl;
    //}
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);

  }

  virtual void update() {}

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);


    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
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
  zxd::prefix_sum_app app;
  app.run();
}
