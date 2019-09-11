/*
 * 3d hilbert curve
 */
#include <sstream>
#include <algorithm>

#include <app.h>
#include <bitmap_text.h>
#include <turtle.h>
#include <common_program.h>
#include <lsystem.h>
#include <debugger.h>
#include <geometry_util.h>
#include <line.h>
#include <capsule3d.h>
#include <timer.h>
#include <common.h>

namespace zxd {

GLuint width = 720;
GLuint height = 720;

vec3 world_center;
glm::mat4 v_mat;
glm::mat4 p_mat;

kcip kci_pattern;
kcip kci_n;
kcip kci_degree;
kcip kci_scale_per_iteartion;
kcip kci_debug_count;
kcip kci_lsystem_step;
bool display_help = true;
bool draw_seed = false;
bool rotating = false;

blinn_program prg;

light_vector lights;
light_model lm;
material mtl;
lightless_program llprg;
capsule3d capsule;

GLuint instance_buffer[3];

enum Buffer{MV_MAT = 0, MVP_MAT = 1, MV_MAT_IT = 2};

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
std::vector<mat4> m_mats;
std::vector<mat4> mvp_mats;
std::vector<mat4> mv_mats;
std::vector<mat4> mv_mat_its;
mat4 rotate_mat;

class plant1_5_app : public app {
private:
  turtle m_turtle;
  vec3_vector m_vertices;
  line m_geometry;
  GLint m_step_vertices = 0;
  GLint m_last_step_vertices = 0;
  vec3 m_bound;
  std::string m_geometry_text;

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
  void update_uniforms();
};

void plant1_5_app::init_info() {
  app::init_info();
  m_info.title = "plant1_5_app";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
  m_info.samples = 16;
  m_info.wm.x = 100;
  m_info.wm.y = 100;
}

void plant1_5_app::create_scene() {
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glEnable(GL_LINE_SMOOTH);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);


  prg.instance = GL_TRUE;
  prg.init();
  p_mat = glm::perspective(fpi4, wnd_aspect(), 0.1f, 1000.0f);
  v_mat = zxd::isometric_projection(100.0f);
  set_v_mat(&v_mat);

  llprg.init();

  { // 1.19
    lsystem l;
    l.add_rule('A', "B-F+CFC+F-D&F^D-F+&&CFC+F+B//");
    l.add_rule('B', "A&F^CFB^F^D^^-F-D^|F^B|FC^F^A//");
    l.add_rule('C', "|D^|F^B-F+C^F^A&&FA&F^C+F+B^F^D//");
    l.add_rule('D', "|CFB-F+B|FA&F^A&&FB-F+B|FC//");
    patterns.push_back({"1.19", 2, 90, 4, "A", l});
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

  light_source l0;
  l0.position = vec4(1, -1, 1, 0);
  l0.diffuse = vec4(1);
  l0.specular = vec4(1);
  lights.push_back(l0);

  lm.local_viewer = true;
  lm.ambient = vec4(0.2f);

  mtl.diffuse = vec4(0.8f);
  mtl.specular = vec4(0.5f);
  mtl.shininess = 50;

  prg.bind_lighting_uniform_locations(lights, lm, mtl);

  capsule.sphere_slice(10);
  capsule.sphere_stack(10);
  capsule.radius(0.15f);
  // scale to cmake clender reach size 1
  capsule.height(1 + capsule.radius() * 2.0f);
  capsule.build_mesh({attrib_semantic::vertex, attrib_semantic::normal});

  glGenBuffers(3, instance_buffer);

  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MV_MAT]);
  matrix_attrib_pointer(3, 1, 0);

  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MVP_MAT]);
  matrix_attrib_pointer(7, 1, 0);

  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MV_MAT_IT]);
  matrix_attrib_pointer(11, 1, 0);

  reset_pattern();
}


void plant1_5_app::update() 
{
  //if(!m_dirty_view)
    //return;
  
  auto buffer_size = sizeof(mat4) * m_mats.size();

  mat4 mat;
  if(rotating)
  {
    rotate_mat *= rotate(0.01f, pza);
    mat = v_mat * translate(world_center) * rotate_mat * translate(-world_center);
  }
  
  for (int i = 0; i < m_mats.size(); ++i) {
    if(rotating)
      mv_mats[i] = mat * m_mats[i];
    else
      mv_mats[i] = v_mat * m_mats[i];
    mvp_mats[i] = p_mat * mv_mats[i];
    mv_mat_its[i] = transpose(inverse(mv_mats[i]));
  }
  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MV_MAT]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, value_ptr(mv_mats.front()));

  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MVP_MAT]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, value_ptr(mvp_mats.front()));

  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MV_MAT_IT]);
  glBufferSubData(GL_ARRAY_BUFFER, 0, buffer_size, value_ptr(mv_mat_its.front()));
  //m_dirty_view = false;
}

void plant1_5_app::display() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(draw_seed)
  {
    glDisable(GL_DEPTH_TEST);
    llprg.use();
    glUniformMatrix4fv(llprg.ul_mvp_mat, 1, 0, glm::value_ptr(p_mat * v_mat));
    glUniform4f(llprg.ul_color, 1, 1, 1, 1);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    m_geometry.draw();
    glDisable(GL_BLEND);
  }
  else
  {
    glEnable(GL_DEPTH_TEST);
    prg.use();
    prg.update_lighting_uniforms(lights, lm, mtl, v_mat);
    //for (int i = 0; i < m_mats.size(); ++i) {
      //prg.update_uniforms(m_mats[i]);
      //capsule.draw();
    //}
    capsule.draw();
  }

  if(!display_help)
    return;

  if(draw_seed)
  {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // step
    debugger::draw_line(GL_LINES, m_vertices.begin(), m_vertices.begin() + m_step_vertices,
        p_mat * v_mat, 1, vec4(1, 1, 0, 1));

    // debug
    debugger::draw_line(GL_LINES, m_vertices.begin(), m_vertices.begin() + kci_debug_count->get_int(),
        p_mat * v_mat, 1, vec4(1, 0, 0, 1));

    if(m_last_step_vertices > 0)
    {
      // last step
      debugger::draw_line(GL_LINES, m_vertices.begin() + m_step_vertices - m_last_step_vertices,
          m_vertices.begin() + m_step_vertices, p_mat * v_mat, 1, vec4(0, 1, 0, 1));
    }

    // next step line
    auto next = min<GLint>(m_vertices.size(), m_step_vertices + 2);
    debugger::draw_line(GL_LINES, m_vertices.begin() + m_step_vertices, m_vertices.begin() + next,
        p_mat * v_mat, 1, vec4(0, 0, 1, 1));
    glDisable(GL_BLEND);
  }

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
  ss << "l : draw_seed : " << draw_seed << std::endl;
  ss << "h : help " << std::endl;
  ss << "fps : " << m_fps << std::endl;

  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void plant1_5_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
  update_bound(m_vertices);
}

void plant1_5_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(m_wnd, GL_TRUE);
        break;
      case GLFW_KEY_H:
        display_help ^= 1;
        break;
      case GLFW_KEY_L:
        draw_seed ^= 1;
        break;
      case GLFW_KEY_U:
        rotating ^= 1;
        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void plant1_5_app::reset_pattern()
{
  kci_lsystem_step->set<GLint>(0);
  GLint pattern_index = kci_pattern->get_int();
  GLint n = kci_n->get_int();
  GLfloat degree = kci_degree->get_float() * fpi / 180.0f;
  GLfloat scale = kci_scale_per_iteartion->get_float();

  vec3 step = vec3(0, 80 / pow(scale, n) , 0);
  auto op_F = std::make_shared<translate_operation>(step);
  auto op_f = std::make_shared<translate_only_operation>(step);
  auto op_yaw = std::make_shared<rotate_operation>(degree, pza);
  auto op_yaw_n = std::make_shared<rotate_operation>(-degree, pza);
  auto op_pitch = std::make_shared<rotate_operation>(degree, pxa);
  auto op_pitch_n = std::make_shared<rotate_operation>(-degree, pxa);
  auto op_roll = std::make_shared<rotate_operation>(degree, pya);
  auto op_roll_n = std::make_shared<rotate_operation>(-degree, pya);
  auto op_turn = std::make_shared<rotate_operation>(fpi, pza);
  auto op_l = std::make_shared<translate_operation>(step);
  auto op_r = std::make_shared<translate_operation>(step);

  auto& pattern = patterns[pattern_index];

  m_geometry_text = pattern.system.generate(pattern.axiom, n);

  m_turtle.register_operation('F', op_F);
  m_turtle.register_operation('f', op_f);
  m_turtle.register_operation('+', op_yaw);
  m_turtle.register_operation('-', op_yaw_n);
  m_turtle.register_operation('&', op_pitch);
  m_turtle.register_operation('^', op_pitch_n);
  m_turtle.register_operation('\\', op_roll);
  m_turtle.register_operation('/', op_roll_n);
  m_turtle.register_operation('|', op_turn);

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

  update_uniforms();
  update_bound(m_vertices);
}

void plant1_5_app::step()
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

  // set debug count to changed graph
  auto part0 = m_turtle.generate(text.substr(0, text.size() - left), mat4(1));
  m_vertices = m_turtle.generate(text, mat4(1));
  m_geometry.build_line(GL_LINES, m_vertices.begin(), m_vertices.end());

  kci_debug_count->set<GLint>(part0.size());
  m_last_step_vertices = part0.size() - m_step_vertices;
  m_step_vertices = part0.size();
  std::static_pointer_cast<kci_template<GLint>>(kci_debug_count)
    ->max_value(m_vertices.size());

  // set bound to next level
  if(kci_lsystem_step->get_int() == 1)
  {
    auto n = kci_n->get_int() + 1;
    auto next_text = pattern.system.generate(pattern.axiom, n);
    auto vertices = m_turtle.generate(next_text, mat4(1));
    update_bound(vertices);
  }

  update_uniforms();
}

void plant1_5_app::update_bound(const vec3_vector& vertices)
{
  // include entire bounding sphere into scene
  auto corners = bounding_box(vertices.begin(), vertices.end());
  world_center = (corners.first + corners.second) * 0.5f;
  auto r = glm::length(corners.second - corners.first) * 0.5f;
  GLfloat d = r / sin(fpi8) * (wnd_aspect() > 1.0f ? 1.0f : 1.0f/wnd_aspect()) ;
  auto l = d*1.05f/sqrt(3);
  lookat(world_center + vec3(l, -l, l), world_center, vec3(0, 0, 1), &v_mat);

  // clamp near far
  //p_mat = glm::perspective(fpi4, wnd_aspect(), d - r - 0.001f, d+r + 0.001f);
}

void plant1_5_app::update_uniforms()
{
  m_mats.resize(m_vertices.size() / 2);
  mv_mats.resize(m_mats.size());
  mvp_mats.resize(m_mats.size());
  mv_mat_its.resize(m_mats.size());
  if(m_vertices.empty())
    return;

  for (int i = 0; i < m_vertices.size(); i += 2 ) {
    const vec3& v0 = m_vertices[i];
    const vec3& v1 = m_vertices[i+1];
    GLfloat l = length(v1 - v0);
    vec3 pos = 0.5f * (v0 + v1);
    m_mats[i/2] = scale(translate(pos) * zxd::rotate_to_any(pza, v1 - v0), vec3(l));
  }

  auto buffer_size = sizeof(mat4) * m_mats.size();
  
  for (int i = 0; i < capsule.get_num_primitive_set(); ++i) {
    capsule.get_primitive_set(i).num_instance(m_mats.size());
  }

  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MV_MAT]);
  glBufferData(GL_ARRAY_BUFFER, buffer_size, 0, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MVP_MAT]);
  glBufferData(GL_ARRAY_BUFFER, buffer_size, 0, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, instance_buffer[MV_MAT_IT]);
  glBufferData(GL_ARRAY_BUFFER, buffer_size, 0, GL_DYNAMIC_DRAW);

  rotate_mat = mat4(1);

  m_dirty_view = true;
}

}

int main(int argc, char *argv[]) {
  zxd::plant1_5_app app;
  app.run();
}
