#include <sstream>
#include <algorithm>

#include <app.h>
#include <common_program.h>
#include <sphere.h>
#include <quad.h>

namespace zxd {

const GLint width = 800;
const GLint height = 800;
const GLint num_models = 100;
const GLfloat scene_radius = 30;
const GLfloat mirror_size = scene_radius * 2;

mat4 p_mat;
mat4 v_mat;
mat4 v_mat_r;
GLuint depth_map;

blinn_program scene_prg;
blinn_program mirror_scene_prg;
lightless_program ll_prg;
blinn_program mirror_prg;

light_model lm;
std::vector<zxd::light_source> lights;
material mtl;
material mirror_mtl;

std::vector<mat4> models;
sphere geom;

quad q;
mat4 q_mat;
vec4 q_plane;

enum mirror_technique
{
  mt_scene_mirror_reflect, // render scene, render mirror color, depth, depth
                           // tested stencil, backup depth, clear depth, render reflected
                           // scene, render backup depth
  mt_mirror_reflect_depth_scene // render mirror color, stencil, render reflected scene,
                                // render mirror depth. render normal scene
};

const char* mirror_techniques[] = {
  "scene_mirror_reflect",
  "mirror_reflect_depth_scene"
};

kcip technique;
kcip random_plane;

class depth_of_field_app : public app {
private:

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) override;
  void glfw_mouse_button(GLFWwindow *wnd, int button, int action,
    int mods) override;

  void glfw_mouse_move(GLFWwindow *wnd, double x, double y) override;
};

void depth_of_field_app::init_info() {
  app::init_info();
  m_info.title = "depth_of_field_app";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

GLuint create_texture()
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return tex;
}

void random_plane_callback(const kci* p)
{
  q_mat = glm::translate(glm::ballRand(30.0f)) *
          glm::rotate(glm::linearRand(0.0f, f2pi), glm::sphericalRand(1.0f));
  q_plane = glm::transpose(glm::inverse(q_mat)) * vec4(0, 0, 1, 0);
  q_mat = q_mat * glm::scale(vec3(mirror_size));
}

void depth_of_field_app::create_scene()
{
  // prg.init();
  models.reserve(num_models);
  std::generate_n(std::back_inserter(models), num_models, []() -> mat4 {
    auto pos = glm::ballRand(scene_radius);
    return glm::translate(pos);
  });
  // models.push_back(glm::translate(vec3(5, 0, 5)));
  // models.push_back(glm::translate(vec3(-5, 0, -5)));

  light_source light;
  light.position = vec4(1, -1, 1, 0.0f);
  light.ambient = vec4(0.2);
  light.diffuse = vec4(0.8);
  light.specular = vec4(0.5);
  lights.push_back(light);

  mtl.shininess = 50;
  mtl.specular = vec4(0.8, 0.8, 0.8, 1.0f);

  mirror_mtl.shininess = 50;
  mirror_mtl.diffuse = vec4(0.75f, 0.75f, 0.75f, 1.0f);
  mirror_mtl.specular = vec4(1.0f);

  lm.local_viewer = true;

  scene_prg.init();

  mirror_scene_prg.with_clipplane0 = GL_TRUE;
  mirror_scene_prg.init();

  ll_prg.init();

  mirror_prg.with_texcoord = true;
  mirror_prg.init();

  depth_map = create_texture();
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0,
    GL_DEPTH_COMPONENT, GL_FLOAT, 0);

  geom.slice(64);
  geom.stack(64);
  geom.build_mesh({attrib_semantic::vertex, attrib_semantic::normal});

  p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 280.0f);
  v_mat = zxd::isometric_projection(60);
  set_v_mat(&v_mat);

  q.build_mesh({attrib_semantic::vertex, attrib_semantic::normal});
  // q_mat = glm::rotate(glm::linearRand(0.0f, f2pi), glm::sphericalRand(1.0f));
  q_mat = glm::mat4(1.0f);
  q_plane = q_mat * vec4(0, 0, 1, 0); // q_mat is orthogonal
  q_mat = q_mat * glm::scale(vec3(mirror_size));

  technique = m_control.add_enum('Q', {0, 1});
  random_plane = m_control.add_bool('W', true, random_plane_callback);
}

void depth_of_field_app::update() {}

// note that if you turn on blend when rendering reflected scene, scene
// element will blend with each other, if that's a trouble, you might need to
// use gbuffer to defer blend between the final reflected scene and the mirror.
void render_scene(const mat4& view_mat, bool mirror = false,
  bool clear_depth = false, bool blend = true)
{
  blinn_program* prg = mirror ? &mirror_scene_prg : &scene_prg;
  prg->use();

  if (mirror)
  {
    if (clear_depth)
    {
      glClear(GL_DEPTH_BUFFER_BIT);
    }
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_CLIP_DISTANCE0);
    glStencilFunc(GL_EQUAL, 0x1, 0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
    if (blend)
    {
      glEnable(GL_BLEND);
      glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
      glBlendColor(0, 0, 0, 0.5f);
    }
    static int i = 0;
    auto view_mat_it = glm::transpose(glm::inverse(view_mat));
    auto eye_plane = view_mat_it * q_plane;
    bool reversed = false;
    if (dot(vec3(0, 0, -1), vec3(eye_plane)) < 0)
    {
      eye_plane *= -1.0f;
    }

    glUniform4fv(prg->ul_clipplane0, 1, value_ptr(eye_plane));
  }

  prg->bind_lighting_uniform_locations(lights, lm, mtl);
  prg->update_lighting_uniforms(lights, lm, mtl, view_mat);
  for (const auto& m_mat : models)
  {
    prg->update_uniforms(m_mat, view_mat, p_mat);
    geom.draw();
  }

  if (mirror)
  {
    // glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glDisable(GL_STENCIL_TEST);
    glDisable(GL_CLIP_DISTANCE0);
    glDisable(GL_BLEND);
  }

}


void render_mirror_stencil(bool write_depth)
{
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0x1, 0x1);
  if (!write_depth)
    glDepthMask(GL_FALSE);
  // only write stencil if depth pass successed
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

  scene_prg.use();
  scene_prg.bind_lighting_uniform_locations(lights, lm, mirror_mtl);
  scene_prg.update_lighting_uniforms(lights, lm, mirror_mtl, v_mat);
  scene_prg.update_uniforms(q_mat, v_mat, p_mat);
  q.draw();

  glDisable(GL_STENCIL_TEST);
  if (!write_depth)
    glDepthMask(GL_TRUE);
}

void render_mirror_depth()
{
  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

  ll_prg.use();
  auto mvp_mat = p_mat * v_mat * q_mat;
  glUniformMatrix4fv(ll_prg.ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
  q.draw();

  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
}

void depth_of_field_app::display() {

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  auto tech = technique->get_int_enum();

  if (tech == mt_scene_mirror_reflect)
  {
    // render normal scene, write color, depth
    render_scene(v_mat);

    // render mirror, write color, depth, stencil. stencil must be depth tested.
    render_mirror_stencil(true);

    // backup depth, you can skip this if you don't need it later
    glBindTexture(GL_TEXTURE_2D, depth_map);
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, width, height);

    // clear depth, render reflected scene
    render_scene(v_mat * reflect(q_plane), true, true);

    // copy depth back
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
    draw_depth_quad(depth_map);

    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
  }
  else
  {
    // render mirror, write color, stencil.
    render_mirror_stencil(false);

    // render reflected scene
    render_scene(v_mat * reflect(q_plane), true, false);

    // render mirror depth, this is important, it stop normal scene element
    // that's behind the mirror from erasing reflection.
    // If you are sure there has no scene element behind the mirror, you can put
    // render_scene before render_mirror_color_stencil
    render_mirror_depth();

    // render normal scene, write color, depth.
    render_scene(v_mat);
  }

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "Q : technique : " << mirror_techniques[tech] << std::endl;
  ss << "W : random_plane : " << q_plane << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void depth_of_field_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
}

void depth_of_field_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void depth_of_field_app::glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                               int mods)
{
  app::glfw_mouse_button(wnd, button, action, mods);
}

void depth_of_field_app::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
}

}

int main(int argc, char *argv[]) {
  zxd::depth_of_field_app app;
  app.run();
}

