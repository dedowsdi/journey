#define GLM_FORCE_SWIZZLE
#include <sstream>
#include <list>

#include "app.h"
#include "bitmap_text.h"
#include "common_program.h"
#include "sphere.h"
#include "movable.h"
#include "cone.h"
#include "debugger.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

vec4 poison_color = vec4(1, 0, 0, 1);
vec4 food_color = vec4(0 ,0, 1, 1);
GLuint num_poision = 50;
GLuint num_food = 50;
GLuint num_ships = 50;
GLfloat food_radius = 1;
GLfloat food_radius2 = food_radius * food_radius;
GLfloat food_nutrition = 0.1;
GLfloat food_rate = 0.007;
GLfloat poision_rate = 0.01;
GLfloat ship_rate = 0.0017;
GLfloat poision_nutrition = -0.1;
GLfloat max_ship_speed = 1;
GLfloat max_ship_speed2 = max_ship_speed * max_ship_speed;
GLfloat max_ship_force = 0.125;
GLfloat ship_health_step = 0.002;
GLfloat ship_mutate_rate = 0.1;
GLfloat boundary = 100;

glm::mat4 v_mat;
glm::mat4 p_mat;

sphere sphere0;
cone cone0;

blinn_program prg;
light_vector lights;

material food_mtl;
material poison_mtl;
material food_bubble_mtl;
material poison_bubble_mtl;

light_model lm;
lightless_program ll_prg;

struct food
{
  vec3 pos;
  GLfloat nutrition;

  food() = default;

  food(const vec3& pos_, GLfloat nutrition_):
    pos(pos_),
    nutrition(nutrition_)
  {
  }

  mat4 m_mat()
  {
    return glm::translate(pos);
  }
};

class ship : public movable
{
protected:

  struct dna
  {
    GLfloat food_weight;
    GLfloat poision_weight;
    GLfloat food_perception;
    GLfloat poison_perception;

    void random()
    {
      food_weight = glm::linearRand(-1.0f, 1.0f);
      poision_weight = glm::linearRand(-1.0f, 1.0f);
      food_perception = glm::linearRand(2.0f, 80.0f);
      poison_perception = glm::linearRand(2.0f, 80.0f);
      //std::cout << food_weight << std::endl;
    }

    void mutate()
    {
      food_weight += glm::linearRand(-0.1f, 0.1f);
      poision_weight += glm::linearRand(-0.1f, 0.1f);
      food_perception += glm::max(glm::linearRand(-10.0f, 10.0f), 0.0f);
      poison_perception += glm::max(glm::linearRand(-10.0f, 10.0f), 0.0f);
    }
  };

  material m_mtl;
  GLfloat m_health = 1.0;
  dna m_dna;
  food* m_target_poison;
  food* m_target_food;

public:

  ship()
  {
    m_mtl.specular = vec4(0.5);
    m_mtl.shininess = 50;
    m_mtl.bind_uniform_locations(prg);
    m_dna.random();
  }

  void draw()
  {
    mat4 m = glm::translate(glm::translate(m_pos), vec3(0, 0, cone0.height() * -0.5f))
      * zxd::rotate_to_any(pza, m_vel) ;
    m_mtl.diffuse = glm::mix(food_color, poison_color, m_health);

    prg.update_uniforms(m, v_mat, p_mat);
    m_mtl.update_uniforms();

    cone0.draw();

  }

  void debug_draw_target()
  {
    if(m_target_food)
    {
      debugger::draw_line(m_pos, m_target_food->pos, p_mat * v_mat, 1, food_color);
    }

    if(m_target_poison)
    {
      debugger::draw_line(m_pos, m_target_poison->pos, p_mat * v_mat, 1, poison_color);
    }
  }

  void debug_draw_perception()
  {
    GLfloat inner_radius;
    GLfloat outer_radius;
    material* inner_material;
    material* outer_material;

    if(m_dna.food_perception > m_dna.poison_perception)
    {
      inner_radius = m_dna.poison_perception;
      inner_material = &poison_bubble_mtl;
      outer_radius = m_dna.food_perception;
      outer_material = &food_bubble_mtl;
    }
    else
    {
      outer_radius = m_dna.poison_perception;
      outer_material = &poison_bubble_mtl;
      inner_radius = m_dna.food_perception;
      inner_material = &poison_bubble_mtl;
    }
    mat4 m = glm::scale(glm::translate(m_pos), vec3(inner_radius));
    prg.update_uniforms(m, v_mat, p_mat);
    inner_material->update_uniforms();
    sphere0.draw();

    m = glm::scale(glm::translate(m_pos), vec3(outer_radius));
    prg.update_uniforms(m, v_mat, p_mat);
    outer_material->update_uniforms();
    sphere0.draw();

    //mat4 m = glm::scale(glm::translate(m_pos), vec3(inner_radius));
    //glUniformMatrix4fv(ll_prg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat * v_mat * m));
    //glUniform4fv(ll_prg.ul_color, 1, glm::value_ptr(inner_material->diffuse));
    //sphere0.draw();

    //m = glm::scale(glm::translate(m_pos), vec3(outer_radius));
    //glUniformMatrix4fv(ll_prg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat * v_mat * m));
    //glUniform4fv(ll_prg.ul_color, 1, glm::value_ptr(outer_material->diffuse));
    //sphere0.draw();
  }

  ship split()
  {
    //std::cout << "splitting!" << std::endl;
    ship child = *this;
    child.m_health = 1;
    child.vel(vec3(0));
    child.acc(vec3(0));
    if(glm::linearRand(0.0f, 1.0f) < ship_mutate_rate)
      child.m_dna.mutate();
    return child;
  }

  bool dead()
  {
    return m_health < 0;
  }

  void hunter(std::list<food>& foods)
  {
    m_target_food = 0;
    m_target_poison = 0;
    GLfloat target_food_dist2 = FLT_MAX;
    GLfloat target_poison_dist2 = FLT_MAX;

    GLfloat fp2 = m_dna.food_perception * m_dna.food_perception;
    GLfloat pp2 = m_dna.poison_perception * m_dna.poison_perception;

    // eat food or poison, find nearest food or poison
    for(auto iter = foods.begin(); iter != foods.end();)
    {
      food& f = *iter;
      GLfloat d2 = glm::distance2(f.pos, m_pos);

      if(d2 < food_radius2)
      {
        m_health += f.nutrition;
        iter = foods.erase(iter);
        continue;
      }

      ++iter;

      if(f.nutrition > 0 && d2 > fp2)
        continue;
      if(f.nutrition < 0 && d2 > pp2)
        continue;

      if(f.nutrition > 0 && d2 < target_food_dist2)
      {
        m_target_food = &f;
        target_food_dist2 = d2;
      }
      else if(f.nutrition < 0 && d2 < target_poison_dist2)
      {
        m_target_poison = &f;
        target_poison_dist2 = d2;
      }
    }

    if(m_target_food)
    {
      steer_to(m_target_food->pos, m_dna.food_weight);
    }

    if(m_target_poison)
    {
      steer_to(m_target_poison->pos, m_dna.poision_weight);
    }

    m_health -= ship_health_step;
    apply_boundary();
    movable::update();
    // steer back if it's out of boundary
  }

  void apply_boundary()
  {
    vec3 desire;
    GLfloat s = max_ship_speed;

    if(m_pos.x < -boundary)
      desire = vec3(s, m_vel.yz());
    else if(m_pos.x > boundary)
      desire = vec3(-s, m_vel.yz());

    if(m_pos.y < -boundary)
      desire = vec3(m_vel.x, s, m_vel.z);
    else if(m_pos.y > boundary)
      desire = vec3(m_vel.x, -s, m_vel.z);

    if(m_pos.z < -boundary)
      desire = vec3(m_vel.xy(), s);
    else if(m_pos.z > boundary)
      desire = vec3(m_vel.xy(), -s);

    if(desire == vec3(0))
      return;

    desire = glm::normalize(desire) * max_ship_speed;
    vec3 force = glm::normalize(desire - m_vel) * max_ship_force;
    apply_force(force);
  }


  void steer_to(const vec3& target, float weight)
  {
    vec3 desire = zxd::clamp_length(target - m_pos, 0.0f, max_ship_speed);
    apply_force(zxd::clamp_length((desire - m_vel) * weight, 0.0f, max_ship_force));
  }

};

std::list<food> foods;
std::list<ship> ships;

class evolutionary_steer_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "evolutionary_steer_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);


    light_source l0;
    l0.position = vec4(-1, 1, 1, 0);
    l0.diffuse = vec4(1);
    l0.specular = vec4(0.8);
    lights.push_back(l0);

    food_mtl.diffuse = food_color;
    food_mtl.specular = vec4(0.5);
    food_mtl.shininess = 50;
    food_mtl.emission = food_color * 0.25f;

    poison_mtl.diffuse = poison_color;
    poison_mtl.specular = vec4(0.5);
    poison_mtl.shininess = 50;
    poison_mtl.emission = poison_color * 0.25f;

    lm.local_viewer = true;

    prg.init();
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
    v_mat = zxd::isometric_projection(boundary * 2);
    set_v_mat(&v_mat);
    ll_prg.init();

    prg.bind_lighting_uniform_locations(lights, lm, food_mtl);
    poison_mtl.bind_uniform_locations(prg);

    food_bubble_mtl = food_mtl;
    food_bubble_mtl.specular = vec4(0);
    poison_bubble_mtl = poison_mtl;
    poison_bubble_mtl.specular = vec4(0);

    sphere0.include_normal(true);
    sphere0.slice(16);
    sphere0.stack(16);
    sphere0.radius(1);
    sphere0.build_mesh();

    cone0.include_normal(true);
    cone0.slice(32);
    cone0.stack(16);
    cone0.height(5);
    cone0.build_mesh();

    for (int i = 0; i < num_food; ++i) 
      foods.emplace_back(glm::linearRand(vec3(-boundary), vec3(boundary)), food_nutrition);
    for (int i = 0; i < num_poision; ++i) 
      foods.emplace_back(glm::linearRand(vec3(-boundary), vec3(boundary)), poision_nutrition);

    ships.resize(num_ships);

    for(auto& ship : ships)
    {
      ship.pos(glm::linearRand(vec3(-boundary), vec3(boundary)));
      ship.vel(glm::sphericalRand(1.0f) * glm::linearRand(max_ship_speed * 0.1f, max_ship_speed));
      ship.acc(vec3(0));
    }
  }

  virtual void update() 
  {
    if(glm::linearRand(0.0f, 1.0f) < food_rate)
      foods.emplace_back(glm::linearRand(vec3(-boundary), vec3(boundary)), food_nutrition);
    if(glm::linearRand(0.0f, 1.0f) < poision_rate)
      foods.emplace_back(glm::linearRand(vec3(-boundary), vec3(boundary)), poision_nutrition);

    for(auto iter = ships.begin(); iter != ships.end(); )
    {
      if(iter->dead())
      {
        foods.emplace_back(iter->pos(), food_nutrition);
        iter = ships.erase(iter);
        continue;
      }

      iter->hunter(foods);
      if(glm::linearRand(0.0f, 1.0f) < ship_rate)
        ships.push_back(iter->split());
      ++iter;
    }
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prg.use();
    prg.update_uniforms(mat4(1), v_mat, p_mat);
    prg.update_lighting_uniforms(lights, lm, food_mtl, v_mat);
    // render food
    for(auto& food : foods)
    {
      if(food.nutrition > 0)
      {
        prg.update_uniforms(food.m_mat(), v_mat, p_mat);
        sphere0.draw();
      }
    }

    // render poison
    poison_mtl.update_uniforms();
    for(auto& food : foods)
    {
      if(food.nutrition < 0)
      {
        prg.update_uniforms(food.m_mat(), v_mat, p_mat);
        sphere0.draw();
      }
    }

    for(auto& ship : ships)
    {
      ship.draw();
    }

    ll_prg.use();
    for(auto& ship : ships)
    {
      ship.debug_draw_target();
    }

    //ll_prg.use();
    prg.use();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_COLOR);
    for(auto& ship : ships)
    {
      ship.debug_draw_perception();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps;
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
  zxd::evolutionary_steer_app app;
  app.run();
}
