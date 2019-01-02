// http://www.oftenpaper.net/sierpinski.htm
// 
// used style:
//  
//  edge 4
//  draw_method : times
//  ratio : 0.52
//
//  edge 3
//  draw_method : divide
//  power : 3 5
//
//  edge 32
//  pick : radius_2
//  draw_method : times
//  power : 1 32
//
//  edge 10
//  draw_method : divide
//  pick : radius_2
//  power : 3 1.6
//
#include "app.h"

#include <sstream>
#include <algorithm>

#include "bitmap_text.h"
#include "circle.h"
#include "common_program.h"
#include "debugger.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

GLint edges = 3;
GLint draw_per_frame = 20000;
GLint pick = 0;
GLint draw_method = 0;
GLint pick_radius = 2;
GLfloat radius = 1;
GLfloat alpha = 0.1;
GLfloat ratio = 0.5;
float point_power = 1;
float length_power = 1;
bool stop = false;
bool draw_polygon = false;
bool display_help = true;

kcip kci_edges;
kcip kci_ratio;
kcip kci_draw_per_frame;
kcip kci_alpha;
kcip kci_pick;
kcip kci_draw_method;
kcip kci_point_power;
kcip kci_length_power;
kcip kci_radius;

circle polygon;
vec2_vector* vertices;
// after rotate left, current select in front
int_vector selections;

lightless_program prg;

const char* picks[] = {"random", "no repeat", "radius_n"};
const char* draw_methods[] = {"normal", "times", "divide"};

vec4_vector colors;

vec2 point;
vec2 draw_point;

class chaos_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info();
  virtual void create_scene();
  void reset_polygon(const kci* kci = 0);

  virtual void update();

  virtual void display();

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h);

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods);
private:

  vec2 get_draw_point(const vec2& point);
  void pick_next();
};

void chaos_app::init_info() {
  app::init_info();
  m_info.title = "chaos_app";
  m_info.samples = 8;
  m_info.double_buffer = GL_FALSE;
  m_info.wnd_width = 0;
  m_info.wnd_height = 0;
  m_info.fullscreen = true;
}

void chaos_app::create_scene() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  glfwSetInputMode(m_wnd, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

  m_text.init();
  m_text.reshape(wnd_width(), wnd_height());

  prg.init();

  auto callback = std::bind(std::mem_fn(&chaos_app::reset_polygon), this, std::placeholders::_1);
  kci_draw_per_frame = m_control.add_control(GLFW_KEY_Q, draw_per_frame , 1, 10000, 10, callback);
  kci_edges = m_control.add_control(GLFW_KEY_W, edges, 3, 1000, 1, callback);
  kci_ratio = m_control.add_control<GLfloat>(GLFW_KEY_E, ratio, -100, 100, 0.05, callback);
  kci_alpha = m_control.add_control<GLfloat>(GLFW_KEY_R, alpha, 0, 1, 0.02, callback);
  kci_pick = m_control.add_control(GLFW_KEY_U, pick, 0, 2, 1, callback);
  kci_draw_method = m_control.add_control(GLFW_KEY_I, draw_method, 0, 2, 1, callback);
  kci_point_power = m_control.add_control<GLfloat>(GLFW_KEY_O, point_power, -100, 100, 0.1, callback);
  kci_length_power = m_control.add_control<GLfloat>(GLFW_KEY_P, length_power, -100, 100, 0.1, callback);
  kci_radius = m_control.add_control<GLfloat>(GLFW_KEY_A, pick_radius, 1, 1000, 1, callback);

  reset_polygon();
}

void chaos_app::reset_polygon(const kci* kci)
{
  glClear(GL_COLOR_BUFFER_BIT);

  edges = kci_edges->get_int();
  ratio = kci_ratio->get_float();
  draw_per_frame = kci_draw_per_frame->get_int();
  alpha = kci_alpha->get_float();
  pick = kci_pick->get_int();
  draw_method = kci_draw_method->get_int();
  point_power = kci_point_power->get_float();
  length_power = kci_length_power->get_float();
  pick_radius = kci_radius->get_float();

  polygon.slice(edges);
  polygon.radius(radius);
  polygon.type(circle::LINE);
  polygon.build_mesh();
  vertices = &polygon.attrib_vec2_array(0)->get_vector();

  selections.resize(edges);
  std::iota(selections.begin(), selections.end(), 0);
  colors.resize(edges);
  for (auto& color : colors) {
    color = vec4(zxd::hsb2rgb(vec3(glm::linearRand(0.0f, 1.0f), 1, 1)), 1);
  }

  GLfloat scale = 1.08f;

  if(draw_method == 0)
    prg.p_mat = zxd::rect_ortho(scale, scale, wnd_aspect());
  else
  {
    vec2 bak_point = point;
    vec2 max_point(0);

    for (int i = 0; i < 1000; ++i) {
      update();
      max_point = max(max_point, glm::abs(get_draw_point(point)));
    }
    GLfloat l = max(max_point.x, max_point.y) * scale;
    prg.p_mat = zxd::rect_ortho(l, l, wnd_aspect());
    point = bak_point;
    std::iota(selections.begin(), selections.end(), 0);
  }

  mat4 mvp_mat = prg.p_mat * mat4(1);
  prg.use();
  glUniform4f(prg.ul_color, 1, 1, 1, 1);
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));

  // create first random point in polygon
  point = vertices->at(0);

  if(draw_polygon)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    polygon.draw();
    glDisable(GL_BLEND);
  }
}

void chaos_app::pick_next()
{
  int idx = 0;
  switch(pick)
  {
    case 0:
      idx = glm::linearRand(0, edges - 1);
      break;

    case 1:
      do {
        idx = glm::linearRand(0, edges-1);
      } while (idx == selections.front());
      break;

    case 2:
      idx = edges + selections.front() + glm::linearRand(-pick_radius, pick_radius);
      idx %= edges;
      break;

    default:
      break;
  }
  std::rotate(selections.begin(), selections.begin() + 1, selections.end());
  selections.front() = idx;
}

vec2 chaos_app::get_draw_point(const vec2& point)
{
  switch(draw_method)
  {
    case 0:
      return point;
      break;

    case 1:
      return glm::pow(glm::abs(point), vec2(point_power)) * glm::sign(point)
      * glm::pow(glm::length(point), length_power);
      break;

    case 2:
      return glm::pow(glm::abs(point), vec2(point_power)) * glm::sign(point)
      / glm::pow(glm::length(point), length_power);
      break;
  }
  throw std::runtime_error("illegal draw method");
}


void chaos_app::update() {
  pick_next();
  point = glm::mix(point, vertices->at(selections.front()), ratio);
  draw_point = get_draw_point(point);
}

void chaos_app::display() {
  //glClear(GL_COLOR_BUFFER_BIT);

  mat4 mvp_mat = prg.p_mat * mat4(1);
  prg.use();
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));

  glEnable(GL_BLEND);
  glBlendColor(0, 0, 0, alpha);
  glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);

  if(!stop)
  {
    for (int i = 0; i < draw_per_frame; ++i) {
      //debugger::draw_point(point, mvp_mat);
      //glUniform4fv(prg.ul_color, 1, glm::value_ptr(colors[selections.front()]));
      debugger::draw_point(draw_point, mvp_mat, 1, colors[selections.front()]);
      //debugger::draw_point(point * point, mvp_mat);
      update();
    }
  }

  glEnable(GL_SCISSOR_TEST);
  glScissor(0, wnd_height()-300, 300, 300);
  glClear(GL_COLOR_BUFFER_BIT);

  if(!display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "aA : pick_radius : " << pick_radius << std::endl;
  ss << "uU : pick : " << picks[pick] << std::endl;
  ss << "iI : draw_method : " << draw_methods[draw_method] << std::endl;
  ss << "oOpP : power : " << point_power << " " << length_power << std::endl;
  ss << "qQ : draw count : " << draw_per_frame << std::endl;
  ss << "wW : edge : " << edges << std::endl;
  ss << "eE : ratio : " << ratio << std::endl;
  ss << "rR : alpha : " << alpha << std::endl;
  ss << "s : stop" << std::endl;
  ss << "c : random color" << std::endl;
  ss << "h : toggle help " << std::endl;
  ss << "fps : " << m_fps << std::endl;
  m_text.print(ss.str(), 10, wnd_height()- 20);
  glDisable(GL_BLEND);
  glDisable(GL_SCISSOR_TEST);
}

void chaos_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
  m_text.reshape(wnd_width(), wnd_height());
}

void chaos_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;

      case GLFW_KEY_S:
        stop ^= 1;
        break;

      case GLFW_KEY_H:
        display_help ^= 1;
        break;

      case GLFW_KEY_C:
        reset_polygon();
        break;

      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

}

int main(int argc, char *argv[]) {
  zxd::chaos_app app;
  app.run();
}
