/*
 * lsystem is a text based system, it works like this:
 * it starts with an axiom and some rules:
 *  axiom : A
 *  rule0 : A->AB
 *  rule1 : B->A
 *
 *     A
 *  1  AB
 *  2  ABA
 *  3  ABAAAB
 *  4  ..
 *
 * this program use only onle rule:
 * F->FF+[+F-F-F]-[-F+F+F]
 *
 * character meaning:
 *  F draw line, translate forward
 *  + rotate left
 *  - rotate right
 *  [ store transform
 *  ] restore transform
 *
 *
 */

#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include "lsystem.h"


#include <algorithm>
#include <mat_stack.h>
#include <sstream>

#define WIDTH 800
#define HEIGHT 800


#define BUFFER_OFFSET(x) ((GLubyte*)NULL + (x))

namespace zxd {

lightless_program prg;
GLint iteration_count = 4;
GLfloat angle = 25.0f;

class lsystem_tree
{
protected:
  GLuint m_vao = -1;
  GLuint m_vbo = -1;
  GLfloat m_angle;
  GLfloat m_f_len;
  vec2_vector m_vertices;

public:
  void compile(const std::string& text)
  {
    m_vertices.clear();
    m_vertices.reserve(std::count(text.begin(), text.end(), 'F') * 2);
    GLfloat len = m_f_len;

    mat_stack ms;
    for(char c : text)
    {
      switch(c)
      {
        // F->FF+[+F-F-F]-[-F+F+F]
        case '[':
          ms.push();
          break;

        case ']':
          ms.pop();
          break;

        case '+':
          ms.rotate(-m_angle, vec3(0, 0, 1));
          break;

        case '-':
          ms.rotate(m_angle, vec3(0, 0, 1));
          break;

        case 'F':
          {
            vec4 v0 = ms.top() * vec4(0,0,0,1);
            vec4 v1 = ms.top() * vec4(0,len,0,1);
            m_vertices.push_back(v0.xy());
            m_vertices.push_back(v1.xy());
            ms.translate(vec3(0, len, 0));
          }
          break;

        default:
          std::cout << "unknow lsystem character " << c << std::endl;
      }
    }
    
    if(ms.size() != 1)
      std::cout << "warning : missing ] " << std::endl;

    if(m_vao == -1)
      glGenVertexArrays(1, &m_vao);

    if(m_vbo == -1)
      glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vec2), 
        glm::value_ptr(m_vertices.front()), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, 0, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

  }

  GLfloat angle() const { return m_angle; }
  void angle(GLfloat v){ m_angle = v; }

  GLfloat f_len() const { return m_f_len; }
  void f_len(GLfloat v){ m_f_len = v; }

  void draw()
  {
    glBindVertexArray(m_vao);
    glDrawArrays(GL_LINES, 0, m_vertices.size());
  }
};


class lsystem_tree_app : public app {
protected:
  bitmap_text m_text;
  lsystem m_lsystem;
  lsystem_tree m_tree;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "lsystem_tree_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());
    
    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    update_tree();
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();

    mat4 m_mat = glm::translate(vec3(WIDTH*0.5, 0, 0));
    prg.mvp_mat = prg.p_mat * prg.v_mat * m_mat;
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.mvp_mat));
    glUniform4f(prg.ul_color, 1, 1, 1, 1);
    m_tree.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "qQ : iteration count " << iteration_count << std::endl;
    ss << "wW : angle " << angle << std::endl;
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  void update_tree()
  {
    m_lsystem.add_rule('F', "FF+[+F-F-F]-[-F+F+F]");
    std::string text = m_lsystem.generate("F", iteration_count);

    //std::cout << text << std::endl;
    m_tree.f_len(200.0f / glm::pow(2, iteration_count));
    m_tree.angle(glm::radians(angle));
    m_tree.compile(text);
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
          iteration_count += mods & GLFW_MOD_SHIFT ? -1 : 1;
          iteration_count = glm::max(1, iteration_count);
          update_tree();
          break;

        case GLFW_KEY_W:
          angle += mods & GLFW_MOD_SHIFT ? -1 : 1;
          update_tree();

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
  zxd::lsystem_tree_app app;
  app.run();
}
