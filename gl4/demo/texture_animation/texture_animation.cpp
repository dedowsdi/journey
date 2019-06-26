#include "app.h"
#include "bitmap_text.h"
#include "quad.h"
#include "texutil.h"
#include "common.h"

#define WIDTH 800
#define HEIGHT 800

#define ANIM_COOLDOWN (1/24.0)

namespace zxd
{

quad quad0;

glm::mat4 v_mat;
glm::mat4 p_mat;

class texture_animation_program : public program
{

public:
  GLint al_vertex;
  GLint al_texcoord;
  GLint ul_diffuse_map;
  GLint ul_tex_mat;
  GLint ul_mvp_mat;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/texture_animation.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/texture_animation.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
    uniform_location(&ul_tex_mat, "tex_mat");
  }

  void bind_attrib_locations()
  {
    al_vertex = attrib_location("vertex");
    al_texcoord = attrib_location("texcoord");
  }

} prg;

class texture_animation_app : public app
{
protected:
  GLuint m_texture;
  mat4 m_tex_mat;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "texture_animation_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    prg.init();
    p_mat = glm::ortho<GLfloat>(-WIDTH*.5, WIDTH*.5, -HEIGHT*.5, HEIGHT*.5, -1, 1);

    quad0.include_texcoord(GL_TRUE);
    quad0.build_mesh();

    fipImage fi = fipLoadResource("texture/explosion.jpg");

    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fi.getWidth(), fi.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE, fi.accessPixels());

  }

  virtual void update()
  {
    static GLfloat anim_cooldown = 0;
    static GLint tex_index = -1;

    anim_cooldown -= m_delta_time;
    if(anim_cooldown < 0)
    {
      anim_cooldown += ANIM_COOLDOWN;
      tex_index = ++tex_index % 16;

      GLfloat tw = 1.0/4;
      GLint row =  tex_index / 4;
      GLint col = tex_index % 4;
      m_tex_mat = glm::translate(vec3(col * tw, row * tw, 0)) *  glm::scale(vec3(tw));
    }

  }

  virtual void display()
  {

    glClear(GL_COLOR_BUFFER_BIT);
    prg.use();

    glBindTexture(GL_TEXTURE_2D, m_texture);

    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat * glm::scale(vec3(100.0))));
    glUniformMatrix4fv(prg.ul_tex_mat, 1, 0, glm::value_ptr(m_tex_mat));
    glUniform1i(prg.ul_diffuse_map, 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
    quad0.draw();
    glDisable(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_text.print("", 10, HEIGHT - 20);
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
  zxd::texture_animation_app app;
  app.run();
}
