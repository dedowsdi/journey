#include <app.h>

#include <sstream>
#include <algorithm>

#include <common_program.h>

namespace zxd {

const GLint width = 800;
const GLint height = 800;

lightless_program prg;

std::shared_ptr<kci> method;
std::shared_ptr<kci> num_points;

std::array<std::string, 2> methods = {"method0", "method1"} ;

struct vertex
{
  vec3 pos;
};

auto vsize = sizeof(vertex);

vec4 point_color = vec4(0, 0, 1, 1);
vec4 triangle_color = vec4(0.5, 0.5, 0.5, 1);

struct triangle_model
{
  GLuint vao;
  GLuint vbo;
  std::array<vertex, 3> vertices;

  void build()
  {
    vertices[0] = { vec3(-1, -1, 0) };
    vertices[1] = { vec3(1, -1, 0) };
    vertices[2] = { vec3(0, 1, 0) };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vsize * vertices.size(),
        value_ptr(vertices.front().pos), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vsize, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  void draw()
  {
    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 3);
  }
};

struct point_model
{
  GLuint vao = -1;
  GLuint vbo = -1;
  std::vector<vertex> points;

  void build(std::vector<vertex>&& points_)
  {
    points = std::move(points_);

    if( vao == -1)
    {
      glGenVertexArrays(1, &vao);
      glGenBuffers(1, &vbo);
      glBindVertexArray(vao);
      glBindBuffer(GL_ARRAY_BUFFER, vbo);
      glBufferData(GL_ARRAY_BUFFER, vsize * points.size(), 0, GL_STATIC_DRAW);
      glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vsize, BUFFER_OFFSET(0));
      glEnableVertexAttribArray(0);
    }

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vsize * points.size(),
      value_ptr(points.front().pos), GL_STATIC_DRAW);
  }

  void draw()
  {
    glBindVertexArray(vao);
    glDrawArrays(GL_POINTS, 0, points.size());
  }
};

triangle_model triangle;
point_model points;

class triangle_random_point : public app {
private:

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) override;
};

void triangle_random_point::init_info() {
  app::init_info();
  m_info.title = "triangle_random_point";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

vec3 rand_triangle_point0(const vec3& p0, const vec3& p1, const vec3& p2)
{
  // get random point t∈[0,1], there exists a unique S△ADE : S△ABC = t : 1 ,
  // |AD| : |AB| = sqrt(t) : 1, S△BCQ : S△ABC = (1 - sqrt(q)) : 1 . Note that
  // there are t*100 perecent chance to generate vertices inside △ADE, which is
  // also propotion to it's area, which means the distribution is uniform.
  //
  //                     X  A
  //                    /|\
  //                   / | \
  //                  /  |  \
  //                 /   |   \
  //                /    |    \
  //               /     |     \
  //             D-------+-------E
  //             /       |Q      \
  //            /        |        \
  //           /         |         \
  //          /          |          \
  //         /           |           \
  //        /            |            \
  //      B -------------+-------------\ C

  auto t = linearRand(0.0f, 1.0f);
  t = sqrt(t);
  auto s = linearRand(0.0f, 1.0f);
  auto a = 1 - t; // b + c = t
  auto b = (1 - s) * t; // b : c = (1 - s) : s
  auto c = s * t;
  return p0 * a + p1 * b + p2 * c;
}

vec3 rand_triangle_point1(const vec3& p0, const vec3& p1, const vec3& p2)
{

// generate random s,t∈[0,1], use them as barycentric coordinate for B and C, 
// let |AF| : |AB| = s : 1 , |FG| : |AC| = t : 1, if s + t > 1, reflect G by
// center of BC, then bcc for B is 1-s, bcc for C is 1-t.
//
//                     C \-----/-------------- D
//                      / \   /             /
//                     /   \ /      G/     /
//                    /     X       /     /
//                   /     / \     /     /
//                  /     /   \   /     /
//                 /     /     \ / t   /
//                /     /       X     /
//               /   G'/       / \   /
//              /             /   \ /
//             ---------- ---/-----X
//            A    s         F      B

  auto s = linearRand(0.0f, 1.0f);
  auto t = linearRand(0.0f, 1.0f);
  if (s + t > 1)
  {
    s = 1 - s;
    t = 1 - t;
  }

  return p0 * s + p1 * t + p2 * (1 - s - t);
}

void reset_points(const kci* p)
{
  auto n = num_points->get_int();
  auto m = method->get_int_enum();
  auto func = m == 0 ? rand_triangle_point0 : rand_triangle_point1;

  std::vector<vertex> vertices;
  vertices.reserve(n);
  const auto& p0 = triangle.vertices[0].pos;
  const auto& p1 = triangle.vertices[1].pos;
  const auto& p2 = triangle.vertices[2].pos;

  std::generate_n(std::back_inserter(vertices), n,
    [func, p0, p1, p2]() -> vertex { return {func(p0, p1, p2)}; });

  points.build(std::move(vertices));
}

void triangle_random_point::create_scene() {
  prg.init();

  triangle.build();

  method = m_control.add_enum('Q', {0, 1}, 0, reset_points);
  num_points = m_control.add('W', 20000, 1000, 1000000, 1000, reset_points);
  reset_points(0);
}

void triangle_random_point::update() {}

void triangle_random_point::display() {
  glClear(GL_COLOR_BUFFER_BIT);

  prg.use();
  auto mvp_mat = mat4(1);
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp_mat));

  glUniform4fv(prg.ul_color, 1, value_ptr(triangle_color));
  triangle.draw();

  glUniform4fv(prg.ul_color, 1, value_ptr(point_color));
  points.draw();

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "q : method : " << methods[method->get_int_enum()] << std::endl;
  ss << "w : num_points : " << num_points->get_int() << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void triangle_random_point::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
}

void triangle_random_point::glfw_key(
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
  zxd::triangle_random_point app;
  app.run();
}
