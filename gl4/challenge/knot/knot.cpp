/*
 * http://paulbourke.net/geometry/knots/
 */
#include <app.h>
#include <bitmap_text.h>
#include <sstream>
#include <common_program.h>
#include <geometry.h>
#include <geometry_util.h>

#define WIDTH 800
#define HEIGHT 800


namespace zxd
{

GLuint num_knots = 2;
GLuint knot_index = 0;

kcip kci_slices;
kcip kci_extrude_radius;
kcip kci_num_faces;

glm::mat4 m_mat;
glm::mat4 v_mat;
glm::mat4 p_mat;

GLuint slices;
GLfloat extrude_radius;
GLuint num_faces;

blinn_program prg;
light_vector lights;
light_model lm;
material mtl;
geometry_base knot;

class knot_app : public app
{
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "knot_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    prg.init();
    p_mat = glm::perspective(fpi2, wnd_aspect(), 0.1f, 1000.0f);
    v_mat = zxd::isometric_projection(200);
    set_v_mat(&v_mat);

    light_source l0;
    l0.position = vec4(-1, 1, 1, 0);
    l0.diffuse = vec4(1);
    l0.specular = vec4(0.8);
    lights.push_back(l0);

    lm.ambient = vec4(0.2);
    lm.local_viewer = true;

    mtl.diffuse = vec4(0.8, 0.8, 0.8, 1);
    mtl.ambient = vec4(0.2);
    mtl.specular = vec4(0.5);
    mtl.shininess = 50;

    prg.bind_lighting_uniform_locations(lights, lm, mtl);

    auto callback = std::bind(std::mem_fn(&knot_app::reset_knot), this, std::placeholders::_1);
    kci_slices = m_control.add(GLFW_KEY_Q, 1000, 10, 1000000, 10, callback);
    kci_num_faces = m_control.add(GLFW_KEY_W, 6, 3, 256, 1, callback);
    kci_extrude_radius = m_control.add<GLfloat>(GLFW_KEY_E, 3, 0.01, 100, 1, callback);

    reset_knot();
  }

  vec3 rpt_to_xyz(GLfloat r, GLfloat phi, GLfloat theta)
  {
    GLfloat x = r * cos(phi) * cos(theta);
    GLfloat y = r * cos(phi) * sin(theta);
    GLfloat z = r * sin(phi);
    return vec3(x, y, z);
  }

  vec3 rpt_beta4(GLfloat beta, GLfloat radius)
  {
    GLfloat r = 0.8 + 1.6 * sin(6 * beta);
    r *= radius;
    GLfloat theta = 2 * beta;
    GLfloat phi = 0.6 * fpi * sin(12 * beta);
    return vec3(r, phi, theta);
  }

  vec3 rpt_beta5(GLfloat beta, GLfloat radius)
  {
    GLfloat r = 1.2 * 0.6 * sin(0.5 * fpi + 6 * beta);
    r *= radius;
    GLfloat theta = 4 * beta;
    GLfloat phi = 0.2 * fpi * sin(6 * beta);
    return vec3(r, phi, theta);
  }

  void build_rpt_knot(std::function<vec3 (float, float)> rpt_func)
  {
    GLfloat radius = 100;

    GLfloat step_beta = fpi / slices;
    
    vec3_vector line_strip;
    line_strip.reserve(slices + 1);
    for (int i = 0; i < slices; ++i)
    {
      GLfloat beta = step_beta * i;
      vec3 rpt = rpt_func(beta, radius);
      line_strip.push_back(rpt_to_xyz(rpt.x, rpt.y, rpt.z));
    }
    line_strip.push_back(line_strip.front());
    update_geometry(line_strip);
  }

  void reset_knot(const kci* kci = 0)
  {
    slices = kci_slices->get_int();
    num_faces = kci_num_faces->get_int();
    extrude_radius = kci_extrude_radius->get_float();

    if(knot_index == 0)
    {
      build_rpt_knot(std::bind(std::mem_fn(&knot_app::rpt_beta4), 
            this, std::placeholders::_1, std::placeholders::_2));
    }
    else if(knot_index == 1)
    {
      build_rpt_knot(std::bind(std::mem_fn(&knot_app::rpt_beta5), 
            this, std::placeholders::_1, std::placeholders::_2));
    }

  }

  void update_geometry(const vec3_vector& line_strip)
  {
    vec3_vector vertices = extrude_along_line_strip(line_strip, extrude_radius, num_faces);

    knot.set_attrib_array(0, std::make_unique<vec3_array>(vertices));

    knot.remove_primitive_sets(0, knot.get_num_primitive_set());

    GLuint strip_size = knot.num_vertices() / num_faces;;
    for (int i = 0; i < num_faces; ++i)
    {
      knot.add_primitive_set(std::make_shared<draw_arrays>(
        GL_TRIANGLE_STRIP, strip_size * i, strip_size));
    }

    smooth(knot);
  }

  virtual void update() {}

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);

    prg.use();
    prg.update_lighting_uniforms(lights, lm, mtl, v_mat);
    prg.update_uniforms(mat4(1), v_mat, p_mat);

    knot.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ: slices " << slices << std::endl;
    ss << "wW: num faces : " << num_faces << std::endl;
    ss << "eE: extrude radius : " << extrude_radius << std::endl;
    ss << "rR: knot : " << knot_index << std::endl;
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
        case GLFW_KEY_R:
          knot_index = ++knot_index % num_knots;
          reset_knot();
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
  zxd::knot_app app;
  app.run();
}
