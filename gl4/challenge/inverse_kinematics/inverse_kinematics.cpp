#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"
#include "debugger.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

glm::mat4 v_mat;
glm::mat4 p_mat;

lightless_program prg;

struct segment
{
  GLfloat angle = 0;
  GLfloat len;
  vec2 start;
  vec2 end;

  segment* child = 0;
  segment* parent = 0;

  segment()
  {
  }

  ~segment()
  {
    if(child)
      delete child;
  }

  void update_end()
  {
    end = start + vec2(cos(angle), sin(angle)) * len;
  }

  void update_from_parent()
  {
    if(!parent)
      return;

    start = parent->end;
    update_end();
    if(child)
      child->update_from_parent();
  }

  void update_from_child()
  {
    if(!child)
      return;
    reach(child->start);

    if(parent)
      parent->update_from_child();
  }

  void set_child(segment* t)
  {
    child = t;
    t->start = end;
    t->parent = this;
    t->update_end();
  }

  void reach(const vec2& target)
  {
    vec2 dir = target - start;
    angle = atan2(dir.y, dir.x);
    start = target - glm::normalize(dir) * len;
    end = target;
  }

  segment* get_tail()
  {
    return child ? child->get_tail() : this;
  }

  segment* get_root()
  {
    return parent ? parent->get_root() : this;
  }

  void lock(const vec2& pos)
  {
    segment* root = get_root();
    root->move_to(pos);
  }

  // translation only
  void move_to(const vec2& pos)
  {
    start = pos;
    update_end();
    if(child)
      child->update_from_parent();
  }

  // reach something with tail, update parents
  void tail_reach(const vec2& target)
  {
    segment* tail = get_tail();
    tail->reach(target);
    if(tail->parent)
      tail->parent->update_from_child();

  }

};

struct tentacle
{
  segment* root = 0;
  bool lock_root = false;
  vec2 lock_pos = vec2(0,0);

  tentacle(GLfloat len, GLuint num_segments)
  {
    root = new segment;
    root->start = vec2();
    root->angle = fpi2;
    root->len = 5;

    GLfloat seg_len  = len / num_segments;

    segment* parent = root;
    for (int i = 0; i < num_segments; ++i) 
    {
      segment* current = new segment;
      current->len = seg_len;
      parent->set_child(current);
      parent = current;
    }
  }

  void lock(const vec2& pos)
  {
    lock_root = true;
    lock_pos = pos;
    root->move_to(pos);
  }

  void move_to(const vec2& pos)
  {
    root->move_to(pos);
  }

  void reach(const vec2& pos)
  {
    root->tail_reach(pos);
    if(lock_root)
      root->lock(lock_pos);
  }

  void draw(const mat4& mat)
  {
    segment* current = root;
    while(current)
    {
      debugger::draw_line(current->start, current->end, mat);
      current = current->child;
    }
  }

  ~tentacle()
  {
    if(root)
      delete root;
  }
};

class inverse_kinematics_app : public app {
protected:
  bitmap_text m_text;
  tentacle tentacle0 = tentacle(300, 100);
  tentacle tentacle1 = tentacle(300, 100);
  tentacle tentacle2 = tentacle(300, 100);

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "inverse_kinematics_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());

    dvec2 mouse;
    glfwGetCursorPos(m_wnd, &mouse.x, &mouse.y);
      mouse.y = glfw_to_gl(mouse.y);

    tentacle0.lock(vec2(0, HEIGHT*0.5));
    tentacle1.lock(vec2(WIDTH, HEIGHT*0.5));

    tentacle0.reach(mouse);
    tentacle1.reach(mouse);
    tentacle2.reach(mouse);

  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    //prg.use();
    tentacle0.draw(p_mat);
    tentacle1.draw(p_mat);
    tentacle2.draw(p_mat);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "inverse kinematic, follow mouse" << std::endl;
    //ss << "q : lock root : " << lock_root << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(wnd_width(), wnd_height());
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ESCAPE:
          glfwSetWindowShouldClose(m_wnd, GL_TRUE);
          break;
        case GLFW_KEY_Q:
          //lock_root = !lock_root;
          break;
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y)
  {
    app::glfw_mouse_move(wnd, x, y);
    vec2 pos = app::glfw_to_gl(vec2(x, y));
    tentacle0.reach(pos);
    tentacle1.reach(pos);
    tentacle2.reach(pos);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::inverse_kinematics_app app;
  app.run();
}
