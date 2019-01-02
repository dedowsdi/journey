/*
 * https://www.myphysicslab.com/pendulum/double-pendulum-en.html
 */
#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "common_program.h"
#include "circle.h"
#include "quad.h"
#include "debugger.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

lightless_program prg;
circle ball;

GLfloat m1 = 1;
GLfloat m2 = 1;
GLfloat l1 = 30;
GLfloat l2 = 30;
GLfloat theta1 = fpi2;
GLfloat theta2 = fpi4;
GLfloat radius = 2;
GLfloat g = 0.2;

GLfloat av1 = 0;
GLfloat av2 = 0;
GLfloat aa1 = 0;
GLfloat aa2 = 0;

vec2 origin = vec2(0, 50);
vec2 pos1;
vec2 pos2;
vec2 last_pos;

class double_pendulum_app : public app {
protected:
  bitmap_text m_text;
  GLuint m_tex;
  GLuint m_fbo;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "double_pendulum_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    ball.radius(radius);
    ball.slice(64);
    ball.build_mesh();

    prg.init();
    prg.p_mat = zxd::rect_ortho(100, 100, wnd_aspect());
    prg.v_mat = glm::mat4(1);
    prg.vp_mat = prg.p_mat * prg.v_mat;

    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, wnd_width(), wnd_height(),
        0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

    glGenFramebuffers(1, &m_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_tex, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");
    glClear(GL_COLOR_BUFFER_BIT);
  }

  virtual void update() 
  {
    last_pos = pos2;
    pos1 = origin + l1 * vec2(sin(theta1), -cos(theta1));
    pos2 = pos1 + l2 * vec2(sin(theta2), -cos(theta2));

    av1 += aa1;
    av2 += aa2;

    theta1 += av1;
    theta2 += av2;

    float denominator = 2*m1 + m2 - m2 * cos(2*theta1 - 2*theta2); 
    aa1= -g*(2*m1 + m2)*sin(theta1) - m2*g*sin(theta1 - 2*theta2) -
      2*sin(theta1 - theta2)*m2*(av2*av2*l2 + av1*av1*l1*cos(theta1 - theta2));
    aa2 = 2*sin(theta1 - theta2)*(av1*av1*l1*(m1 + m2) + g*(m1 + m2)*cos(theta1)
        + av2*av2*l2*m2*cos(theta1 - theta2));
    aa1 /= (l1*denominator);
    aa2 /= (l2*denominator);

  }

  virtual void display() {

    glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
    debugger::draw_line(last_pos, pos2, prg.vp_mat, 1, vec4(0));

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, m_tex);
    draw_quad(0);

    prg.use();
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat));
    glUniform4f(prg.ul_color, 0,0,0,1);

    debugger::draw_line(origin, pos1, prg.vp_mat, 1, vec4(0));
    debugger::draw_line(pos1, pos2, prg.vp_mat, 1, vec4(0));

    mat4 mvp_mat = glm::translate(prg.vp_mat, vec3(pos1, 0));
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    ball.draw();

    mvp_mat = glm::translate(prg.vp_mat, vec3(pos2, 0));
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    ball.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
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
        default:
          break;
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }
};
}

int main(int argc, char *argv[]) {
  zxd::double_pendulum_app app;
  app.run();
}
