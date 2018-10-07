#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include "dict_script.h"
#include "super_shape_3d.h"
#include "glenumstring.h"
#include <sstream>
#define RADIUS 1

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

twod_program0 prg;
super_shape_3d shape;
std::string script_file = "supershape3d.txt";
dict_script dict;
GLenum polygon_mode = GL_LINE;

class supershape_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "supershape_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    prg.init();
    prg.p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
    prg.v_mat = glm::lookAt(glm::vec3(5, -5, 5), vec3(0), pza);
    set_v_mat(&prg.v_mat);

    dict.read(script_file);
    dict.track();
    update_shape();

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
  }

  void update_shape()
  {
    dict.update();
    shape.radius(RADIUS);
    shape.n1(dict.get("n1", 1));
    shape.n2(dict.get("n2", 1));
    shape.n3(dict.get("n3", 1));
    shape.m(dict.get("m", 1));
    shape.a(dict.get("a", 1));
    shape.b(dict.get("b", 1));
    shape.slice(dict.get("slice", 32));
    shape.stack(dict.get("stack", 32));
    shape.longitude_start(glm::radians(dict.get("longitude_start", -180.0f)));
    shape.longitude_end(glm::radians(dict.get("longitude_end", 180.0f)));
    shape.latitude_start(glm::radians(dict.get("latitude_start", -180.0f)));
    shape.latitude_end(glm::radians(dict.get("latitude_end", 180.0f)));
    shape.build_mesh();
  }

  virtual void update() 
  {
    if(dict.changed())
      update_shape();
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glPolygonMode(GL_FRONT_AND_BACK, polygon_mode);
    prg.use();

    mat4 mvp_mat = prg.p_mat * prg.v_mat;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4f(prg.ul_color, 1, 1, 1, 1);
    shape.draw();

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : GL_DEPTH_TEST : " << (glIsEnabled(GL_DEPTH_TEST) ? 1 : 0) << std::endl;
    ss << "w : GL_CULL_FACE : " << (glIsEnabled(GL_CULL_FACE) ? 1 : 0) << std::endl;
    ss << "e : GL_POLYGON_MODE : " << gl_polygon_mode_to_string(polygon_mode) << std::endl;;
    ss << "n1 : " << shape.n1() << std::endl;
    ss << "n2 : " << shape.n2() << std::endl;
    ss << "n3 : " << shape.n3() << std::endl;
    ss << "m : " << shape.m() << std::endl;
    ss << "a : " << shape.a() << std::endl;
    ss << "b : " << shape.b() << std::endl;
    ss << "slice : " << shape.slice() << std::endl;
    ss << "stack : " << shape.stack() << std::endl;
    ss << "longitude start : " << shape.longitude_start() << std::endl;
    ss << "longitude end : " << shape.longitude_end() << std::endl;
    ss << "latitude start : " << shape.latitude_start() << std::endl;
    ss << "latitude end : " << shape.latitude_end() << std::endl;
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

        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[]) {
  //if(argc < 2)
  //{
    //std::cout << "missing script file" << std::endl;
    //return 0;
  //}

  //zxd::script_file = argv[1];

  zxd::supershape_app app;
  app.run();
}
