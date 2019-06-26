#include <sstream>

#include "app.h"
#include "bitmap_text.h"
#include "common_program.h"
#include "dict_script.h"
#include "super_shape_3d.h"
#include "glenumstring.h"
#include "common.h"
#include "stream_util.h"

#define RADIUS 1

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

glm::mat4 v_mat;
glm::mat4 p_mat;

normal_viewer_program nv_prg;
blinn_program prg;
light_vector lights;
light_model lm;
material mtl;
GLuint diffuse_map;

super_shape_3d shape;
std::string script_file = "../challenge/supershape3d/supershape3d.txt";
dict_script dict;
GLenum polygon_mode = GL_LINE;
bool show_normal = true;

class supershape_app : public app
  {
protected:

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "supershape_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);


    nv_prg.smooth_normal = GL_TRUE;
    nv_prg.init();

    prg.with_texcoord = GL_TRUE;
    prg.init();
    p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
    v_mat = glm::lookAt(glm::vec3(5, -5, 5), vec3(0), pza);
    set_v_mat(&v_mat);

    glGenTextures(1, &diffuse_map);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    auto img = create_chess_image(64, 64, 8, 8);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 64, 64, 0, GL_RGBA, GL_UNSIGNED_BYTE, &img.front());

    // setup light
    light_source l0;
    l0.position = vec4(1, 1, 1, 0);
    l0.diffuse = vec4(0.8, 0.8, 0.8, 1);
    l0.specular = vec4(0.8);
    lights.push_back(l0);
    
    lm.local_viewer = true;

    mtl.diffuse = vec4(0.8, 0.8, 0.8, 1);
    mtl.specular = vec4(0.5);
    mtl.shininess = 50;
    mtl.ambient = vec4(1);

    prg.bind_lighting_uniform_locations(lights, lm, mtl);

    dict.read(stream_util::get_resource(script_file));
    dict.track();
    update_shape();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glfwSetWindowPos(m_wnd, 1000, 20);
  }

  void update_shape()
  {
    dict.update();
    shape.radius(RADIUS);
    shape.type(super_shape_3d::shape_type_from_string(dict.get("type", "")));
    shape.n1(dict.get("n1", 1));
    shape.n2(dict.get("n2", 1));
    shape.n3(dict.get("n3", 1));
    shape.m(dict.get("m", 1));
    shape.a(dict.get("a", 1));
    shape.b(dict.get("b", 1));
    if(shape.type() == super_shape_3d::ST_SPHERE)
    {
      shape.slice(dict.get("slice", 32));
      shape.stack(dict.get("stack", 32));
    }
    else
    {
      shape.side(dict.get("side", 32));
      shape.ring(dict.get("ring", 32));
    }
    shape.theta_start(glm::radians(dict.get("theta_start", -180.0f)));
    shape.theta_end(glm::radians(dict.get("theta_end", 180.0f)));
    shape.phi_start(glm::radians(dict.get("phi_start", -180.0f)));
    shape.phi_end(glm::radians(dict.get("phi_end", 180.0f)));
    shape.include_texcoord(true);
    shape.include_normal(true);
    shape.build_mesh();
  }

  virtual void update() 
  {
    if(dict.changed())
      update_shape();
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
    prg.use();

    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    prg.update_uniforms(mat4(1), v_mat, p_mat);
    prg.update_lighting_uniforms(lights, lm, mtl, v_mat);
    shape.draw();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    if(show_normal)
    {
      nv_prg.use();
      nv_prg.update_uniforms(mat4(1), v_mat, p_mat);
      shape.draw();
    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : GL_DEPTH_TEST : " << (glIsEnabled(GL_DEPTH_TEST) ? 1 : 0) << std::endl;
    ss << "w : GL_CULL_FACE : " << (glIsEnabled(GL_CULL_FACE) ? 1 : 0) << std::endl;
    ss << "e : GL_POLYGON_MODE : " << gl_polygon_mode_to_string(polygon_mode) << std::endl;;
    ss << "r : show normal : " << show_normal << std::endl;;
    ss << "super shape type : " << shape.shape_type_to_string(shape.type()) << std::endl;
    ss << "n1 : " << shape.n1() << std::endl;
    ss << "n2 : " << shape.n2() << std::endl;
    ss << "n3 : " << shape.n3() << std::endl;
    ss << "m : " << shape.m() << std::endl;
    ss << "a : " << shape.a() << std::endl;
    ss << "b : " << shape.b() << std::endl;
    if(shape.type() == super_shape_3d::ST_SPHERE)
    {
      ss << "slice : " << shape.slice() << std::endl;
      ss << "stack : " << shape.stack() << std::endl;
    }
    else
    {
      ss << "side : " << shape.side() << std::endl;
      ss << "ring : " << shape.ring() << std::endl;
    }
    ss << "theta start : " << shape.theta_start() << std::endl;
    ss << "theta end : " << shape.theta_end() << std::endl;
    ss << "phi start : " << shape.phi_start() << std::endl;
    ss << "phi end : " << shape.phi_end() << std::endl;
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
          if(glIsEnabled(GL_DEPTH_TEST))
            glDisable(GL_DEPTH_TEST);
          else
            glEnable(GL_DEPTH_TEST);
          break;

        case GLFW_KEY_W:
          if(glIsEnabled(GL_CULL_FACE))
            glDisable(GL_CULL_FACE);
          else
            glEnable(GL_CULL_FACE);
          break;

        case GLFW_KEY_E:
          polygon_mode = GL_POINT + (polygon_mode + 1 - GL_POINT)%3;
          break;

        case GLFW_KEY_R:
          show_normal = !show_normal;
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
  //if(argc < 2)
  //{
    //std::cout << "missing script file" << std::endl;
    //return 0;
  //}

  //zxd::script_file = argv[1];

  zxd::supershape_app app;
  app.run();
}
