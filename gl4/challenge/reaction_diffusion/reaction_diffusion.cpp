#include "app.h"
#include "bitmaptext.h"
#include "quad.h"
#include <limits>
#include <sstream>
#include "common_program.h"

#define WIDTH 512
#define HEIGHT 512
#define TEX_WIDTH WIDTH*1
#define TEX_HEIGHT HEIGHT*1

namespace zxd {

quad q;

// ping pong
GLuint tex[2];
GLuint fbo[2];

GLfloat da = 1;
GLfloat db = 0.5;
GLfloat feed = 0.055;
GLfloat kill = 0.062;
GLfloat delta_time = 1;

std::vector<u16vec2> create_seed()
{
  std::vector<u16vec2> pixels;
  pixels.reserve(WIDTH*HEIGHT);


  GLfloat hw = TEX_WIDTH/2;
  GLfloat hh = TEX_HEIGHT/2;

  for (int i = 0; i < TEX_HEIGHT; ++i) 
  {
    for (int j = 0; j < TEX_WIDTH; ++j) 
    {
      u16vec2 pixel(-1, 0);
      if( i >= hh - 50 && i <= hh + 50 && j >= hw - 50 && j <= hw + 50)
      {
        pixel = u16vec2(-1, -1);
      }
      pixels.push_back(pixel);
    }
  }

  return pixels;
};

class reaction_diffusion_program : public program
{

public:
  GLint al_vertex;
  GLint al_texcoord;
  GLint ul_diffuse_map;
  GLint ul_da;
  GLint ul_db;
  GLint ul_feed;
  GLint ul_kill;
  GLint ul_delta_time;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/reaction_diffusion.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/reaction_diffusion.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_da        ,  "da");
    uniform_location(&ul_db        ,  "db");
    uniform_location(&ul_feed      ,  "feed");
    uniform_location(&ul_kill      ,  "kill");
    uniform_location(&ul_delta_time,  "delta_time");
  }

  void bind_attrib_locations()
  {
    al_vertex = attrib_location("vertex");
    al_texcoord = attrib_location("texcoord");
  }
} prg;

class reaction_diffusion_draw_program : public program
{

public:
  GLint al_vertex;
  GLint al_texcoord;
  GLint ul_quad_map;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/reaction_diffusion_draw.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/reaction_diffusion_draw.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_quad_map, "quad_map");
  }

  void bind_attrib_locations()
  {
    al_vertex = attrib_location("vertex");
    al_texcoord = attrib_location("texcoord");
  }
} prg1;

class app_name : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "reaction diffusion";
    m_info.wnd_width = 0;
    m_info.wnd_height = 0;
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    //m_info.fullscreen = GL_TRUE;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    prg.init();
    prg1.init();

    q.include_texcoord(true);
    q.build_mesh();

    glGenFramebuffers(2, fbo);
    glGenTextures(2, tex);

    std::vector<u16vec2> pixels = create_seed();

    for (int i = 0; i < 2; ++i) 
    {
      glBindTexture(GL_TEXTURE_2D, tex[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
      
      //glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16, WIDTH, HEIGHT, 0, GL_RG, GL_UNSIGNED_BYTE, 0);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16,
          TEX_WIDTH, TEX_HEIGHT, 0, GL_RG, GL_UNSIGNED_SHORT, glm::value_ptr(pixels.front()));

      glBindFramebuffer(GL_FRAMEBUFFER, fbo[i]);
      glFramebufferTexture2D(
        GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex[i], 0);
      if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("incomplete frame buffer%i\n", fbo[i]);
    }

  }

  virtual void update() {}

  virtual void display() {

    static GLuint pp = 0; // use 0 as texture, 1 as fbo in the beginning

    GLuint tex_index = pp; // texture 
    pp ^= 1;  // framebuffer

    // update texture
    glBindFramebuffer(GL_FRAMEBUFFER, fbo[pp]);
    glBindTexture(GL_TEXTURE_2D, tex[tex_index]);
    glViewport(0, 0, TEX_WIDTH, TEX_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();

    q.bind_vt(prg.al_vertex, prg.al_texcoord);
    glUniform1i(prg.ul_diffuse_map, 0);
    glUniform1f(prg.ul_da        ,  da);
    glUniform1f(prg.ul_db        ,  db);
    glUniform1f(prg.ul_feed      ,  feed);
    glUniform1f(prg.ul_kill      ,  kill);
    glUniform1f(prg.ul_delta_time,  delta_time);

    q.draw();

    prg1.use();
    // render to screen
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, WIDTH, HEIGHT);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, tex[pp]);
    glUniform1i(prg1.ul_quad_map, 0);

    q.bind_vt(prg.al_vertex, prg.al_texcoord);
    q.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << m_fps;
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
