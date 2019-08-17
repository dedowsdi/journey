#include <sstream>
#include <algorithm>

#include "app.h"
#include "bitmap_text.h"
#include "turtle.h"
#include "common_program.h"
#include "lsystem.h"
#include "debugger.h"
#include "geometry_util.h"
#include "line.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

glm::mat4 v_mat;
glm::mat4 p_mat;

kcip kci_pattern;
kcip kci_n;
kcip kci_degree;
kcip kci_scale_per_iteartion;
kcip kci_debug_count;
kcip kci_lsystem_step;
bool display_help = true;

lightless_program llprg;

struct pattern
{
  std::string name; // name should be 1st in brace initialization
  GLint n;
  GLfloat degree;
  GLfloat scale;
  std::string axiom;
  lsystem system;
};

std::vector<pattern> patterns;

class turtle_app : public app {
private:
  turtle m_turtle;
  vec3_vector m_vertices;
  line m_geometry;
  GLint m_step_vertices = 0;
  GLint m_last_step_vertices = 0;
  vec3 m_bound;
  vec3 m_step;
  mat4 m_step_transform; // current transform after step
  std::string m_geometry_text;
  geometry_base m_turtle_geometry;


public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) override;

private:
  void reset_pattern();
  void step();
  void update_bound(const vec3_vector& vertices);
};

void turtle_app::init_info() {
  app::init_info();
  m_info.title = "turtle_app";
  m_info.wnd_width = WIDTH;
  m_info.wnd_height = HEIGHT;
  m_info.samples = 16;
}

void turtle_app::create_scene() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);


  llprg.init();
  p_mat = zxd::rect_ortho(100, 100, wnd_aspect());
  set_camera_mode(CM_ORTHO);
  set_p_mat(&p_mat);
  
  {
    auto vertices = std::make_shared<vec3_array>();
    vertices->push_back(vec3(0, 0.6, 0));
    vertices->push_back(vec3(-0.3, -0.4, 0));
    vertices->push_back(vec3(0.3, -0.4, 0));
    m_turtle_geometry.attrib_array(0, vertices);
    m_turtle_geometry.add_primitive_set(new draw_arrays(GL_TRIANGLES, 0, vertices->size()));
    m_turtle_geometry.bind_and_update_buffer();
  }

  {
    lsystem l;
    l.add_rule('F', " F - F + F + FF - F - F + F");
    patterns.push_back({"1.6", 2, 90, 4, "F-F-F-F", l});
  }
  {
    lsystem l;
    l.add_rule('F', "F+FF-FF-F-F+F+FF-F-F+F+FF+FF-F");
    patterns.push_back({"1.7.a", 2, 90, 4, "F-F-F-F", l});
  }
  {
    lsystem l;
    l.add_rule('F', " F + F - F - F + F");
    patterns.push_back({"1.7.b", 4, 90, 4, "-F", l});
  }
  {
    lsystem l;
    l.add_rule('F', "F + f - FF + F + FF + Ff + FF - f + FF -F - FF - Ff - FFF");
    l.add_rule('f', "ffffff");
    patterns.push_back({"1.8", 2, 90, 4, "F+F+F+F", l});
  }
  { // a
    lsystem l;
    l.add_rule('F', "FF-F-F-F-F-F+F");
    patterns.push_back({"1.9.a", 4, 90, 1, "F-F-F-F", l});
  }
  { // b
    lsystem l;
    l.add_rule('F', "FF-F-F-F-FF");
    patterns.push_back({"1.9.b", 4, 90, 4, "F-F-F-F", l});
  }
  { // c
    lsystem l;
    l.add_rule('F', "FF-F+F-F-FF");
    patterns.push_back({"1.9.c", 3, 90, 4,"F-F-F-F", l});
  }
  { // d
    lsystem l;
    l.add_rule('F', "FF-F--F-F");
    patterns.push_back({"1.9.d", 4, 90, 4, "F-F-F-F", l});
  }
  { // e
    lsystem l;
    l.add_rule('F', "F-FF--F-F");
    patterns.push_back({"1.9.e", 5, 90, 4, "F-F-F-F", l});
  }
  { // f
    lsystem l;
    l.add_rule('F', "F-F+F-F-F");
    patterns.push_back({"1.9.f", 4, 90, 4, "F-F-F-F", l});
  }
  { // 1.10.a
    lsystem l;
    l.add_rule('l', "l+r+");
    l.add_rule('r', "-l-r");
    patterns.push_back({"1.10.a", 10, 90, 2, "l", l});
  }
  { // 1.10.b
    lsystem l;
    l.add_rule('l', "r+l+r");
    l.add_rule('r', "l-r-l");
    patterns.push_back({"1.10.a", 6, 60, 2, "l", l});
  }
  { // 1.11.a
    lsystem l;
    l.add_rule('l', "l+r++r-l--ll-r+");
    l.add_rule('r', "-l+rr++r+l--l-r");
    patterns.push_back({"1.11.a", 4, 60, 2, "l", l});
  }
  { // 1.16.a
    lsystem l;
    l.add_rule('L', "LF+RFR+FL-F-LFLFL-FRFR+");
    l.add_rule('R', "-LFLF+RFRFR+F+RF-LFL-FR");
    patterns.push_back({"node 1.16.a", 3, 90, 4, "-L", l});
  }
  {
    lsystem l;
    l.add_rule('F', "F[+F]F[-F]F");
    patterns.push_back({"1.24.a", 5, 25.7, 3, "F", l});
  }
  {
    lsystem l;
    l.add_rule('F', "F[+F]F[-F][F]");
    patterns.push_back({"1.24.b", 5, 20, 3, "F", l});
  }
  {
    lsystem l;
    l.add_rule('F', "FF-[-F+F+F]+[+F-F-F]");
    patterns.push_back({"1.24.c", 4, 22.5, 3, "F", l});
  }
  {
    lsystem l;
    l.add_rule('X', "F[+X]F[-X]+X"); // node
    l.add_rule('F', "FF");
    patterns.push_back({"1.24.d", 7, 20, 3, "X", l});
  }
  {
    lsystem l;
    l.add_rule('X', "F[+X][-X]FX"); // node
    l.add_rule('F', "FF");
    patterns.push_back({"1.24.e", 7, 25.7, 3, "X", l});
  }
  {
    lsystem l;
    l.add_rule('X', "F-[[X]+X]+F[+FX]-X"); // node
    l.add_rule('F', "FF");
    patterns.push_back({"1.24.f", 5, 22.5, 3, "X", l});
  }
  {
    lsystem l;
    l.add_rule('X', "F[+X+X][-X-X]FX"); // node
    l.add_rule('F', "FF");
    patterns.push_back({"variation0 of 1.24e", 7, 25.7, 3, "X", l});
  }
  {
    lsystem l;
    l.add_rule('X', "F[+X][-X]F[-X]F+X"); // node
    l.add_rule('F', "FF");
    patterns.push_back({"variation0 of 1.24d", 7, 20, 3, "X", l});
  }
  {
    lsystem l;
    l.add_rule('X', "F-[[X]++X]+F[+FX]--X"); // node
    l.add_rule('F', "FF");
    patterns.push_back({"variation0 of 1.24f", 5, 22.5, 3, "X", l});
  }
  {
    lsystem l;
    l.add_rule('F', "F[++F-F]F[--F+F]+F--F+");
    patterns.push_back({"variation0 of 1.24a", 5, 25.7, 3, "F", l});
  }

  auto callback = [this](const kci* )->void
  {
    this->reset_pattern();
  };

  kci_pattern = m_control.add<GLint>(GLFW_KEY_Q, 0, 0, patterns.size() - 1, 1, 
      [callback](const kci* k)
      {
        const auto& pattern = patterns[kci_pattern->get_int()];
        kci_n->set<GLint>(pattern.n);
        kci_degree->set<GLfloat>(pattern.degree);
        kci_scale_per_iteartion->set<GLfloat>(pattern.scale);
        callback(k);
      });
  kci_n = m_control.add<GLint>(GLFW_KEY_W, 2, 0, 99, 1, callback);
  kci_degree = m_control.add<GLfloat>(GLFW_KEY_E, 90, 0, 360, 1, callback);
  kci_scale_per_iteartion = m_control.add<GLfloat>(GLFW_KEY_R, 4, 1, 99, 1, callback);
  kci_debug_count = m_control.add<GLint>(GLFW_KEY_U, 2, 0, 10000, 2);
  kci_lsystem_step = m_control.add<GLint>(GLFW_KEY_S, 1, 0, 10000, 1, 
      [this](const kci* k)
      {
        this->step();
      });

  reset_pattern();
}


void turtle_app::update() {}

void turtle_app::display() {
  glClear(GL_COLOR_BUFFER_BIT);

  if(!display_help)
  {
    llprg.use();
    glUniformMatrix4fv(llprg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat * v_mat));
    glUniform4f(llprg.ul_color, 1, 1, 1, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_geometry.draw();
    return;
  }

  // draw unstepped
  llprg.use();
  glUniformMatrix4fv(llprg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat * v_mat));
  glUniform4f(llprg.ul_color, 1, 1, 1, 1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  debugger::draw_line(GL_LINES, m_vertices.begin() + m_step_vertices, m_vertices.end(),
      p_mat * v_mat, 1, vec4(1, 1, 1, 1));

  // step
  debugger::draw_line(GL_LINES, m_vertices.begin(), m_vertices.begin() + m_step_vertices,
      p_mat * v_mat, 1, vec4(1, 1, 0, 0.5));

  // debug
  debugger::draw_line(GL_LINES, m_vertices.begin(), m_vertices.begin() + kci_debug_count->get_int(),
      p_mat * v_mat, 1, vec4(1, 0, 0, 0.5));

  if(m_last_step_vertices > 0)
  {
    // last step
    debugger::draw_line(GL_LINES, m_vertices.begin() + m_step_vertices - m_last_step_vertices,
        m_vertices.begin() + m_step_vertices, p_mat * v_mat, 1, vec4(0, 1, 0, 0.5));
  }

  // next step line (for edge rewritting)
  auto next = min<GLint>(m_vertices.size(), m_step_vertices + 2);
  debugger::draw_line(GL_LINES, m_vertices.begin() + m_step_vertices, m_vertices.begin() + next,
      p_mat * v_mat, 1, vec4(0, 0, 1, 0.5));

  // draw turtle
  mat4 m = glm::scale(m_step_transform, vec3(m_step.y * 0.5f));
  llprg.use();
  glUniformMatrix4fv(llprg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat * v_mat * m));
  glUniform4f(llprg.ul_color, 0, 1, 1, 0.5);
  m_turtle_geometry.draw();

  //glEnable(GL_BLEND);
  //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  const auto& pattern = patterns[kci_pattern->get_int()];
  ss << "q : pattern : " << kci_pattern->get_int() << ":" << pattern.name << std::endl;
  ss << "w : n : " << kci_n->get_int() << std::endl;
  ss << "e : degree : " << kci_degree->get_float() << std::endl;
  ss << "r : scale : " << kci_scale_per_iteartion->get_float() << std::endl;
  ss << "axiom : " << pattern.axiom << std::endl;
  ss << "rules : " << std::endl;

  const auto& rules = pattern.system.get_rules();
  for (const auto& p : rules) {
    ss << "   " << p.first << " : " << p.second << std::endl;
  }
  ss << "u : debug draw debug_count : " << kci_debug_count->get_int() 
  << "/" << m_geometry.num_vertices() << std::endl;
  ss << "s : step : " << kci_lsystem_step->get_int()  << " "
  << m_geometry_text.substr(0, kci_lsystem_step->get_int()) << std::endl;
  ss << "h : help " << std::endl;
  ss << "fps : " << m_fps << std::endl;

  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void turtle_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
  update_bound(m_vertices);
}

void turtle_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;
      case GLFW_KEY_H:
        display_help ^= 1;
        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void turtle_app::reset_pattern()
{
  kci_lsystem_step->set<GLint>(0);
  GLint pattern_index = kci_pattern->get_int();
  GLint n = kci_n->get_int();
  GLfloat degree = kci_degree->get_float() * fpi / 180.0f;
  GLfloat scale = kci_scale_per_iteartion->get_float();

  m_step = vec3(0, 64 / pow(scale, n) , 0);
  auto op_F = std::make_shared<translate_operation>(m_step);
  auto op_f = std::make_shared<translate_only_operation>(m_step);
  auto op_plus = std::make_shared<rotate_operation>(degree, pza); // left
  auto op_minus = std::make_shared<rotate_operation>(-degree, pza); // right
  auto op_l = std::make_shared<translate_operation>(m_step);
  auto op_r = std::make_shared<translate_operation>(m_step);
  auto op_push = std::make_shared<push_operation>();
  auto op_pop = std::make_shared<pop_operation>();

  auto& pattern = patterns[pattern_index];

  m_geometry_text = pattern.system.generate(pattern.axiom, n);

  m_turtle.register_operation('F', op_F);
  m_turtle.register_operation('f', op_f);
  m_turtle.register_operation('+', op_plus);
  m_turtle.register_operation('-', op_minus);
  m_turtle.register_operation('l', op_l);
  m_turtle.register_operation('r', op_r);
  m_turtle.register_operation('[', op_push);
  m_turtle.register_operation(']', op_pop);

  m_vertices = m_turtle.generate(m_geometry_text, mat4(1));
  m_geometry.build_line(GL_LINES, m_vertices.begin(), m_vertices.end());

  std::static_pointer_cast<kci_template<GLint>>(kci_debug_count)
    ->max_value(m_vertices.size());
  std::static_pointer_cast<kci_template<GLint>>(kci_lsystem_step)
    ->max_value(m_geometry_text.size());
  kci_debug_count->set<GLint>(0);
  kci_lsystem_step->set<GLint>(0);
  m_step_vertices = 0;
  m_last_step_vertices = 0;

  update_bound(m_vertices);
}

void turtle_app::step()
{
  if(kci_lsystem_step->get_int() == 0)
  {
    reset_pattern();
    return;
  }

  GLint pattern_index = kci_pattern->get_int();
  auto& pattern = patterns[pattern_index];

  auto left = m_geometry_text.size() - kci_lsystem_step->get_int();
  auto text = pattern.system.step(m_geometry_text, kci_lsystem_step->get_int());

  auto part0 = m_turtle.generate(text.substr(0, text.size() - left), mat4(1));
  m_step_transform = m_turtle.get_transform();
  m_vertices = m_turtle.generate(text, mat4(1));
  m_geometry.build_line(GL_LINES, m_vertices.begin(), m_vertices.end());

  kci_debug_count->set<GLint>(part0.size());
  m_last_step_vertices = part0.size() - m_step_vertices;
  m_step_vertices = part0.size();
  std::static_pointer_cast<kci_template<GLint>>(kci_debug_count)
    ->max_value(m_vertices.size());

  // set bound to next level
  auto n = kci_n->get_int() + 1;
  auto next_text = pattern.system.generate(pattern.axiom, n);
  auto vertices = m_turtle.generate(next_text, mat4(1));
  update_bound(vertices);
}

void turtle_app::update_bound(const vec3_vector& vertices)
{
  auto corners = bounding_box(vertices.begin(), vertices.end());

  auto w = corners.second.x - corners.first.x;
  auto h = corners.second.y - corners.first.y;
  if(w == 0)
    w = h;
  if(h == 0)
    h = w;
  auto center = (corners.first + corners.second) * 0.5f;
  p_mat = zxd::rect_ortho(w*0.51f, h*0.51f, wnd_aspect());
  v_mat = glm::translate(-center);
}

}

int main(int argc, char *argv[]) {
  zxd::turtle_app app;
  app.run();
}
