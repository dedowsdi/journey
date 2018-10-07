#include "app.h"
#include "bitmaptext.h"
#include "geometry.h"
#include "points.h"
#include <list>
#include <sstream>
#include "texmap_anim.h"
#include "texutil.h"

#define WIDTH 800.0f
#define HEIGHT 800.0f
#define SIZE 50.0f // invader and ship size
#define SHIP_SPEED 160
#define INVADER_SPEED 50
#define FIRE_COOLDOWN 0.2
#define INVADER_STEP 50
#define BULLET_SPEED 500
#define BULLET_SIZE 15

namespace zxd {

class ship_geometry : public geometry_base
{
protected:
  virtual void build_vertex() { 

    vec2_array& vertices = *(new vec2_array());
    attrib_array(num_arrays(), array_ptr(&vertices));

    vertices.reserve(15);

    // create polygon in griangle fan
    vertices.push_back(vec2(0, 0));

    vertices.push_back(vec2(0, 0.5));
    vertices.push_back(vec2(-0.2, 0.4));
    vertices.push_back(vec2(-0.2, 0));
    vertices.push_back(vec2(-0.35, -0.15));
    vertices.push_back(vec2(-0.2, -0.15));
    vertices.push_back(vec2(-0.2, -0.3));
    vertices.push_back(vec2(-0.1, -0.15));
    
    // mirror right part
    for (size_t i = 7; i > 0; --i) {
      vertices.push_back(vertices[i] * vec2(-1, 1));
    }
  }

  virtual void draw(GLuint primcount = -1)
  {
    this->bind_vao();
    this->draw_arrays(GL_TRIANGLE_FAN, 0, this->num_vertices(), primcount);
  }

} ship_geometry0;

// spined circle
class invader_geomtry : public geometry_base
{
protected:
  virtual void build_vertex() { 

    vec2_array& vertices = *(new vec2_array());
    attrib_array(num_arrays(), array_ptr(&vertices));

    GLint num_spides = 10;
    GLfloat step_angle = f2pi / (num_spides * 2);
    GLfloat innder_radius = 0.15;
    GLfloat outer_radius = 0.45;

    vertices.reserve(num_spides*2 + 1);

    vertices.push_back(vec2(0));
    for (int i = 0; i < (num_spides+1)*2; ++i) 
    {
      mat4 m = glm::rotate(step_angle * i, vec3(0, 0, 1));
      if(i & 1)
      {
        vec4 v = m * vec4(innder_radius, 0, 0, 1);
        vertices.push_back(v.xy());
      }
      else if(glm::linearRand(0.0f, 1.0f) > 0.65f)
      {
        vec4 v = m * vec4(outer_radius, 0, 0, 1);
        vertices.push_back(v.xy());
      }
    }

  }

  virtual void draw(GLuint primcount = -1)
  {
    this->bind_vao();
    this->draw_arrays(GL_TRIANGLE_FAN, 0, this->num_vertices(), primcount);
  }

} invader_geometry0;

class bullet_geomtry : public geometry_base
{
protected:
  virtual void build_vertex() { 

    vec2_array& vertices = *(new vec2_array());
    attrib_array(num_arrays(), array_ptr(&vertices));

    GLint num_segs = 12;
    GLfloat step_angle = f2pi / num_segs;
    GLfloat radius = 0.5;

    vertices.reserve(num_segs+2);

    vertices.push_back(vec2(0));

    for (int i = 0; i < num_segs+1; ++i) 
    {
      mat4 m = glm::rotate(step_angle * i, vec3(0, 0, 1));
      vec4 v = m * vec4(radius, 0, 0, 1);
      vertices.push_back(v.xy());
    }

  }

  virtual void draw(GLuint primcount = -1)
  {
    this->bind_vao();
    this->draw_arrays(GL_TRIANGLE_FAN, 0, this->num_vertices(), primcount);
  }

} bullet_geometry0;

class program_name : public program
{
public:
  GLint ul_color;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "data/shader/space_invader.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/space_invader.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_color, "color");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
  }

} prg;

texture_animation_program prg1;

class base // base for ship, bullet, invader
{
protected:
  bool m_dead;
  vec2 m_pos;
  vec3 m_color;
  GLfloat m_size;
  vec2 m_velocity;
  GLfloat m_speed;
  geometry_base* m_geometry;

public:

  virtual ~base(){}

  base():
    m_dead(0)
  {
  }

  bool dead() const { return m_dead; }
  void dead(bool v){ m_dead = v; }

  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }
  void pos(GLfloat x, GLfloat y){ m_pos = vec2(x, y); }

  const vec3& color() const { return m_color; }
  void color(const vec3& v){ m_color = v; }

  GLfloat size() const { return m_size; }
  void size(GLfloat v){ m_size = v; }

  vec2 velocity() const { return m_velocity; }
  void velocity(vec2 v){ m_velocity = v; }

  bool hit(const base& i)
  {
    GLfloat r = (m_size + i.size())*0.45;
    return glm::length2(m_pos-i.pos()) <= r*r;
  }

  virtual mat4 m_mat()
  {
    return glm::translate(vec3(m_pos, 0)) * glm::scale(vec3(m_size)); 
  }

  geometry_base* geometry() const { return m_geometry; }
  void geometry(geometry_base* v){ m_geometry = v; }

  virtual void update(GLfloat dt)
  {
    m_pos += m_velocity * dt;
  }

  virtual void draw()
  {
    m_geometry->bind_vao();

    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat * m_mat()));
    glUniform3fv(prg.ul_color, 1, glm::value_ptr(m_color));

    m_geometry->draw();
  }

  virtual void reset()
  {
    m_dead = 0;
    m_velocity = vec2(0);
  }

};

struct explotion
{
  GLfloat m_size;
  vec2 m_pos;
  texmap_anim m_texmap_anim;
  mat4 m_m_mat;

  explotion(const vec2& pos, GLfloat size):
    m_size(size),
    m_pos(pos)
  {
    m_texmap_anim.num_row(4);
    m_texmap_anim.num_col(4);
    m_texmap_anim.frame_rate(24*2);

    m_texmap_anim.init();
    m_texmap_anim.prg(&prg1);

    m_m_mat = glm::scale(glm::translate(vec3(pos, 1)), vec3(m_size, m_size, 1));
  }

  mat4 m_mat()
  {
    return m_m_mat;
  }

  bool finished()
  {
    return m_texmap_anim.finished();
  }

  void update(GLfloat dt)
  {
    m_texmap_anim.update(dt);
  }

};

class invader : public base
{
protected:
  GLfloat m_angle;
public :

  invader()
  {
    geometry(&invader_geometry0);
    color(vec3(0.5, 0, 0.5));
    size(SIZE);
    m_angle = glm::linearRand(0.0f, f2pi);
  }

  void update(GLfloat dt)
  {
    m_angle += dt;
    base::update(dt);
  }

  void forward()
  {
    m_pos.y -= INVADER_STEP;
    m_velocity *= -1;
  }

  virtual mat4 m_mat()
  {
    return base::m_mat() * glm::rotate(m_angle, vec3(0, 0, 1));
  }

  virtual void reset()
  {
    base::reset();
    m_speed = INVADER_SPEED;
    m_velocity = vec2(m_speed, 0);
  }

  explotion* explode()
  {
    return new explotion(m_pos, m_size * 0.25);
  }
};

typedef std::list<invader> invader_list;

class bullet: public base
{
public:
  bullet()
  {
    geometry(&bullet_geometry0);
    m_color = vec3(0.5, 0, 1);
    m_speed = BULLET_SPEED;
    m_velocity = vec2(0, m_speed);
    m_size = BULLET_SIZE;
  }

};

typedef std::list<bullet*> bullet_list;

class ship : public base
{
protected:
  GLfloat m_fire_cooldown;
  ivec2 m_dir;

public:

  ship():
    m_dir(vec2(0))
  {
    geometry(&ship_geometry0);
    reset();
  }

  void update(GLfloat dt)
  {
    base::update(dt);
    m_fire_cooldown -= 0.2;
  }

  bullet* fire()
  {
    if(m_fire_cooldown > 0)
      return 0;

    m_fire_cooldown = 0.2;
    bullet* b = new bullet();
    b->pos(m_pos);
    return b;
  }

  void turn_left(GLboolean b)
  {
    m_dir.x += b ? -1 : 1;
    update_velocity();
  }

  void turn_right(GLboolean b)
  {
    m_dir.x += b ? 1 : -1;
    update_velocity();
  }

  void update_velocity()
  {
    m_dir.x = glm::min(glm::max(m_dir.x, -1), 1);
    m_velocity = vec2(m_dir) * m_speed;
  }

  bool hit(const invader& invader0)
  {
    return false;
  }

  virtual void reset()
  {
    base::reset();
    color(vec3(0));
    size(SIZE);
    m_fire_cooldown = FIRE_COOLDOWN;
    m_speed = SHIP_SPEED;

    // dont't reset dir here, as user might still hoding keys
    //m_dir = vec2(0);
    //update_velocity();
  }

};

typedef std::list<explotion*> explotion_list;

class space_invader_app : public app {
protected:
  bitmap_text m_text;
  ship m_ship;
  invader_list m_invaders;
  bullet_list m_bullets;
  explotion_list m_explotions;
  GLuint m_score;
  GLuint m_explotion_texture;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "app_name";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    ship_geometry0.build_mesh();
    invader_geometry0.build_mesh();
    bullet_geometry0.build_mesh();

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    prg1.init();

    glGenTextures(1, &m_explotion_texture);
    glBindTexture(GL_TEXTURE_2D, m_explotion_texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    fipImage fi = fipLoadImage("data/texture/explosion.jpg");

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fi.getWidth(), fi.getHeight(), 0,
        GL_BGR, GL_UNSIGNED_BYTE, fi.accessPixels());

    reset_game();
  }

  virtual void reset_game()
  {
    m_score = 0;
    // ship
    m_ship.reset();
    m_ship.pos(vec2(WIDTH/2, 30));

    // place invaders
    GLfloat spacing = 20;
    GLint cx = 10;
    GLint cy = 5;

    GLfloat y0 = HEIGHT - 80;
    GLfloat x0 = (WIDTH - cx * (SIZE) - (cx-1) * spacing)/2 + SIZE/2;

    m_invaders.resize(cx*cy);

    std::list<invader>::iterator iter;
    int i;

    for (i = 0, iter = m_invaders.begin(); i < m_invaders.size(); ++i, ++iter) {
      GLint row = i/cx;
      GLint col = i%cx;
      GLfloat x= x0 + (SIZE + spacing) * col;
      GLfloat y= y0 - (SIZE + spacing) * row;
      iter->pos(x, y);
      iter->reset();
    }

    for(auto iter = m_bullets.begin(); iter != m_bullets.end(); ++iter){
      delete *iter;
    }
    m_bullets.clear();

    for(auto iter = m_explotions.begin(); iter != m_explotions.end(); ++iter){
      delete (*iter);
    }
    m_explotions.clear();
  }

  virtual void update() 
  {

    if(m_ship.dead() || m_invaders.empty())
    {
      reset_game();
      return;
    }

    // remove dead bullets, invaders, explotions
    for(auto iter = m_bullets.begin(); iter != m_bullets.end();){
      if((*iter)->dead())
      {
        m_score += 100;
        delete *iter;
        iter = m_bullets.erase(iter);
      }
      else
        ++iter;
    }

    for(auto iter = m_invaders.begin(); iter != m_invaders.end(); ){
      if(iter->dead())
        iter = m_invaders.erase(iter);
      else
        ++iter;
    }

    for(auto iter = m_explotions.begin(); iter != m_explotions.end(); ){
      if((*iter)->finished())
      {
        delete *iter;
        iter = m_explotions.erase(iter);
      }
      else
        ++iter;
    }

    // animate
    m_ship.update(m_delta_time);
    for(auto iter = m_bullets.begin(); iter != m_bullets.end();++iter){
      (*iter)->update(m_delta_time);
    }

    for(auto iter = m_explotions.begin(); iter != m_explotions.end(); ++iter){
      (*iter)->update(m_delta_time);
    }
  
    // min max invader xy
    vec2 xy0 = vec2(WIDTH, HEIGHT);
    vec2 xy1 = vec2(0, 0);
    for(auto iter = m_invaders.begin(); iter != m_invaders.end(); ++iter ){
      iter->update(m_delta_time);
      xy0 = glm::min(xy0, iter->pos());
      xy1 = glm::max(xy1, iter->pos());
    }

    // forward if hit border
    if((xy1.x >= WIDTH && m_invaders.begin()->velocity().x > 0) ||
       (xy0.x <= 0 && m_invaders.begin()->velocity().x < 0))
    {
      for(auto iter = m_invaders.begin(); iter != m_invaders.end(); ++iter)
      {
        iter->forward();
      }
    }

    xy0 -= vec2(SIZE * 0.5);
    xy1 += vec2(SIZE * 0.5);
    // dead check
    GLfloat hz = BULLET_SIZE * 0.5;
    for(auto iter = m_bullets.begin(); iter != m_bullets.end(); ++iter){
      bullet& b = **iter;
      if(b.pos().x < 0 - 0.5*hz || b.pos().x > WIDTH + 0.5 * hz || b.pos().y > HEIGHT + 0.5 * hz)
      {
        b.dead(true);
        continue;
      }

      // hit test
      if(b.pos().x < xy0.x || b.pos().x > xy1.x || b.pos().y < xy0.y || b.pos().y > xy1.y)
        continue;

      for(auto iter1 = m_invaders.begin(); iter1 != m_invaders.end(); ++iter1){
        if(b.hit(*iter1))
        {
          std::cout << "bullet at " << b.pos().x << ":" << b.pos().y << " hit invader at "
          << iter1->pos().x << ":" << iter1->pos().y << std::endl;
          b.dead(true);
          iter1->dead(true);
          m_explotions.push_back(iter1->explode());
          break;
        }
      }
    }
    
    for(auto iter = m_invaders.begin(); iter != m_invaders.end(); ++iter)
    {
      if(iter->hit(m_ship))
      {
        m_ship.dead(true);
        break;
      }
    }

  }

  virtual void display() {

    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();

    m_ship.draw();
    for(auto iter = m_invaders.begin(); iter != m_invaders.end(); ++iter)
    {
      iter->draw();
    }
    glPointSize(BULLET_SIZE);
    for(auto iter = m_bullets.begin(); iter != m_bullets.end(); ++iter)
    {
      (*iter)->draw();
    }
    glPointSize(1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);

    glBindTexture(GL_TEXTURE_2D, m_explotion_texture);
    for(auto iter = m_explotions.begin(); iter != m_explotions.end(); ++iter){
      (*iter)->m_texmap_anim.draw(prg.vp_mat * (*iter)->m_mat(), 0);
    }

    glDisable(GL_BLEND);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << m_score << std::endl;
    m_text.print(ss.str(), 20, HEIGHT - 30);
    glDisable(GL_BLEND);

  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {

    if(action == GLFW_REPEAT)
      return;

    switch(key)
    {
      case GLFW_KEY_A:
        m_ship.turn_left(action == GLFW_PRESS);
        break;

      case GLFW_KEY_D:
        m_ship.turn_right(action == GLFW_PRESS);
        break;

      default:
        break;
    }

    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;

        case GLFW_KEY_J:
          {
            bullet* b = m_ship.fire();
            if(b)
              m_bullets.push_back(b);
          }
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
  zxd::space_invader_app app;
  app.run();
}
