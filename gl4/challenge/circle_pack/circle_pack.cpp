#include <sstream>
#include <algorithm>
#include <list>

#include <app.h>
#include <bitmap_text.h>
#include <common_program.h>
#include <texutil.h>
#include <quad.h>
#include <circle.h>

namespace zxd
{

using vec2_list = std::list<vec2>;

GLint spawn_per_frame = 5;
GLuint max_circle = 5000;
GLuint tex;
GLfloat grow_step = 0.1;
vec3_vector pixels;
vec2_list black_pixels; //position
fipImage img;
quad q;
mat4_vector m_mats;
GLuint vbo;
circle geometry;

glm::mat4 v_mat;
glm::mat4 p_mat;

lightless_program prg0; // picture prg
lightless_program prg1; // circle prg

class auto_grow_circle;
typedef std::vector<auto_grow_circle> circle_vector;
// grow until collide with another auto_grow_circle or border
class auto_grow_circle
{
protected:
  bool m_growing = true;
  GLfloat m_radius = 0.01;
  GLfloat m_growing_step = 0.1;
  vec2 m_pos;
public:

  auto_grow_circle(const vec2& pos):
    m_pos(pos)
  {
  }

  bool growing() const { return m_growing; }
  void growing(bool v){ m_growing = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }

  GLfloat growing_step() const { return m_growing_step; }
  void growing_step(GLfloat v){ m_growing_step = v; }

  void grow()
  {
    if(!m_growing)
      return;

    m_radius += m_growing_step;
  }

  bool collide(const vec2& min_ext, const vec2& max_ext) const
  {
    return (m_pos.x + m_radius) > max_ext.x || (m_pos.x - m_radius) < min_ext.x ||
       (m_pos.y + m_radius) > max_ext.y || (m_pos.y - m_radius) < min_ext.y;
  }

  bool collide(const auto_grow_circle& c) const
  {
    GLfloat a = m_radius + c.radius();
    return glm::length2(m_pos - c.pos()) <= a*a;
  }

  bool collide(const vec2& point) const
  {
    return glm::length2(m_pos - point) <=  m_radius * m_radius;
  }

  mat4 m_mat()
  {
    return glm::scale(glm::translate(vec3(m_pos, 0)), vec3(m_radius, m_radius, 1));
  }
};


class circle_pack_app : public app
{
protected:
  circle_vector m_circles;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "circle_pack_app";
    m_info.wnd_width = img.getWidth();
    m_info.wnd_height = img.getHeight();
  }

  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    prg0.with_texcoord = true;
    prg0.init();
    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());

    prg1.instance = true;
    prg1.init();
    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());

    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, img.accessPixels());
    glGenerateMipmap(GL_TEXTURE_2D);

    pixels.resize(img.getWidth() * img.getHeight());
    glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_FLOAT, &pixels.front());

    // collect black pixels
    for (int y = 0; y < img.getHeight(); ++y) 
    {
      int row_start = y * img.getWidth();
      for (int x = 0; x < img.getWidth(); ++x) 
      {
        int index = row_start + x;
        const vec3& pixel = pixels[index];
        if(pixel.x < 0.5)
          black_pixels.push_back(vec2(x, y));
      }
    }
    std::cout << "collect " << black_pixels.size() << " black pixels" << std::endl;

    q.setup(0, 0, wnd_width(), wnd_height());
    q.build_mesh({attrib_semantic::vertex, attrib_semantic::texcoord});

    geometry.slice(64);
    geometry.type(circle::LINE);
    geometry.build_mesh();

    // create additonal instance attribute
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    matrix_attrib_pointer(2);
  }

  auto_grow_circle create_circle()
  {
    vec2 pos;
    while(!black_pixels.empty())
    {
      int index = glm::linearRand(0.0f, static_cast<GLfloat>(black_pixels.size()));
      auto iter = black_pixels.begin();
      std::advance(iter, index);
      pos = *iter;

      bool collide = std::any_of(m_circles.begin(), m_circles.end(), 
          [&pos](const auto& item)->bool
          {
            return item.collide(pos);
          });
      
      black_pixels.erase(iter);

      if(!collide)
        break;
    }
    auto_grow_circle c(pos);
    c.growing_step(grow_step);
    return c;
  }
  
  virtual void update()
  {
    if(m_circles.size() < max_circle)
    {
      for (int i = 0; i < spawn_per_frame; ++i)
      {
        m_circles.push_back(create_circle());
      }

      if(!black_pixels.empty())
        std::cout << black_pixels.size() << " black pixels left" << std::endl;
    }


    bool dirty = false;
    for (int i = 0; i < m_circles.size(); ++i)
    {
      auto_grow_circle& c0 = m_circles[i];
      if(!c0.growing())
        continue;

      dirty = true;
      c0.grow();
      // border collide test
      if(c0.collide(vec2(0), vec2(wnd_width(), wnd_height())))
      {
        c0.growing(false);
        continue;
      }

      // collide test between circles
      for (int j = 0; j < m_circles.size(); ++j) 
      {
        if(i == j)
          continue;

        if(c0.collide(m_circles[j]))
        {
          c0.growing(false);
        }
      }

    }

    if(dirty)
      update_instance_buffer();
  }
  void update_instance_buffer()
  {
    m_mats.resize(m_circles.size());
    for (int i = 0; i < m_circles.size(); ++i)
    {
      m_mats[i] = m_circles[i].m_mat();
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_mats.size()*sizeof(mat4), glm::value_ptr(m_mats.front()), GL_DYNAMIC_DRAW);

    geometry.get_primitive_set(0).num_instance(m_circles.size());
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg0.use();
    glUniformMatrix4fv(prg0.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat));
    glUniform1i(prg0.ul_diffuse_map, 0);
    glBindTexture(GL_TEXTURE_2D, tex);
    //q.draw();

    prg1.use();
    //glUniform4fv(prg1.ul_color, 1,  glm::value_ptr(vec4(1)));
    //for (int i = 0; i < m_circles.size(); ++i)
    //{
      //mat4 mvp_mat = prg1.vp_mat * m_circles[i].m_mat();
      //glUniformMatrix4fv(prg1.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
      //geometry.draw();
    //}

    glUniformMatrix4fv(prg1.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat));
    glUniform4fv(prg1.ul_color, 1,  glm::value_ptr(vec4(1)));
    geometry.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps " << m_fps << std::endl;
    ss << "num circles " << m_circles.size() << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20, vec4(0,0,0,1));
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

  zxd::img = zxd::fipLoadResource32("texture/z.png");

  zxd::circle_pack_app app;
  app.run();
}
