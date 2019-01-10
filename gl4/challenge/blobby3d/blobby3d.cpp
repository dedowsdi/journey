#include <sstream>

#include "app.h"
#include "bitmap_text.h"
#include "icosahedron.h"
#include "common_program.h"
#include "geometry_util.h"

namespace zxd {

const GLint width = 640;
const GLint height = 640;

class program_name : public program
{
public:
  GLint ul_time;

protected:

  void attach_shaders()
  {
    attach(GL_COMPUTE_SHADER, "shader4/blobby3d.cs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_time, "time");
  }

} prg;

light_vector lights;
light_model lm;
material mtl;
blinn_program bprg;

class blobby3d_app : public app {
private:
  bitmap_text m_text;
  icosahedron m_sphere;
  GLuint m_mesh_buffer;

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) override;
};

void blobby3d_app::init_info() {
  app::init_info();
  m_info.title = "blobby3d_app";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
  m_info.samples = 16;
  m_info.wm.x = 100;
  m_info.wm.y = 100;
}

void blobby3d_app::create_scene() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  m_text.init();
  m_text.reshape(wnd_width(), wnd_height());

  light_source l;
  l.position = vec4(0, 0, 1, 0);
  l.diffuse = vec4(1);
  l.specular = vec4(1);
  lights.push_back(l);

  lm.ambient = vec4(0.2f);
  lm.local_viewer = true;

  mtl.diffuse = vec4(0.6f);
  mtl.specular = vec4(0.3f);
  mtl.ambient = vec4(0.5f);
  mtl.shininess = 20;

  prg.init();
  bprg.init();
  bprg.v_mat = zxd::isometric_projection(2);
  bprg.p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 100.0f);
  set_v_mat(&bprg.v_mat);
  bprg.bind_lighting_uniform_locations(lights, lm, mtl);

  m_sphere.include_normal(true);
  m_sphere.subdivisions(4);
  m_sphere.build_mesh();
  auto vertices = geometry_util::vec3_vector_to_vec4_vector(m_sphere.attrib_vec3_array(0)->get_vector());
  m_mesh_buffer = m_sphere.attrib_array(0)->buffer();

  glGenBuffers(1, &m_mesh_buffer);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_mesh_buffer);
  glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(vec4),
      glm::value_ptr(vertices.front()), GL_STATIC_COPY);
}

void blobby3d_app::update() {}

void blobby3d_app::display() {

  GLuint num_vertices = m_sphere.num_vertices();
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_mesh_buffer);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_sphere.attrib_array(0)->buffer());
  prg.use();
  glUniform1f(prg.ul_time, m_current_time);
  glDispatchCompute(ceil(num_vertices * 3 / 64.0), 1, 1);
  // TODO avoid sync
  m_sphere.attrib_array(0)->read_buffer();
  geometry_util::smooth(m_sphere);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  //prg.use();
  bprg.use();
  bprg.update_model(mat4(1));
  bprg.update_lighting_uniforms(lights, lm, mtl);
  m_sphere.draw();

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "fps : " << m_fps << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void blobby3d_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
  m_text.reshape(m_info.wnd_width, m_info.wnd_height);
}

void blobby3d_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

}

int main(int argc, char *argv[]) {
  zxd::blobby3d_app app;
  app.run();
}
