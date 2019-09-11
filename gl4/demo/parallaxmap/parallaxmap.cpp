/*
 * Parallax map.
 *
 * Normalmap tried to use coarse grained geometry instead of fine grained
 * geometry, it reserved normals of fined grained geometry in normal map(tangent
 * space), but it doesn't reserve the surface detail, tangent space surface is
 * always flat, which means the vertex point you look at in a coarsed grained
 * geometry can not be the same vertex point as you llok at it's fine grained
 * cousin. Parallax map is used to overcome this problem, it was applied on
 * trangent space, with an extra depth map(invert of height map).  In tangent
 * space xy component of viewdir is aligned to uv, and the offset is propotion
 * to depth (in a positive random way), so we can approximate the offset based
 * on depth, and apply the offset to texcoord to retrieve normal from normal
 * map.
 *
 */
#include <app.h>
#include <bitmap_text.h>
#include <sstream>
#include <common.h>
#include <light.h>
#include <quad.h>
#include <texutil.h>
#include <program.h>
#include <stream_util.h>

namespace zxd
{

glm::mat4 m_mat;
glm::mat4 v_mat;
glm::mat4 p_mat;

struct parallax_program : public program
  {
  GLint ul_normal_map;
  GLint ul_diffuse_map;
  GLint ul_depth_map;
  GLint ul_m_camera;
  GLint ul_height_scale;
  GLint ul_mvp_mat;

  GLuint parallax_method{0};

  virtual void update_uniforms(const mat4 &m_mat)
  {
    mat4 mv_mat = v_mat * m_mat;
    // mat4 mv_mat_i = glm::inverse(mv_mat);
    // mat4 m_mat_i = glm::inverse(m_mat);
    mat4 mvp_mat = p_mat * mv_mat;
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
  };
  virtual void attach_shaders()
  {
    string_vector sv;
    sv.push_back("#version 430 core\n #define LIGHT_COUNT 1\n");
    attach(GL_VERTEX_SHADER, sv, "shader4/parallaxmap.vs.glsl");
    std::stringstream ss;
    ss << "#define PARALLAX_METHOD " << parallax_method << std::endl;
    sv.push_back(ss.str());
    sv.push_back(stream_util::read_resource("shader4/blinn.frag"));
    attach(GL_FRAGMENT_SHADER, sv, "shader4/parallaxmap.fs.glsl");

    name("parallaxmap");
  }

  virtual void bind_uniform_locations()
  {
    ul_mvp_mat = get_uniform_location("mvp_mat");
    ul_normal_map = get_uniform_location("normal_map");
    ul_diffuse_map = get_uniform_location("diffuse_map");
    ul_depth_map = get_uniform_location("depth_map");
    ul_m_camera = get_uniform_location("m_camera");
    ul_height_scale = get_uniform_location("height_scale");
  }
  virtual void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "normal");
    bind_attrib_location(2, "texcoord");
    bind_attrib_location(3, "tangent");
  };
} prg;

GLuint normal_map, diffuse_map, depth_map;
std::vector<zxd::light_source> lights;
zxd::light_model light_model;
zxd::material material;
quad q;
GLfloat height_scale = 0.05f;
std::string parallax_methods[] = {"parallax_occlusion_map", "parallaxSteepMap",
  "parallax_map_with_offset", "parallaxMapWithoutOffset"};

class normal_map_app : public app
{
protected:

public:
  normal_map_app() {}

  virtual void init_info()
  {
    app::init_info();
    m_info.title = "hello world";
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    prg.parallax_method = 3;
    prg.init();
    p_mat = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 20.0f);
    v_mat = glm::lookAt(vec3(0, -5, 5), vec3(0.0f), vec3(0, 1, 0));

    // init quad
    q.build_mesh({attrib_semantic::vertex, attrib_semantic::normal,
      attrib_semantic::texcoord, attrib_semantic::tangent});

    // load maps
    fipImage diffuse_image = zxd::fipLoadResource("texture/bricks2.jpg");
    fipImage normal_image =
      zxd::fipLoadResource("texture/bricks2_normal.jpg");
    fipImage depth_image = zxd::fipLoadResource("texture/bricks2_disp.jpg");

    glGenTextures(1, &diffuse_map);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, diffuse_image.getWidth(),
      diffuse_image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      diffuse_image.accessPixels());

    glGenTextures(1, &normal_map);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, normal_image.getWidth(),
      normal_image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      normal_image.accessPixels());

    glGenTextures(1, &depth_map);
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, depth_image.getWidth(),
      depth_image.getHeight(), 0, GL_BGR, GL_UNSIGNED_BYTE,
      depth_image.accessPixels());

    // light
    zxd::light_source dir_light;
    dir_light.position = vec4(0, -1, 1, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);
    dir_light.linear_attenuation = 1.0f;

    lights.push_back(dir_light);

    light_model.local_viewer = 1;

    // material
    material.ambient = vec4(0.2);
    material.diffuse = vec4(0.8);
    material.specular = vec4(0.8);
    material.shininess = 50;

    set_v_mat(&v_mat);
    bind_uniform_locations(prg);

  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // draw low quality mesh with normal map
    glUseProgram(prg);
    prg.update_uniforms(m_mat);
    glUniform1i(prg.ul_diffuse_map, 0);
    glUniform1i(prg.ul_normal_map, 1);
    glUniform1i(prg.ul_depth_map, 2);
    glUniform1f(prg.ul_height_scale, height_scale);

    mat4 mv_mat_i = glm::inverse(v_mat * m_mat);
    mat4 m_mat_i = glm::inverse(m_mat);

    // get camera model position
    vec3 camera = vec3(glm::column(mv_mat_i, 3));
    glUniform3fv(prg.ul_m_camera, 1, glm::value_ptr(camera));

    for (int i = 0; i < lights.size(); ++i)
    {
      lights[i].update_uniforms(m_mat_i);
    }
    light_model.update_uniforms();
    material.update_uniforms();

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, normal_map);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, depth_map);

    q.draw();

    glEnable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : parallax method : " << parallax_methods[prg.parallax_method]
       << std::endl;
    ss << "w : height_scale : " << height_scale << std::endl;
    ss << "fps : " << m_fps << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 25);
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
  }

  virtual void bind_uniform_locations(zxd::program &prg)
  {
    light_model.bind_uniform_locations(prg.get_object(), "lm");
    for (int i = 0; i < lights.size(); ++i)
    {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bind_uniform_locations(prg.get_object(), ss.str());
    }
    material.bind_uniform_locations(prg.get_object(), "mtl");
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
          {
          prg.parallax_method = (prg.parallax_method + 1) % 4;
          prg.clear();
          prg.init();
        } break;

          break;
        default:
          break;
      }
    }

    if (action == GLFW_PRESS || action == GLFW_REPEAT)
    {
      switch (key)
      {
        case GLFW_KEY_W:
          if (mods & GLFW_MOD_SHIFT)
          {
            height_scale -= 0.002;
            height_scale = glm::max(height_scale, 0.0f);
          } else
          {
            height_scale += 0.002;
          }

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
