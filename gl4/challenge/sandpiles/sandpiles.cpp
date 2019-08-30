#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "quad.h"
#include "pingpong.h"
#include <fstream>
#include "stream_util.h"

#define WIDTH 951
#define HEIGHT 951

namespace zxd
{

pingpong tex;
GLuint fbo;
GLint num_sands;
GLuint loops;
quad q;
bool display_help = true;

vec4 color0 = vec4(0, 0, 0, 1);
vec4 color1 = vec4(1, 0, 0, 1);
vec4 color2 = vec4(0, 1, 0, 1);
vec4 color3 = vec4(0, 0, 1, 1);
vec4 color4 = vec4(1, 1, 1, 1);

GLuint createTexture()
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  float zero[4] = {0,0,0,0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, zero);

  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, WIDTH, HEIGHT, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
  return tex;
}

class sandpile_topple_program : public program
{
public:

  GLint ul_diffuse_map;
protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/sandpiles_topple.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_diffuse_map, "diffuse_map");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} prg;

class sandpile_render_program : public program
{
public:

  GLint ul_diffuse_map;
  GLint ul_color0;
  GLint ul_color1;
  GLint ul_color2;
  GLint ul_color3;
  GLint ul_color4;
protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/sandpiles_render.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_color0, "color0");
    uniform_location(&ul_color1, "color1");
    uniform_location(&ul_color2, "color2");
    uniform_location(&ul_color3, "color3");
    uniform_location(&ul_color4, "color4");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} rprg;

class sandpiles_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "sandpiles_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    prg.init();
    rprg.init();

    q.include_texcoord(true);
    q.build_mesh();

    tex.ping(createTexture());
    tex.pong(createTexture());

    num_sands = 1500000;
    glBindTexture(GL_TEXTURE_2D, tex.pong());
    glTexSubImage2D(GL_TEXTURE_2D, 0, WIDTH*0.5, HEIGHT*0.5, 
        1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &num_sands);

    glGenFramebuffers(1, &fbo);

    // topple sand
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, WIDTH, HEIGHT);
    glDisable(GL_DITHER);

    //for (int i = 0; i < 100000; ++i)
    //{
      //tex.swap();
      //glFramebufferTexture2D(
        //GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.pong(), 0);
      //if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        //printf("incomplete frame buffer\n");

      //prg.use();
      //glBindTexture(GL_TEXTURE_2D, tex.ping());
      //glUniform1i(prg.ul_diffuse_map, 0);
      //q.draw();
    //}

  }

  virtual void update() 
  {
    glBindTexture(GL_TEXTURE_2D, tex.pong());
    std::vector<GLuint> pixels;
    pixels.resize(WIDTH*HEIGHT);
    ivec2 pos = ivec2(WIDTH*0.5, HEIGHT*0.5);
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixels.front());
    GLint index = pos.y * WIDTH + pos.x;
    num_sands = pixels[index];

    loops = 100;
    if(num_sands > 40000)
      loops = 10000;
    else if(num_sands > 10000)
      loops = 2500;
    else if(num_sands > 4000)
      loops = 1000;
    else if(num_sands > 1000)
      loops = 250;

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, WIDTH, HEIGHT);
    glDisable(GL_DITHER);
    for (int i = 0; i < loops; ++i)
    {
      tex.swap();
      glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.pong(), 0);
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("incomplete frame buffer\n");

      prg.use();
      glBindTexture(GL_TEXTURE_2D, tex.ping());
      glUniform1i(prg.ul_diffuse_map, 0);
      q.draw();
    }
  }

  virtual void display()
  {

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, wnd_width(), wnd_height());
    glEnable(GL_DITHER);
    rprg.use();

    glUniform4fv(rprg.ul_color0, 1, glm::value_ptr(color0));
    glUniform4fv(rprg.ul_color1, 1, glm::value_ptr(color1));
    glUniform4fv(rprg.ul_color2, 1, glm::value_ptr(color2));
    glUniform4fv(rprg.ul_color3, 1, glm::value_ptr(color3));
    glUniform4fv(rprg.ul_color4, 1, glm::value_ptr(color4));

    glBindTexture(GL_TEXTURE_2D, tex.pong());
    glUniform1i(rprg.ul_diffuse_map, 0);
    q.draw();

    if(!display_help)
      return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps " << m_fps << std::endl;
    ss << "num_sands " << num_sands << std::endl;
    ss << "loops " << loops << std::endl;
    ss << "q : read color from sandpiles_color " << std::endl;
    ss << "h : toggle help " << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y)
  {
    app::glfw_mouse_move(wnd, x, y);
    return;

    glBindTexture(GL_TEXTURE_2D, tex.pong());
    std::vector<GLuint> pixels;
    pixels.resize(WIDTH*HEIGHT);
    ivec2 pos = glfw_to_gl(vec2(x, y));
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixels.front());

    GLint index = pos.y * WIDTH + pos.x;
    pixels[index] += 4;

    glTexSubImage2D(GL_TEXTURE_2D, 0, pos.x, pos.y, 1, 1, GL_RED_INTEGER, GL_UNSIGNED_INT, &pixels[index]);
  }

  void read_color()
  {
    std::ifstream ifs("sandpiles_color");
    if(!ifs)
      return;

    try
    {
      std::string line;

      vec4_vector colors;
      colors.reserve(5);

      while(std::getline(ifs, line))
      {
        std::stringstream ss(line);
        colors.push_back(vec4());
        ss >> colors.back();
      }

      if(colors.size() != 5)
        std::cout << "not enough colors" << std::endl;
      color0 = colors[0]/255.0f;
      color1 = colors[1]/255.0f;
      color2 = colors[2]/255.0f;
      color3 = colors[3]/255.0f;
      color4 = colors[4]/255.0f;
    }catch(std::exception& e)
    {
      std::cout << e.what() << std::endl;
    }

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
        case GLFW_KEY_H:
          display_help ^= 1;
          break;

        case GLFW_KEY_Q:
          read_color();
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
  zxd::sandpiles_app app;
  app.run();
}
