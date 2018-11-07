// https://web.archive.org/web/20160418004150/http://freespace.virgin.net/hugo.elias/models/m_fire.htm


#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "quad.h"
#include "texutil.h"
#define RECORD

GLint width;
GLint height;
GLuint fbo;
GLuint heat_map;
GLuint fire_map[2]; // pingpong read write
GLuint fire_current = 0; // write
GLuint cool_map[2];
GLuint cool_current = 0; // write
GLuint cool_smooth_time = 10;
GLuint cool_spot = 500;
bool display_cool_map = false;
bool warp = true;
fipImage img;

namespace zxd {

GLuint fireSourceRows = 5;
GLfloat fireSourceChance = 0.5;
quad q;

GLuint createTexture(void* data = 0)
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  //float black[4] = {0,0,0,0};
  //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, black);
  
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_BGRA, GL_UNSIGNED_BYTE, data);
  return tex;
}

class program_name : public program
{
public:
  GLint ul_fire_map;
  GLint ul_cool_map;
  GLint ul_time;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/fire.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/fire.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_fire_map, "fire_map");
    uniform_location(&ul_cool_map, "cool_map");
    uniform_location(&ul_time, "time");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} prg;

class cool_map_program : public program
{
public:
  GLint ul_cool_map;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/fire.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/coolmap.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_cool_map, "cool_map");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} cprg;

class warp_program : public program
{
public:
  GLint ul_time;
  GLint ul_diffuse_map;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/warp.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_time, "time");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} wprg;

class heat_program : public program
{
public:
  GLint ul_time;
  GLint ul_diffuse_map;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/quad.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/fire_heat.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_time, "time");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} hprg;

class fire_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "fire_app";
    m_info.samples = 4;
    width = img.getWidth();
    height = img.getHeight();
#ifdef RECORD
    m_info.wnd_width = 480;
    m_info.wnd_height = m_info.wnd_width * static_cast<GLfloat>(height)/width;
    m_info.decorated = GL_FALSE;
    std::cout << "wnd width : " << m_info.wnd_width << std::endl;
    std::cout << "wnd height : " << m_info.wnd_height << std::endl;
#else
    m_info.wnd_width = width;
    m_info.wnd_height = height;
#endif
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    glfwSetWindowPos(m_wnd, 100, 100);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    cprg.init();
    wprg.init();
    hprg.init();
    q.include_texcoord(true);
    q.build_mesh();

    fire_map[0] = createTexture();
    fire_map[1] = createTexture();

    glGenFramebuffers(1, &fbo);

    cool_map[0] = createTexture();
    cool_map[1] = createTexture();

    heat_map = createTexture(img.accessPixels());

    reset_cool_map();
  }

  void reset_cool_map()
  {
    float_vector sources;
    sources.resize(width*height, 0);
    for (int i = 0; i < cool_spot; ++i) {
      GLint index = glm::linearRand(0, width*height - 1);
      sources[index] = 1;
    }

    glBindTexture(GL_TEXTURE_2D, cool_map[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_FLOAT, &sources.front());

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);
    cprg.use();
    glUniform1i(cprg.ul_cool_map, 0);

    cool_current = 0;
    for (int i = 0; i < cool_smooth_time; ++i) {
      GLuint tex = cool_current;
      cool_current = ++cool_current % 2;

      glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cool_map[cool_current], 0);
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("incomplete frame buffer\n");

      glBindTexture(GL_TEXTURE_2D, cool_map[tex]);
      q.draw();
    }
  }

  virtual void update() {
    feed_fire_source();
  }

  void feed_fire_source()
  {
    fire_current = ++fire_current % 2;

    hprg.use();
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, fire_map[(fire_current + 1)%2], 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, heat_map);
    glUniform1i(hprg.ul_diffuse_map, 0);
    glUniform1f(hprg.ul_time, m_current_time);
    q.draw();

    // create fire sorce
    //float_vector sources;
    //for (int y = 0; y < fireSourceRows; ++y) 
    //{
      //for (int x = 0; x < width; ++x) 
      //{
        ////float heat = glm::linearRand(0.2, 1.0);
        //float heat = glm::mix(0.6, 1.0, glm::perlin(vec2(x*0.03, y*0.03) + vec2(0, m_current_time)));
        //sources.push_back(heat);
      //}
    //}
    //if(!sources.empty())
    //{
      //glBindTexture(GL_TEXTURE_2D, fire_map[(fire_current + 1)%2]);
      //glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, fireSourceRows, 
          //GL_RED, GL_FLOAT, &sources.front());
    //}
  }

  virtual void display() {

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glViewport(0, 0, width, height);

    for (int i = 0; i < 3; ++i) {
      feed_fire_source();

      glFramebufferTexture2D(
          GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fire_map[fire_current], 0);
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("incomplete frame buffer\n");
      prg.use();

      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, fire_map[(fire_current + 1)%2]);
      glActiveTexture(GL_TEXTURE1);
      glBindTexture(GL_TEXTURE_2D, cool_map[cool_current]);

      glUniform1i(prg.ul_fire_map, 0);
      glUniform1i(prg.ul_cool_map, 1);
      glUniform1f(prg.ul_time, m_current_time);
      q.draw();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_info.wnd_width, m_info.wnd_height);

    if(display_cool_map)
      draw_quad(cool_map[cool_current], 0);
    else
    {
      if(warp)
      {
        wprg.use();
        glUniform1i(wprg.ul_diffuse_map, 0);
        glBindTexture(GL_TEXTURE_2D, fire_map[fire_current]);
        q.draw();
      }
      else
      {
        draw_quad(fire_map[fire_current], 0);
      }
    }

    return;
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : display cool map : " << display_cool_map << std::endl;
    ss << "w : warp : " << warp << std::endl;
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
        case GLFW_KEY_Q:
          display_cool_map = !display_cool_map;
          break;
        case GLFW_KEY_W:
          warp = !warp;
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
  img = zxd::fipLoadImage32("data/texture/ren.jpg");
  zxd::fire_app app;
  app.run();
}
