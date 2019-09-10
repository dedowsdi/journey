#include "app.h"

#include <sstream>
#include <algorithm>
#include <limits>
#include <chrono>

#include "common_program.h"
#include "sphere.h"

namespace zxd {

const GLint width = 800;
const GLint height = 800;
std::shared_ptr<kci> num_vertices;

mat4 p_mat;
mat4 v_mat;
mat4 sphere_mat;

lightless_program prg;
sphere bounding_sphere;
const vec4 point_color = vec4(1, 1, 1, 1);
const vec4 sphere_color = vec4(0.5, 0, 0, 1);

struct vertex
{
  vec3 pos;
};

auto vsize = sizeof(vertex);

struct points_model
{
  GLuint vao = -1;
  GLuint vbo = -1;
  std::vector<vertex> vertices;

  void reset()
  {
    vertices.resize(num_vertices->get_int());
    if (vao == -1)
    {
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);

      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, vertices.size() * vsize, 0, GL_STATIC_DRAW);

      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vsize, BUFFER_OFFSET(0));
      glEnableVertexAttribArray(0);

    }
    std::generate_n(vertices.begin(), num_vertices->get_int(),
      []() -> vertex { return {linearRand(vec3(0), vec3(100))}; });

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * vsize,
      value_ptr(vertices.front().pos), GL_STATIC_DRAW);
  }

  void draw()
  {
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, vertices.size());
  }
};

points_model points;

void reset()
{
  points.reset();

  auto t0 = std::chrono::steady_clock::now();

  // 1st pass, find max span on x, y, z between two vertices, it's the minimal
  // possible radius of bounding sphere
  vec3 xmin(FLT_MAX), xmax(-FLT_MAX), ymin(FLT_MAX), ymax(-FLT_MAX),
    zmin(FLT_MAX), zmax(-FLT_MAX);
  for (const auto& vertex : points.vertices)
  {
    const auto& pos = vertex.pos;
    if (pos.x < xmin.x)
      xmin = pos;
    if (pos.y < ymin.y)
      ymin = pos;
    if (pos.z < zmin.z)
      zmin = pos;
    if (pos.x > xmax.x)
      xmax = pos;
    if (pos.y > ymax.y)
      ymax = pos;
    if (pos.z > zmax.z)
      zmax = pos;
  }

  auto xspan = length2(xmax - xmin);
  auto yspan = length2(ymax - ymin);
  auto zspan = length2(zmax - zmin);

  GLfloat radius;
  vec3 origin;

  if (xspan >= yspan && xspan >= zspan)
  {
    radius = 0.5f * sqrt(xspan);
    origin = 0.5f * (xmin + xmax);
  }
  else if (yspan >= yspan && yspan >= zspan)
  {
    radius = 0.5f * sqrt(yspan);
    origin = 0.5f * (ymin + ymax);
  }
  else
  {
    radius = 0.5f * sqrt(zspan);
    origin = 0.5f * (zmin + zmax);
  }

  std::cout << "after 1st pass, set origin to " << origin << std::endl;
  std::cout << "after 1st pass, set radius to " << radius << std::endl;

  // second pass, find vertices that's outside of the bounding sphere, inflate
  // sphere upon found one, note that during inflating , the intersecton point
  // of vertex-->origin-->sphere keeps still.
  auto radius2 = radius * radius;
  for (const auto& vertex : points.vertices)
  {
    auto dist2 = length2(vertex.pos - origin);
    if (dist2 < radius2)
      continue;

    // use static point, origin, current point as new diameter
    auto dist = sqrt(dist2);
    auto origin_move_dist = (dist - radius) * 0.5f;
    radius = (radius + dist) * 0.5f;
    origin = mix(origin, vertex.pos, origin_move_dist / dist);
    radius2 = radius * radius;
  }
  
  std::cout << "after 2nd pass, set origin to " << origin << std::endl;
  std::cout << "after 2nd pass, set radius to " << radius << std::endl;

  sphere_mat = scale(translate(origin), vec3(radius));

  auto t1 = std::chrono::steady_clock::now();
  auto d = t1 - t0;
  std::cout << "it took "
            << std::chrono::duration<GLfloat, std::ratio<1, 1000>>(d).count()
            << "microseconds to compute bounding sphere for "
            << points.vertices.size() << " vertices" << std::endl;
}


class bounding_sphere_app : public app {
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

void bounding_sphere_app::init_info() {
  app::init_info();
  m_info.title = "bounding_sphere_app";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

void bounding_sphere_app::create_scene() {

  num_vertices = m_control.add<GLint>(
    'W', 1000, 1000, 1000000, 1000, [](auto) -> void { reset(); });

  prg.init();
  bounding_sphere.build_mesh( { attrib_semantic::vertex } );

  reset();

  p_mat = perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
  lookat(vec3(0, -300, 0), vec3(50), vec3(0, 0, 1), &v_mat);
}

void bounding_sphere_app::update() {}

void bounding_sphere_app::display() {
  glClear(GL_COLOR_BUFFER_BIT);

  prg.use();
  auto vp_mat = p_mat * v_mat;

  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(vp_mat));
  glUniform4fv(prg.ul_color, 1, value_ptr(point_color));
  points.draw();

  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(vp_mat * sphere_mat));
  glUniform4fv(prg.ul_color, 1, value_ptr(sphere_color));
  bounding_sphere.draw();
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "q : reset" << std::endl;
  ss << "w : num vertices : " << num_vertices->get_int() << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void bounding_sphere_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
}

void bounding_sphere_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_Q:
        reset();
        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void bounding_sphere_app::glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                               int mods)
{
  app::glfw_mouse_button(wnd, button, action, mods);
}

void bounding_sphere_app::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
}

}

int main(int argc, char *argv[]) {
  zxd::bounding_sphere_app app;
  app.run();
}
