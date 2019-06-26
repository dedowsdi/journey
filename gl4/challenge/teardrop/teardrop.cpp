#include <sstream>
#include <iterator>
#include <ostream>
#include <iomanip>
#include <algorithm>

#include "app.h"
#include "bitmap_text.h"
#include "teardrop.h"
#include "common_program.h"
#include "stream_util.h"
#include "geometry_test.h"
#include "pingpong.h"

#define WIDTH 720
#define HEIGHT 720

namespace zxd
{

constexpr GLuint num_particles = 500;
GLuint triangle_count;
vec3_vector geometry_vertices;
vec3_vector geometry_normals;
bool draw_cpu = false;

glm::mat4 v_mat;
glm::mat4 p_mat;

class particle
{
public:
  particle(const vec3& pos, const vec3& vel = vec3(0), const vec3& acc = vec3(0)):
    m_pos(pos),
    m_vel(vel)
    //m_acc(acc)
  {
  }

  const vec3& vel() const { return m_vel; }
  void vel(const vec3& v){ m_vel = v; }

  const vec3& pos() const { return m_pos; }
  void pos(const vec3& v){ m_pos = v; }

  //const vec3& acc() const { return m_acc; }
  //void acc(const vec3& v){ m_acc = v; }

  void update();

  // check if segment from origin to origin + direction intersects ccw triangle
  bool intersect(const vec3& origin, const vec3& direction,
      const vec3& v0, const vec3 v1, const vec3& v2,vec3& point);

private:
  vec3 m_pos;
  vec3 m_vel;
  //vec3 m_acc;
};

using particles = std::vector<particle>;

class geometry_program : public program
{
public:

  void attach_shaders() override
  {
    attach(GL_VERTEX_SHADER, "shader4/teardrop_geometry.vs.glsl");
    //attach(GL_GEOMETRY_SHADER, "shader4/teardrop_geometry.gs.glsl");
  }

} gprg;

class particle_program : public program
{
public:

  GLint ul_geometry_tbo;
  GLint ul_triangle_count;
  GLint ul_color;
  GLint ul_vp_mat;

  void attach_shaders() override
  {
    attach(GL_VERTEX_SHADER, "shader4/teardrop_particle.vs.glsl");
    attach(GL_FRAGMENT_SHADER, {"#version 430 core\n"}, "shader4/lightless.fs.glsl");
  }

  void bind_uniform_locations() override
  {
    uniform_location(&ul_vp_mat, "vp_mat");
    uniform_location(&ul_geometry_tbo, "geometry_tbo");
    uniform_location(&ul_triangle_count, "triangle_count");
    uniform_location(&ul_color, "color");
  }

} pprg;

blinn_program lprg;
lightless_program llprg;
light_vector lights;
light_model lm;
material front_mtl;
material back_mtl;
GLuint geometry_fbo[1];
GLuint geometry_tex;
pingpong particle_buffer;

// for cpu draw purpose only
struct particle_vao
{
  GLuint vao;
  GLuint vbo;
  pingpong vaos;

  void init(const particles& particles_)
  {

    if(zxd::draw_cpu)
    {
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(particle) * num_particles, 0, GL_DYNAMIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(particle), BUFFER_OFFSET(0)); // pos
      glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(particle), BUFFER_OFFSET(12));// vel
      glEnableVertexAttribArray(0);
      glEnableVertexAttribArray(1);
    }
    else
    {
      glGenVertexArrays(2, vaos.pointer());
      glGenBuffers(2, particle_buffer.pointer());

      for (int i = 0; i < 2; ++i)
      {
        glBindVertexArray(vaos[i]);

        glBindBuffer(GL_ARRAY_BUFFER, particle_buffer[i]);
        glBufferData(GL_ARRAY_BUFFER, particles_.size() * sizeof(particle),
            &particles_.front(), GL_DYNAMIC_COPY);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(particle), BUFFER_OFFSET(0)); // pos
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(particle), BUFFER_OFFSET(12));// vel

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
      }
    }
  }

  void update(const particles& particles_)
  {
    if(zxd::draw_cpu)
    {
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, sizeof(particle) * num_particles, &particles_.front(), GL_DYNAMIC_DRAW);
    }
  }

  void draw()
  {
    if(zxd::draw_cpu)
      draw_cpu();
    else
      draw_gpu();
  }

  void draw_cpu()
  {
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER,geometry_fbo[0]);
    glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, geometry_vertices.size() * sizeof(vec3),
        &geometry_vertices.front());

    llprg.use();
    glUniformMatrix4fv(llprg.ul_mvp_mat, 1, 0, value_ptr(p_mat * v_mat));
    glUniform4f(llprg.ul_color, 1, 1, 0, 1);

    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, num_particles);
  }

  void draw_gpu()
  {
    pprg.use();

    glBindTexture(GL_TEXTURE_BUFFER, geometry_tex);

    glUniform1i(pprg.ul_geometry_tbo, 0);
    glUniformMatrix4fv(pprg.ul_vp_mat, 1, 0, value_ptr(p_mat * v_mat));
    glUniform1i(pprg.ul_triangle_count, triangle_count);
    glUniform4f(pprg.ul_color, 1, 1, 0, 0.9);

    glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particle_buffer.pong());
    glBeginTransformFeedback(GL_POINTS);

    glPointSize(1.5);
    glBindVertexArray(vaos.ping());
    glDrawArrays(GL_POINTS, 0, num_particles);

    glEndTransformFeedback();

    //vec3_vector data;
    //data.resize(9);
    //glGetBufferSubData(GL_TRANSFORM_FEEDBACK_BUFFER, 0, 108, &data.front());
    //std::copy(data.begin(), data.end(), std::ostream_iterator<vec3>(std::cout, "\n"));
    //std::cout << "-----------" << std::endl;

    particle_buffer.swap();
    vaos.swap();
  }
} particle_drawable;

class teardrop_app : public app
{
private:
  particles m_particles;
  teardrop m_drop;

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
      GLFWwindow *wnd, int key, int scancode, int action, int mods) override;
};

void particle::update()
{
  //m_vel += m_acc;
  //m_acc = vec3(0);
  m_pos += m_vel;

  vec3 point;

  for (int i = 0; i < triangle_count; ++i )
  {
    const vec3& v0 = geometry_vertices[i*3 + 0];
    const vec3& v1 = geometry_vertices[i*3 + 1];
    const vec3& v2 = geometry_vertices[i*3 + 2];
    if(v0 == v1 || v0 == v2 || v1 == v2)
    {
      //std::cout << i << ":" << v0 << std::endl;
      //std::cout << i << ":" << v1 << std::endl;
      //std::cout << i << ":" << v2 << std::endl;
      continue;
    }


    if (intersect(m_pos, -m_vel, v0, v1, v2, point))
    {
      if(isnan(point.x))
        continue;
      vec3 n = normalize(cross(v1 - v0, v2 - v0));
      //m_pos = point + reflect(m_pos - point, -n); // might cause another collision
      m_pos = point + 0.01f * reflect(m_pos - point, -n);
      m_vel = glm::linearRand(0.9f, 1.111f) * reflect(m_vel, -n);
      break;
    }
  }
  //m_pos += m_vel;
}

bool particle::intersect(const vec3& origin, const vec3& direction,
    const vec3& v0, const vec3 v1, const vec3& v2,vec3& point)
{
  vec3 v01 = v1 - v0;
  vec3 v02 = v2 - v0;
  vec3 pn = cross(v01, v02);
  GLfloat dot_pn_d = dot(pn, direction);
  //if (abs(dot_pn_d) < 0.0000001f) // parallel check
    //return false;

  //GLfloat pd = -dot(v0, pn);
  //GLfloat t = (-pd - dot(origin, pn)) / dot_pn_d;
  GLfloat t = dot(v0 - origin, pn) / dot_pn_d;

  if(t < 0.0f || t > 1.0f )
    return false;

  point = origin + direction * t;

  return geometry_test::is_point_in_triangle(point, v0, v1, v2);
}

void teardrop_app::init_info()
{
  app::init_info();
  m_info.title = "teardrop_app";
  m_info.wnd_width = WIDTH;
  m_info.wnd_height = HEIGHT;
  m_info.wm.x = 100;
  m_info.wm.y = 100;
  m_info.samples = 8;
}

void teardrop_app::create_scene()
{
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  //glEnable(GL_DEPTH_TEST);


  m_drop.xy_scale(0.6);
  m_drop.slice(32);
  m_drop.stack(32);
  m_drop.include_normal(true);
  m_drop.build_mesh();

  auto vertices = m_drop.attrib_vec3_array(0);

  light_source light;
  light.position = vec4(1, -1, 1, 0);
  light.diffuse = vec4(0.8);
  light.specular = vec4(1);
  light.ambient = vec4(0.2);
  lights.push_back(light);

  front_mtl.diffuse = vec4(0.5, 0, 0.5, 0.5);
  front_mtl.specular = vec4(0.5);
  front_mtl.shininess = 80;

  back_mtl.diffuse = vec4(0.0, 0, 0.9, 0.5);
  back_mtl.specular = vec4(0.5);
  back_mtl.shininess = 80;

  lm.ambient = vec4(0.2);
  lm.local_viewer = true;

  llprg.init();

  lprg.init();
  lprg.bind_lighting_uniform_locations(lights, lm, back_mtl);
  front_mtl.bind_uniform_locations(lprg);
  p_mat = perspective<GLfloat>(fpi4, wnd_aspect(), 0.1, 1000);
  v_mat = zxd::isometric_projection(1.5);
  set_v_mat(&v_mat);

  triangle_count = m_drop.slice() * m_drop.stack() * 2;
  glGenBuffers(1, geometry_fbo);
  glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, geometry_fbo[0]);
  glBufferData(GL_TRANSFORM_FEEDBACK_BUFFER, sizeof(vec3) * triangle_count * 3,
      0, GL_DYNAMIC_COPY);

  gprg.init();
  {
    const char* varyings[] = {"w_vertex"};
    glTransformFeedbackVaryings(gprg.object, 1, varyings, GL_INTERLEAVED_ATTRIBS);
    gprg.link();
  }

  pprg.init();
  {
    const char* varyings[] = {"pos_o", "vel_o"};
    glTransformFeedbackVaryings(pprg.object, 2, varyings, GL_INTERLEAVED_ATTRIBS);
    pprg.link();
  }

  glGenTextures(1, &geometry_tex);
  glBindBuffer(GL_TEXTURE_BUFFER, geometry_fbo[0]);
  glBindTexture(GL_TEXTURE_BUFFER, geometry_tex);
  glTexBuffer(GL_TEXTURE_BUFFER, GL_RGB32F, geometry_fbo[0]);

  m_particles.reserve(num_particles);
  std::generate_n(std::back_inserter(m_particles), num_particles, 
      []()->particle
      {
        auto pos = glm::linearRand(vec3(-0.25), vec3(0.25));
        auto vel = glm::linearRand(vec3(-0.02), vec3(0.02));
        return particle(pos, vel);
      });

  particle_drawable.init(m_particles);
  geometry_vertices.resize(triangle_count * 3);
  geometry_normals.resize(geometry_vertices.size());
}

void teardrop_app::update() 
{
  if(m_frame_number < 2)
    return;

  for (auto& p : m_particles)
  {
    p.update();
  }
  particle_drawable.update(m_particles);
}

void teardrop_app::display()
{
  glClear(GL_COLOR_BUFFER_BIT);

  gprg.use();
  glUniformMatrix4fv(0, 1, 0, value_ptr(mat4(1)));
  //glUniformMatrix4fv(1, 1, 0, value_ptr(mat4(1)));

  // get geometry buffer
  glDisable(GL_CULL_FACE);
  glEnable(GL_RASTERIZER_DISCARD);
  glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, geometry_fbo[0]);
  glBeginTransformFeedback(GL_TRIANGLES);
  m_drop.draw();
  glEndTransformFeedback();
  glDisable(GL_RASTERIZER_DISCARD);
  glEnable(GL_CULL_FACE);

  // draw back face of tear drop
  lprg.use();

  lprg.update_uniforms(mat4(1), v_mat, p_mat);
  lprg.update_lighting_uniforms(lights, lm, back_mtl, v_mat);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  glCullFace(GL_FRONT);
  glUniform1f(lprg.ul_normal_scale, -1.0f);
  m_drop.draw();

  // draw particles
  particle_drawable.draw();

  // draw front face of tear drop
  lprg.use();
  glCullFace(GL_BACK);
  glUniform1f(lprg.ul_normal_scale, 1.0f);
  front_mtl.update_uniforms();
  m_drop.draw();

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "fps : " << m_fps << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void teardrop_app::glfw_resize(GLFWwindow *wnd, int w, int h)
{
  app::glfw_resize(wnd, w, h);
}

void teardrop_app::glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods)
{
  if (action == GLFW_PRESS)
  {
    switch (key)
    {
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

int main(int argc, char *argv[])
{
  zxd::teardrop_app app;
  app.run();
}
