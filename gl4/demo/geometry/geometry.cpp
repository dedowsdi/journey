#include "app.h"

#include <sstream>

#include "sphere.h"
#include "common.h"
#include "light.h"
#include "glenumstring.h"
#include "cuboid.h"
#include "cone.h"
#include "stateutil.h"
#include "cylinder.h"
#include "torus.h"
#include "xyplane.h"
#include "disk.h"
#include "bezier.h"
#include "nurb.h"
#include "beziersurface.h"
#include "points.h"
#include "nurbsurface.h"
#include "axes.h"
#include "teardrop.h"
#include "capsule3d.h"
#include "icosahedron.h"

namespace zxd
{

GLuint diffuse_map;
GLuint diffuse_map1d;
GLboolean render_normal = GL_FALSE;

glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 mvp_mat;

blinn_program blinn_prg;
normal_viewer_program nv_prg;
vertex_color_program vc_prg;

vec3 camera_pos(0, -8, 8);
std::vector<zxd::light_source> lights;
zxd::light_model lm;
zxd::material mtl;
axes main_axis;
GLint init_index = 0;
std::shared_ptr<kci> index;

void render_blinn(geometry_base &gm, const vec3 &translation);
void render_texline(geometry_base &gm, const vec3 &translation);
void render_vertex_color(geometry_base &gm, const mat4 &mvp_mat);

struct texline_program : public zxd::program
{
  GLint ul_diffuse_map;
  GLint ul_mvp_mat;

  virtual void update_uniforms(const mat4& mvp_mat)
  {
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  }
  virtual void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/texline.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/texline.fs.glsl");
  }
  virtual void bind_uniform_locations()
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
  }

  virtual void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  };
} texline_prg;

class renderable
{
public:

  renderable(const std::shared_ptr<geometry_base>& drawable):
    m_drawable(drawable)
  {
  }

  virtual void draw(){}
  virtual ~renderable(){}

  const std::shared_ptr<geometry_base>& get_drawable() const
  {
    return m_drawable;
  }
  void set_drawable(const std::shared_ptr<geometry_base>& v) { m_drawable = v; }

protected:
  std::shared_ptr<geometry_base> m_drawable;
};

class blinn_renderable : public renderable
{
public:
  blinn_renderable(const std::shared_ptr<geometry_base>& drawable)
      : renderable(drawable)
  {
  }

  void draw() override
  {
    render_blinn(*m_drawable, vec3(0));
  }
};

template <typename T>
class nurb_surface_renderable : public renderable
{

public:
  nurb_surface_renderable(
    const std::shared_ptr<geometry_base>& drawable, const T* points)
      : renderable(drawable), m_points(points)
  {
  }

  void draw() override
  {
    render_blinn(*m_drawable, vec3(0));
    glPointSize(3);
    draw_points(*m_points, p_mat * v_mat);
    glPointSize(1);
  }

private:
  const T* m_points;
};

template <typename T>
class nurb_renderable : public renderable
{

public:
  nurb_renderable(
    const std::shared_ptr<geometry_base>& drawable, const T* points)
      : renderable(drawable), m_points(points)
  {
  }

  void draw() override
  {
    render_texline(*m_drawable, glm::vec3(0, -2.5, 0));
    glPointSize(3);
    draw_points(
      *m_points, p_mat * v_mat * glm::translate(glm::vec3(0, -2.5, 0)));
    glPointSize(1);
  }

private:
  const T* m_points;
};

std::vector<std::shared_ptr<renderable>> renderables;

template <typename T, typename... Args>
std::shared_ptr<T> add_blinn_renderable(Args&&... args)
{
  auto p = std::make_shared<T>(std::forward<Args>(args)...);
  p->include_normal(true);
  p->include_texcoord(true);
  p->build_mesh();

  renderables.push_back(std::make_shared<blinn_renderable>(p));
  return p;
}

class geometry_app : public app
{
protected:

public:
  geometry_app(){}

  virtual void init_info()
  {
    app::init_info();
    m_info.title = "geometry";
  }

  virtual void create_scene()
  {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);

    // light
    zxd::light_source dir_light;
    dir_light.position = vec4(1, -1, 1, 0);
    dir_light.diffuse = vec4(1, 1, 1, 1);
    dir_light.specular = vec4(1, 1, 1, 1);

    lights.push_back(dir_light);

    // material
    mtl.shininess = 50;
    mtl.specular = vec4(1.0, 1.0, 1.0, 1.0);
    mtl.ambient = vec4(0.8);

    // program
    blinn_prg.with_texcoord = 1;
    blinn_prg.init();
    p_mat = glm::perspective(glm::radians(45.0f), wnd_aspect(), 0.1f, 30.0f);
    v_mat = glm::lookAt(camera_pos, vec3(0, 0, 0), vec3(0, 0, 1));
    set_v_mat(&v_mat);

    blinn_prg.bind_lighting_uniform_locations(lights, lm, mtl);

    nv_prg.init();
    texline_prg.init();
    vc_prg.init();

    // geometry
    add_blinn_renderable<sphere>();
    add_blinn_renderable<cuboid>();
    add_blinn_renderable<cone>();
    add_blinn_renderable<cylinder>(1.0f, 0.5f);
    add_blinn_renderable<torus>();
    add_blinn_renderable<xyplane>(8, 8, 8);
    add_blinn_renderable<disk>();
    add_blinn_renderable<disk>(0.5, 1, 8, 8, fpi4, fpi2);
    add_blinn_renderable<teardrop>();
    add_blinn_renderable<capsule3d>();
    add_blinn_renderable<icosahedron>(
      1.5, 3, icosahedron::mesh_type::PAPER_UNWRAPPER);

    main_axis.include_normal(true);
    main_axis.include_color(true);
    main_axis.build_mesh();

    {
      vec3_vector2 vv;
      for (int i = 0; i < 5; ++i)
      {
        vec3_vector v;
        for (int j = 0; j < 5; ++j)
        {
          v.push_back(
            vec3(-1 + i * 0.5f, -1 + j * 0.5f, glm::linearRand(0.0f, 1.0f)));
        }
        vv.push_back(std::move(v));
      }

      auto surf = std::make_shared<bezier_surface>();
      surf->ctrl_points(vv);
      surf->include_normal(true);
      surf->include_texcoord(true);
      surf->build_mesh();

      renderables.push_back(
        std::make_shared<nurb_surface_renderable<vec3_vector2>>(surf, &surf->ctrl_points()));
    }

    {
      vec4_vector2 vv;
      for (int i = 0; i < 5; ++i)
      {
        vec4_vector v;
        for (int j = 0; j < 5; ++j)
        {
          v.push_back(
            vec4(-1 + i * 0.5f, -1 + j * 0.5f, glm::linearRand(0.0f, 1.0f), 1));
        }
        vv.push_back(std::move(v));
      }
      auto surf = std::make_shared<nurb_surface>();
      surf->ctrl_points(vv);
      surf->uniform_knots();
      surf->udegree(3);
      surf->vdegree(3);
      surf->include_normal(true);
      surf->include_texcoord(true);
      surf->build_mesh();

      renderables.push_back(
        std::make_shared<nurb_surface_renderable<vec4_vector2>>(surf, &surf->ctrl_points()));
    }

    {
      vec3_vector ctrl_points;
      ctrl_points.push_back(vec3(0));
      ctrl_points.push_back(vec3(1, 0, 1));
      ctrl_points.push_back(vec3(-1, 0, 2));
      ctrl_points.push_back(vec3(0, 0, 3));

      auto line = std::make_shared<bezier>();
      line->ctrl_points(ctrl_points);
      line->include_texcoord(true);
      line->build_mesh();

      renderables.push_back(
        std::make_shared<nurb_renderable<vec3_vector>>(line, &line->ctrl_points()));
    }

    {
      vec4_vector ctrl_points;
      ctrl_points.push_back(vec4(0, 0, 0, 1));
      ctrl_points.push_back(vec4(5, 0, 5, 5));
      ctrl_points.push_back(vec4(-5, 0, 10, 5));
      ctrl_points.push_back(vec4(0, 0, 3, 1));

      auto line = std::make_shared<nurb>();
      line->degree(3);
      line->ctrl_points(ctrl_points);
      line->uniform_knots();
      line->include_texcoord(true);
      line->build_mesh();

      renderables.push_back(
        std::make_shared<nurb_renderable<vec4_vector>>(line, &line->ctrl_points()));
    }

    // texture
    GLint image_width = 64;
    GLint image_height = 64;
    auto image = create_chess_image(image_width, image_height, 8, 8);

    glGenTextures(1, &diffuse_map);
    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0,
      GL_RGBA, GL_UNSIGNED_BYTE, &image.front());

    glGenTextures(1, &diffuse_map1d);
    glBindTexture(GL_TEXTURE_1D, diffuse_map1d);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, image_width, 0, GL_RGBA,
      GL_UNSIGNED_BYTE, &image.front());

    index = m_control.add<GLint>('I', init_index, 0, renderables.size() - 1, 1);
    index->set_loop(true);
  }

  virtual void update() {}

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, diffuse_map);
    blinn_prg.use();
    blinn_prg.update_lighting_uniforms(lights, lm, mtl, v_mat);

    // std::for_each(renderables.begin(), renderables.end(),
      // [](auto& renderable) -> void { renderable->draw(); });
    renderables[index->get_int()]->draw();
    render_vertex_color(main_axis, p_mat * v_mat);

    GLint cull_face;
    glGetIntegerv(GL_CULL_FACE_MODE, &cull_face);

    if (!m_display_help)
    {
      return;
    }

    GLint polygon_mode;
    glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
    std::stringstream ss;
    ss << "q : polygon mode : " << gl_polygon_mode_to_string(polygon_mode)
       << std::endl;
    ss << "w : cullface : " << GLint(glIsEnabled(GL_CULL_FACE)) << std::endl;
    ss << "e : depth : " << GLint(glIsEnabled(GL_DEPTH_TEST)) << std::endl;
    ss << "r : cullface : " << gl_cull_face_mode_to_string(cull_face)
       << std::endl;
    ss << "u : render normal : " << static_cast<GLint>(render_normal)
       << std::endl;
    ss << "i : index : " << index->get_int() << std::endl;

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    cap_guard g0(GL_BLEND, GL_TRUE);
    cap_guard g1(GL_CULL_FACE, GL_TRUE);
    m_text.print(ss.str(), 10, 492);
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
          GLint polygon_mode;
          glGetIntegerv(GL_POLYGON_MODE, &polygon_mode);
          glPolygonMode(
            GL_FRONT_AND_BACK, GL_POINT + (polygon_mode - GL_POINT + 1) % 3);
        } break;
        case GLFW_KEY_W:
          {
          if (glIsEnabled(GL_CULL_FACE))
          {
            glDisable(GL_CULL_FACE);
          } else
          {
            glEnable(GL_CULL_FACE);
          }
        } break;
        case GLFW_KEY_E:
          {
          if (glIsEnabled(GL_DEPTH_TEST))
          {
            glDisable(GL_DEPTH_TEST);
          } else
          {
            glEnable(GL_DEPTH_TEST);
          }
        } break;
        case GLFW_KEY_R:
          {
          GLint cull_face;
          glGetIntegerv(GL_CULL_FACE_MODE, &cull_face);
          if (cull_face == GL_FRONT)
          {
            glCullFace(GL_BACK);
          } else if (cull_face == GL_BACK)
          {
            glCullFace(GL_FRONT_AND_BACK);
          } else
          {
            glCullFace(GL_FRONT);
          }
        } break;

        case GLFW_KEY_U:
          render_normal ^= 1;
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

void render_blinn(geometry_base &gm, const vec3 &translation)
{
  mat4 m_mat = glm::translate(translation);
  blinn_prg.update_uniforms(m_mat, v_mat, p_mat);
  gm.draw();

  // bad performance
  if (render_normal)
  {
    nv_prg.use();
    nv_prg.update_uniforms(m_mat, v_mat, p_mat);
    gm.draw();
  }
}

void render_texline(geometry_base &gm, const vec3 &translation)
{
  texline_prg.use();

  glUniform1i(texline_prg.ul_diffuse_map, 0);
  mat4 m_mat = glm::translate(translation);
  texline_prg.update_uniforms(p_mat * v_mat * m_mat);
  gm.draw();
}

void render_vertex_color(geometry_base &gm, const mat4 &mvp_mat)
{
  vc_prg.use();
  vc_prg.update_uniforms(mvp_mat);
  gm.draw();
}

}

int main(int argc, char *argv[])
{
  if (argc > 1)
  {
    zxd::init_index = std::stoi(argv[1]);
  }
  zxd::geometry_app app;
  app.run();
}
