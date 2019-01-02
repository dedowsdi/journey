/*
 * render billboards as snow, put everything except vertex and texcoord in
 * instance attribute, use tex_offset to locate different part in tex pack.
 * as texture is transparent, you need to sort it every frame, so you need to
 * update buffer every frame, not sure it's a good way do do this.
 */
#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"
#include "quad.h"
#include "spritesheet.h"
#include "texutil.h"
#include <algorithm>
#include <chrono>
#include "timer.h"
#include "datautil.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

billboard_program prg;
billboard_quad quad;
GLint num_flakes = 20000;

GLuint vbo; // buffer obj for translation, size, angle

struct snow
{
  // buffer data
  float angle = 0;
  vec2 texcoord = vec2(0); // lb corner
  vec2 size = vec2(10);
  vec3 pos;

  GLfloat angular_velocity = 0;
  vec3 vel = vec3(0);
  vec3 acc = vec3(0);
  float depth;

  snow(const vec3& pos_, const vec2& size_, float angle_):
    angle(angle_),
    size(size_),
    pos(pos_)
  {
    
  }

  void update()
  {
    pos += vel;
    vel += acc;
    angle += angular_velocity;
    acc = vec3(0);

    if(pos.z < 0)
    {
      pos.z = 500;
      random();
    }
  }

  void applyForce(const vec3& f)
  {
    acc += f;
  }

  void random()
  {
    vel = glm::linearRand(vec3(-1, -1, -1), vec3(1, 1, -1));
    angle = glm::linearRand(0.0f, 1.0f) * f2pi;
    angular_velocity = glm::linearRand(-0.03, 0.03);
  }
};

class snow_app : public app {
protected:
  bitmap_text m_text;
  std::vector<snow*> m_snows;
  spritesheet* m_flakes = 0;

public:
  ~snow_app()
  {
    if(m_flakes)
      delete m_flakes;
    for(auto item : m_snows)
    {
      delete item;
    }
    m_snows.clear();
    
  }
  virtual void init_info() {
    app::init_info();
    m_info.title = "snow_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.scale = true;
    prg.rotate = true;
    prg.tex_offset = true;
    prg.use_camera_up = false;
    prg.init();
    prg.p_mat = glm::perspective(fpi4, wnd_aspect(), 1.0f, 5000.0f);
    prg.v_mat = zxd::isometric_projection(500);
    set_v_mat(&prg.v_mat);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    fipImage image = fipLoadResource32("texture/f32.png");
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.getWidth(), image.getHeight(),
        0, GL_RGBA, GL_UNSIGNED_BYTE, image.accessPixels());

    m_flakes = new spritesheet(tex, 16, 16);

    quad.rect(vec2(-0.5), vec2(0.5));
    quad.texcoord(vec2(0), m_flakes->sprite_size());
    quad.include_texcoord(true);
    quad.build_mesh();

    m_snows.reserve(num_flakes);
    for (int i = 0; i < num_flakes; ++i) {
      vec3 pos = glm::linearRand(vec3(-500, -500, 10), vec3(500, 500, 500));
      float size = glm::linearRand(3.0, 6.0);
      float angle = glm::linearRand(0.0f, f2pi);
      snow* s = new snow(pos, vec2(size), angle);
      s->random();
      s->texcoord = m_flakes->get_random_sprite();
      m_snows.push_back(s);
    }

    draw_arrays* da = static_cast<draw_arrays*>(quad.get_primitive_set(0));
    da->num_instance(num_flakes);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_snows.size() * 32, 0, GL_STREAM_DRAW);

    quad.bind_vao();

    GLuint stride = 32;
    // translation
    glVertexAttribPointer(2, 3, GL_FLOAT, 0, stride, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(2);
    glVertexAttribDivisor(2, 1);

    // size
    glVertexAttribPointer(3, 2, GL_FLOAT, 0, stride, BUFFER_OFFSET(12));
    glEnableVertexAttribArray(3);
    glVertexAttribDivisor(3, 1);

    // angle
    glVertexAttribPointer(4, 1, GL_FLOAT, 0, stride, BUFFER_OFFSET(20));
    glEnableVertexAttribArray(4);
    glVertexAttribDivisor(4, 1);

    // texcoord
    glVertexAttribPointer(5, 2, GL_FLOAT, 0, stride, BUFFER_OFFSET(24));
    glEnableVertexAttribArray(5);
    glVertexAttribDivisor(5, 1);

  }

  virtual void update() {
    const mat4& v_mat = prg.v_mat;
    vec3 camera_pos = -v_mat[3].xyz();
    camera_pos = vec3(glm::dot(v_mat[0].xyz(), camera_pos),
                      glm::dot(v_mat[1].xyz(), camera_pos),
                      glm::dot(v_mat[2].xyz(), camera_pos));
    vec3 camera_nza = -glm::row(v_mat, 2).xyz();

    for(auto item : m_snows)
    {
      item->applyForce(vec3(0, 0, -0.02));
      item->update();
      item->depth = glm::dot(camera_nza, item->pos);
    }
    
    //timer timer0;
    // sort by depth
    std::sort(m_snows.begin(), m_snows.end(),
        [&](const snow* lhs, const snow* rhs)->bool
        {
          //return glm::dot(lhs->pos, camera_nza) > glm::dot(rhs->pos, camera_nza);
          return lhs->depth > rhs->depth;
        });
    //data_util::insertion_sort(m_snows.begin(), m_snows.end(),
        //[&](const snow* lhs, const snow* rhs)->bool
        //{
          ////return glm::dot(lhs->pos, camera_nza) > glm::dot(rhs->pos, camera_nza);
          //return lhs->depth > rhs->depth;
        //});

    float_vector buffer;
    buffer.reserve(m_snows.size() * 8);
    for(auto item : m_snows)
    {
      buffer.push_back(item->pos.x);
      buffer.push_back(item->pos.y);
      buffer.push_back(item->pos.z);

      buffer.push_back(item->size.x);
      buffer.push_back(item->size.y);

      buffer.push_back(item->angle);

      buffer.push_back(item->texcoord.x);
      buffer.push_back(item->texcoord.y);
    }
    //std::cout << timer0.time_miliseconds() << std::endl;

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, buffer.size() * 4, &(buffer.front()));
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    prg.use();
    prg.update_uniforms();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, m_flakes->tex());
    quad.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
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
  zxd::snow_app app;
  app.run();
}
