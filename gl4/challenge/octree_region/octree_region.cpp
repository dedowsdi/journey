#include <app.h>
#include <bitmap_text.h>
#include <sstream>
#include "otree.h"
#include <common_program.h>
#include <cuboid.h>
#include <debugger.h>

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

glm::mat4 v_mat;
glm::mat4 p_mat;

cuboid geometry;
light_vector lights;
light_model lm;
material mtl0;
material mtl1; // selected
GLuint num_entities = 2000;
bool draw_tree = false;
bool draw_cube = true;
GLfloat boundary = 100;

box select_box;
vec3 vel;

class entity : public fruit
{
protected:
  vec3 m_pos; // center
  vec3 m_size; // diagnal

public:

  const vec3& pos() const { return m_pos; }
  void pos(const vec3& v){ m_pos = v; }

  const vec3& size() const { return m_size; }
  void size(const vec3& v){ m_size = v; }

  void update_boundingbox()
  {
    vec3 box_size = vec3(glm::max(glm::max(m_size.x, m_size.y), m_size.z));
    m_boundingbox.corner0(m_pos - box_size * 0.5f);
    m_boundingbox.corner1(m_pos + box_size * 0.5f);
  }

  mat4 m_mat()
  {
    return glm::scale(glm::translate(m_pos), m_size);
  }
};

std::vector<entity> entities;
std::vector<fruit*> selected;

blinn_program prg;

class octree_region_app : public app
{
protected:
  octree* m_tree;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "octree_region_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    light_source l0;
    l0.position = vec4(-1, 1, 1, 0);
    l0.diffuse = vec4(1);
    l0.specular = vec4(0.6);
    lights.push_back(l0);

    lm.local_viewer = true;

    mtl0.diffuse = vec4(0.8f);
    mtl0.specular = vec4(0.5f);
    mtl0.shininess = 50;

    mtl1 = mtl0;
    mtl1.diffuse = vec4(1, 0, 0, 1);
    mtl1.emission = vec4(1, 0, 0, 1);

    prg.init();
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
    v_mat = zxd::isometric_projection(300);
    set_v_mat(&v_mat);
    prg.bind_lighting_uniform_locations(lights, lm, mtl0);
    mtl1.bind_uniform_locations(prg);

    entities.resize(num_entities);

    m_tree = new octree(vec3(-boundary), vec3(boundary));

    entity manual;
    manual.size(vec3(1));
    manual.pos(vec3(0));
    m_tree->insert(&manual);

    for(auto& item : entities)
    {
      //std::cout << "start insert " << item.boundingbox().corner0() 
        //<< ":" << item.boundingbox().corner1()  << std::endl;
      item.pos(glm::linearRand(vec3(-100), vec3(100)));
      item.size(glm::linearRand(vec3(2), vec3(6)));
      item.update_boundingbox();
      m_tree->insert(&item);
      //std::cout << "finish insert " << item.boundingbox().corner0() 
        //<< ":" << item.boundingbox().corner1()  << std::endl;
    }
    std::cout << "finish insert" << std::endl;

    geometry.build_mesh({attrib_semantic::vertex, attrib_semantic::normal});

    select_box.corner0(vec3(-10));
    select_box.corner1(vec3(10));
    select_box.set_pos(glm::linearRand(vec3(-50), vec3(50)));
    vel = glm::sphericalRand(0.5);
  }

  virtual void update()
  {
    vec3 pos = select_box.pos();
    pos += vel;
    if(pos.x > boundary)
    {
      pos.x = boundary;
      vel.x *= -1;
    }
    else if(pos.x < -boundary)
    {
      pos.x = -boundary;
      vel.x *= -1;
    }

    if(pos.y > boundary)
    {
      pos.y = boundary;
      vel.y *= -1;
    }
    else if(pos.y < -boundary)
    {
      pos.y = -boundary;
      vel.y *= -1;
    }

    if(pos.z > boundary)
    {
      pos.z = boundary;
      vel.z *= -1;
    }
    else if(pos.z < -boundary)
    {
      pos.z = -boundary;
      vel.z *= -1;
    }
    select_box.set_pos(pos);

    //std::cout <<"start get"<< std::endl;
    selected = m_tree->get(select_box);
    //std::cout <<"end get"<< std::endl;
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    if(draw_tree)
      m_tree->debug_draw(p_mat * v_mat);

    prg.use();

    prg.update_lighting_uniforms(lights, lm, mtl0, v_mat);
    if(draw_cube)
    {
      for (int i = 0; i < entities.size(); ++i)
      {
        prg.update_uniforms(entities[i].m_mat(), v_mat, p_mat);
        geometry.draw();
      }
    }

    mtl1.update_uniforms();
    for (int i = 0; i < selected.size(); ++i)
    {
      entity* ety = static_cast<entity*>(selected[i]);
      prg.update_uniforms(ety->m_mat() * glm::scale(vec3(1.01)), v_mat, p_mat);
      geometry.draw();
    }

    glDisable(GL_DEPTH_TEST);
    vec3_vector lines;
    lines.reserve(24);
    select_box.collect_debug_geometry(lines);
    debugger::draw_line(GL_LINES, lines, p_mat * v_mat, 1, vec4(1, 0, 1, 1));

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps << std::endl;;
    ss << "q : draw tree : " << draw_tree << std::endl;
    ss << "w : draw cube : " << draw_cube << std::endl;
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
        case GLFW_KEY_Q:
          draw_tree = !draw_tree;
          break;
        case GLFW_KEY_W:
          draw_cube = !draw_cube;
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
  zxd::octree_region_app app;
  app.run();
}
