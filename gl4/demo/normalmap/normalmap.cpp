#include "app.h"

#include <sstream>

#include "bitmap_text.h"
#include "common.h"
#include "light.h"
#include "quad.h"
#include "texutil.h"
#include "stream_util.h"
#include "objman.h"

namespace zxd
{

quad q;
glm::mat4 m_mat(1);
glm::mat4 v_mat;
glm::mat4 p_mat;

struct use_normal_map_view_program : public zxd::program
  {
  GLint ul_normal_map;
  GLint ul_mv_mat_it;
  GLint ul_mv_mat;
  GLint ul_mvp_mat;

  virtual void update_uniforms(
    const mat4& p_mat, const mat4& v_mat, const mat4& m_mat, GLuint tui)
  {
    mat4 mv_mat = v_mat * m_mat;
    mat4 mv_mat_it = glm::inverse(glm::transpose(mv_mat));
    mat4 mvp_mat = p_mat * mv_mat;

    glUniformMatrix4fv(ul_mv_mat_it, 1, 0, value_ptr(mv_mat_it));
    glUniformMatrix4fv(ul_mv_mat, 1, 0, value_ptr(mv_mat));
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
    glUniform1i(ul_normal_map, tui);
  }
  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/use_normalmap_view.vs.glsl");
    string_vector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 1\n");
    sv.push_back(stream_util::read_resource("shader4/blinn.frag"));
    attach(GL_FRAGMENT_SHADER, sv, "shader4/use_normalmap_view.fs.glsl");

    name("use_normalmap_view");
  }
  virtual void bind_uniform_locations()
  {
    // uniform_location(&ul_eye, "eye");
    uniform_location(&ul_mv_mat, "mv_mat");
    uniform_location(&ul_mv_mat_it, "mv_mat_it");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_normal_map, "normal_map");
  }

  virtual void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
    bind_attrib_location(2, "texcoord");
    bind_attrib_location(3, "tangent");
  };
} prg0;

struct use_normal_map_object_progrm : public program
{
  GLint ul_normal_map;
  GLint ul_m_camera;
  GLint ul_mvp_mat;

  virtual void update_uniforms(
    const mat4& p_mat, const mat4& v_mat, const mat4& m_mat, GLuint tui)
  {
    mat4 mv_mat = v_mat * m_mat;
    mat4 mvp_mat = p_mat * mv_mat;
    mat4 mv_mat_i = glm::inverse(mv_mat);
    vec3 camera = vec3(glm::column(mv_mat_i, 3));

    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
    glUniform3fv(ul_m_camera, 1, value_ptr(camera));
    glUniform1i(ul_normal_map, tui);
  }

  virtual void attach_shaders()
  {
    string_vector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 1\n");
    attach(GL_VERTEX_SHADER, sv, "shader4/use_normalmap_model.vs.glsl");
    sv.push_back(stream_util::read_resource("shader4/blinn.frag"));
    attach(GL_FRAGMENT_SHADER, sv, "shader4/use_normalmap_model.fs.glsl");

    name("use_normalmap_model");
  }
  virtual void bind_uniform_locations()
  {
    // uniform_location(&ul_eye, "eye");
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_normal_map, "normal_map");
    uniform_location(&ul_m_camera, "m_camera");
  }
  virtual void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
    bind_attrib_location(2, "texcoord");
    bind_attrib_location(3, "tangent");
  };
} prg1;

GLuint normal_map;
std::vector<zxd::light_source> lights;
zxd::light_model light_model;
zxd::material material;
GLint light_space = 0;  // 0 : view, 1 : object

class normal_map_app : public app
{
protected:

public:
  normal_map_app() {}

  virtual void init_info()
  {
    app::init_info();
    m_info.title = "hello world";
    m_info.samples = 4;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    p_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 20.0f);
    v_mat = glm::lookAt(vec3(0, -5, 5), vec3(0.0f), vec3(0, 1, 0));
    prg0.init();
    prg1.init();

    q.include_normal(true);
    q.include_texcoord(true);
    q.include_tangent(true);
    q.build_mesh();

    fipImage normal_image = zxd::fipLoadResource("texture/bricks.bmp");

    glGenTextures(1, &normal_map);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, normal_image.getWidth(),
      normal_image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      normal_image.accessPixels());

    // light
    zxd::light_source dir_light;
    dir_light.position = vec4(1, -1, 1, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);
    dir_light.linear_attenuation = 1.0f;

    lights.push_back(dir_light);

    light_model.local_viewer = 1;

    // material
    material.ambient = vec4(0.2);
    material.diffuse = vec4(0.8);
    material.specular = vec4(1.0);
    material.shininess = 50;

    bind_uniform_locations(prg0);

    set_v_mat(&v_mat);

    auto mvpw_mat = compute_window_mat(0, 0, wnd_width(), wnd_height()) *
                    p_mat * v_mat * m_mat;
    auto pos = mvpw_mat[3];
    auto wpos = vec2(pos) / pos.w;
    auto handler = std::make_shared<trackball_objman>(&m_mat, &v_mat, wpos);
    add_input_handler(handler);
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, normal_map);
    if (light_space == 0)
    {
      glUseProgram(prg0);
      prg0.update_uniforms(p_mat, v_mat, m_mat, 0);

      for (int i = 0; i < lights.size(); ++i)
      {
        lights[i].update_uniforms(v_mat);
      }
    } else
    {
      glUseProgram(prg1);
      prg1.update_uniforms(p_mat, v_mat, m_mat, 0);

      auto m_mat_i = inverse(m_mat);
      for (int i = 0; i < lights.size(); ++i)
      {
        lights[i].update_uniforms(m_mat_i);
      }
    }
    light_model.update_uniforms();
    material.update_uniforms();
    q.draw();

    //glDisable(GL_SCISSOR_TEST);
    //glBindTexture(GL_TEXTURE_2D, normal_map);
    //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    glViewport(0, 0, wnd_width(), wnd_height());
    glEnable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : lighting space : " << (light_space == 0 ? "view" : "object")
       << std::endl;
    ss << "w : local viewer " << static_cast<GLint>(light_model.local_viewer)
       << std::endl;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
  }

  virtual void bind_uniform_locations(zxd::program &program)
  {
    light_model.bind_uniform_locations(program.object, "lm");
    for (int i = 0; i < lights.size(); ++i)
    {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(program.object, ss.str());
    }
    material.bind_uniform_locations(program.object, "mtl");
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
          light_space ^= 1;
          if (light_space == 0)
          {
            bind_uniform_locations(prg0);
          } else
          {
            bind_uniform_locations(prg1);
          }

          break;
        case GLFW_KEY_W:
          light_model.local_viewer ^= 1;
          break;

        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }

  virtual void glfw_mouse_button(
    GLFWwindow *wnd, int button, int action, int mods)
  {
    app::glfw_mouse_button(wnd, button, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y)
  {
    app::glfw_mouse_move(wnd, x, y);
  }

  virtual void glfw_mouse_wheel(
    GLFWwindow *wnd, double xoffset, double yoffset)
  {
    app::glfw_mouse_wheel(wnd, xoffset, yoffset);
  }
};
}

int main(int argc, char *argv[])
{
  zxd::normal_map_app app;
  app.run();
}
