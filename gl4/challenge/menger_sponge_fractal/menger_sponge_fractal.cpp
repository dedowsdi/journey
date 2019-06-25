#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "cuboid.h"
#include "common_program.h"
#include "light.h"
#include "common.h"

#define SPONGE_SIZE 100.0f
#define USE_INSTANCE

namespace zxd
{

glm::mat4 v_mat;
glm::mat4 p_mat;

blinn_program prg;

// use origin as center
class cell;
typedef std::vector<cell> cell_vector;
cuboid geometry;

mat4_vector mv_mats;
mat4_vector mv_mats_it;
mat4_vector mvp_mats;

class cell
{
public:
  GLfloat m_size;
  vec3 m_position;
  mat4 m_model_matrix;
public:
  cell(const vec3& position, GLfloat size):
    m_size(size),
    m_position(position)
  {
    m_model_matrix = glm::translate(m_position) * glm::scale(vec3(m_size));
  }

  // subdivide all cell, 
  cell_vector subdivide()
  {
    cell_vector cv;

    // subdivide each cell face into 9 cells, exclude cell in face center and cube
    // center. there should be 27 - 6 - 1 = 20 cells
    GLfloat size = m_size/3;
    for (int j = -1; j < 2; ++j)
    {
      for (int k = -1; k < 2; ++k)
      {
        for (int l = -1; l < 2; ++l)
        {
          if(j*k || j*l || k*l)
          {
            cv.push_back(cell(m_position + vec3(j*size, k*size, l*size), size));
          }
        }
      }
    }

    return cv;
  }

};

class menger_sponge_fractal : public app
{
protected:
  bitmap_text m_text;
  cell_vector m_sponge;
  light_vector m_lights;
  light_model m_light_model;
  material m_material;
  GLuint m_b_mv_mats;
  GLuint m_b_mvp_mats;
  GLuint m_b_mv_mats_it;
  GLboolean m_dirty_view;
  GLfloat m_cooldown; // used when sponge is divided 4 or more times

  virtual void init_info()
  {
    app::init_info();
    m_info.title = "menger sponge fractal";
    m_info.wnd_width = 800;
    m_info.wnd_height = 800;
  }

  virtual void create_scene()
  {
    glClearColor(0, 0.5, 1, 1);
    m_dirty_view = GL_TRUE;

    m_cooldown = 0;

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    m_sponge.push_back(cell(vec3(0), SPONGE_SIZE));

    //setup light material
    light_source light0; 
    light0.position = vec4(vec3(SPONGE_SIZE * 1.8), 1);
    light0.diffuse = vec4(1);
    light0.specular = vec4(1);
    m_lights.push_back(light0);
    
    m_light_model.local_viewer = 1;

    m_material.diffuse = vec4(0.7);
    m_material.specular = vec4(0.5);
    m_material.ambient = vec4(0.8);
    m_material.shininess = 70.0;

#ifdef USE_INSTANCE 
    prg.instance = GL_TRUE;
#endif
    prg.init();

    // setup camera
    v_mat = glm::lookAt(vec3(1.0) * SPONGE_SIZE * 3.0f, vec3(0), vec3(0, 0, 1));
    p_mat = glm::perspective(fpi4, this->wnd_aspect(), 0.1f, 1000.0f);

    set_v_mat(&v_mat);

    geometry.include_normal(true);
    geometry.build_mesh();
    // additonal instance attribute
    glGenBuffers(1, &m_b_mv_mats);
    glGenBuffers(1, &m_b_mvp_mats);
    glGenBuffers(1, &m_b_mv_mats_it);

    if(prg.instance)
      reset_instance_attribute_buffers();
  }

  virtual void update()
  {
    static GLfloat x = 0;
    static GLfloat y = 0;
    static GLfloat z = 0;
    
    x += m_delta_time * 0.3;
    y += m_delta_time * 0.6;
    z += m_delta_time * 0.9;

    m_lights[0].position = glm::rotate(z, vec3(0,0,1))*glm::rotate(y,
        vec3(0,1,0))*glm::rotate(x, vec3(1,0,0)) * vec4(vec3(SPONGE_SIZE*1.8),
        1);

    m_cooldown -= m_delta_time;
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    prg.use();

    prg.bind_lighting_uniform_locations(m_lights, m_light_model, m_material);
    prg.update_lighting_uniforms(m_lights, m_light_model, m_material, v_mat);

    if(!prg.instance)
    {
      for (size_t i = 0; i < m_sponge.size(); ++i)
      {
        //std::cout << prg.mvp_mat << std::endl;
        prg.update_uniforms(m_sponge[i].m_model_matrix, v_mat, p_mat);
        geometry.draw();
      }
    }
    else
    {
      if(m_dirty_view && ( m_sponge.size() <= 8000 || m_cooldown < 0))
        update_instance_attribute_buffers();
      geometry.draw();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "fps : " << m_fps << std::endl;
    ss << "q : subdivide" << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  void subdivide()
  {
    cell_vector copy(m_sponge);
    m_sponge.clear();
    m_sponge.reserve(copy.size() * 20);

    for (size_t i = 0; i < copy.size(); ++i)
    {
      cell_vector cells = copy[i].subdivide();
      m_sponge.insert(m_sponge.end(), cells.begin(), cells.end());
    }

    if(prg.instance)
    {
      reset_instance_attribute_buffers();
      geometry.set_num_instance(m_sponge.size());
    }
  }

  void reset_instance_attribute_buffers()
  {
    geometry.bind_vao();
    glBindBuffer(GL_ARRAY_BUFFER, m_b_mv_mats);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m_sponge.size(), 0, GL_DYNAMIC_DRAW);
    matrix_attrib_pointer(3);

    glBindBuffer(GL_ARRAY_BUFFER, m_b_mvp_mats);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m_sponge.size(), 0, GL_DYNAMIC_DRAW);
    matrix_attrib_pointer(7);

    glBindBuffer(GL_ARRAY_BUFFER, m_b_mv_mats_it);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mat4) * m_sponge.size(), 0, GL_DYNAMIC_DRAW);
    matrix_attrib_pointer(11);

    update_instance_attribute_buffers();
  }

  void update_instance_attribute_buffers()
  {
    mv_mats.clear();
    mvp_mats.clear();
    mv_mats_it.clear();
    
    for (size_t i = 0; i < m_sponge.size(); ++i) 
    {
      mv_mats.push_back(v_mat * m_sponge[i].m_model_matrix);
      mvp_mats.push_back(p_mat * mv_mats.back());
      mv_mats_it.push_back(glm::transpose(glm::inverse(mv_mats.back())));
      //std::cout << mvp_mats.back() << std::endl;
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_b_mv_mats);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * m_sponge.size(), glm::value_ptr(mv_mats.front()));

    glBindBuffer(GL_ARRAY_BUFFER, m_b_mvp_mats);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * m_sponge.size(), glm::value_ptr(mvp_mats.front()));

    glBindBuffer(GL_ARRAY_BUFFER, m_b_mv_mats_it);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * m_sponge.size(), glm::value_ptr(mv_mats_it.front()));
    m_dirty_view = GL_FALSE;

    m_cooldown = 0.3;
  }


  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
        case GLFW_KEY_Q:
          subdivide();
          break;
        default:
          break;
      }
    }

    app::glfw_key(wnd, key, scancode, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y)
  {
    app::glfw_mouse_move(wnd, x, y);
    //update_instance_attribute_buffers();
    if (glfwGetMouseButton(m_wnd, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
      m_dirty_view = GL_TRUE;
  }

  void glfw_mouse_wheel(GLFWwindow *wnd, double xoffset, double yoffset)
  {
    app::glfw_mouse_wheel(wnd, xoffset, yoffset);
    m_dirty_view = GL_TRUE;
  }

};

}

int main(int argc, char *argv[])
{
  zxd::menger_sponge_fractal app;

  app.run();
  
  return 0;
}
