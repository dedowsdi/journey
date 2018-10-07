#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include <sstream>
#include "param.h"

#define WIDTH 800
#define HEIGHT 800

#define BUFFER_OFFSET(x) ((GLubyte*)NULL + (x))

namespace zxd {

twod_program0 prg;

key_control_item* trunk_len;
key_control_item* min_branch_len;
key_control_item* left_angle_min;
key_control_item* left_angle_max;
key_control_item* right_angle_min;
key_control_item* right_angle_max;

struct branch
{
  GLfloat len;
  mat4 transform;

  branch* left = 0;
  branch* right = 0;

  branch(GLfloat l):
    len(l)
  {
  }

  ~branch()
  {
    if(left) delete left;
    if(right) delete right;
  }

  void spawn(GLfloat min_branch_len, const vec2& scale_range, const vec2& left_rotate_range, const vec2& right_rotate_range)
  {

    GLfloat l = glm::linearRand(scale_range.x, scale_range.y) * len;
    if(l >= min_branch_len) 
    {
      GLfloat lr = glm::linearRand(left_rotate_range.x, left_rotate_range.y);
      left = new branch(l);
      left->transform = glm::translate(vec3(0, len, 0)) * glm::rotate(lr, pza);
      left->spawn(min_branch_len, scale_range, left_rotate_range, right_rotate_range);
    }


    l = glm::linearRand(scale_range.x, scale_range.y) * len;
    if(l >= min_branch_len) 
    {
      GLfloat rr = glm::linearRand(right_rotate_range.x, right_rotate_range.y);
      right = new branch(l);
      right->transform = glm::translate(vec3(0, len, 0)) * glm::rotate(rr, pza);
      right->spawn(min_branch_len, scale_range, left_rotate_range, right_rotate_range);
    }

  }

  void compile(vec2_vector& vertices, glm::mat4 parent_transform)
  {
    parent_transform = parent_transform * transform;
    vec4 v0 = parent_transform * vec4(0,0,0,1);
    vec4 v1 = parent_transform * vec4(0,len,0,1);
    vertices.push_back(v0.xy());
    vertices.push_back(v1.xy());

    if(left)
      left->compile(vertices, parent_transform);

    if(right)
      right->compile(vertices, parent_transform);
  }

};

class fractal_tree
{
protected:
  branch* root = 0;
  GLfloat m_trunk_len;
  GLfloat m_min_branch_len;

  GLuint m_vao = -1;
  GLuint m_vbo = -1;
  mat4 m_transform;
  vec2 m_len_scale_range = vec2(0.6);
  vec2 m_left_rotate_range = vec2(fpi4);
  vec2 m_right_rotate_range = vec2(-fpi4);
  vec2_vector m_vertices;

public:

  ~fractal_tree()
  {
    if(root)
      delete root;
  }

  fractal_tree(GLfloat trunk_len = 1, GLfloat min_branch_len = 0.1):
    m_trunk_len(trunk_len),
    m_min_branch_len(min_branch_len)
  {
  }

  void rebuild()
  {
    if(m_vao == -1)
      glGenVertexArrays(1, &m_vao);
    if(m_vbo == -1)
      glGenBuffers(1, &m_vbo);

    if(root)
      delete root;

    root = new branch(m_trunk_len);
    root->transform = glm::mat4(1);
    root->spawn(m_min_branch_len, m_len_scale_range, m_left_rotate_range, m_right_rotate_range);
  }

  void compile()
  {
    m_vertices.clear();

    GLuint spawn_times = glm::ceil(glm::log2(m_trunk_len/ m_min_branch_len));
    GLuint max_branches = glm::pow(2, spawn_times + 1) - 1;
    m_vertices.reserve(max_branches * 2);

    branch* b = root;
    mat4 m(1);
    b->compile(m_vertices, m);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vec2), glm::value_ptr(m_vertices.front()), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  GLuint vao() const { return m_vao; }
  GLuint vbo() const { return m_vbo; }

  GLfloat trunk_len() const { return m_trunk_len; }
  void trunk_len(GLfloat v){ m_trunk_len = v; }

  GLfloat min_branch_len() const { return m_min_branch_len; }
  void min_branch_len(GLfloat v){ m_min_branch_len = v; }

  void draw()
  {
    glBindVertexArray(m_vao);


    glDrawArrays(GL_LINES, 0, m_vertices.size());
  }

  const vec2& len_scale_range() const { return m_len_scale_range; }
  void len_scale_range(const vec2& v){ m_len_scale_range = v; }

  const vec2& left_rotate_range() const { return m_left_rotate_range; }
  void left_rotate_range(const vec2& v){ m_left_rotate_range = v; }

  const vec2& right_rotate_range() const { return m_right_rotate_range; }
  void right_rotate_range(const vec2& v){ m_right_rotate_range = v; }

  mat4 transform() const { return m_transform; }
  void transform(mat4 v){ m_transform = v; }
};


class fractal_tree_app : public app {
protected:
  bitmap_text m_text;
  fractal_tree m_tree;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "fractal_tree_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    auto update_callback = std::bind(std::mem_fn(&fractal_tree_app::update_tree), this, std::placeholders::_1);
    trunk_len = m_control.add_control(GLFW_KEY_Q, 200, 10, 400, 10, update_callback);
    min_branch_len = m_control.add_control(GLFW_KEY_W, 2, 0.1, 10, 0.5, update_callback);
    left_angle_min = m_control.add_control(GLFW_KEY_U, fpi4, -10000, 10000, 0.1, update_callback);
    left_angle_max = m_control.add_control(GLFW_KEY_I, fpi4, -10000, 10000, 0.1, update_callback);
    right_angle_min = m_control.add_control(GLFW_KEY_O, -fpi4, -10000, 10000, 0.1, update_callback);
    right_angle_max = m_control.add_control(GLFW_KEY_P, -fpi4, -10000, 10000, 0.1, update_callback);

    m_tree.transform(glm::translate(vec3(WIDTH * 0.5 , 0, 0)));
    update_tree(0);
  }

  void update_tree(const key_control_item* item)
  {
    m_tree.trunk_len(trunk_len->value);
    m_tree.min_branch_len(min_branch_len->value);
    m_tree.left_rotate_range(vec2(left_angle_min->value, left_angle_max->value));
    m_tree.right_rotate_range(vec2(right_angle_min->value, right_angle_max->value));
    m_tree.rebuild();
    m_tree.compile();
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();

    prg.mvp_mat = prg.p_mat * prg.v_mat * m_tree.transform();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.mvp_mat));
    glUniform4f(prg.ul_color, 1, 1, 1, 1);

    m_tree.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : trunk len " << trunk_len->value << std::endl;
    ss << "wW : min branch len " << min_branch_len->value << std::endl;
    ss << "uU : left rotate min " << left_angle_min->value << std::endl;
    ss << "iI : left rotate max " << left_angle_max->value << std::endl;
    ss << "oO : right rotate min " << right_angle_min->value << std::endl;
    ss << "pP : right rotate max " << right_angle_max->value << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
  }

  virtual void glfw_key(
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
};
}

int main(int argc, char *argv[]) {
  zxd::fractal_tree_app app;
  app.run();
}
