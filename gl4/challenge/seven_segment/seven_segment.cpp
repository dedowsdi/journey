#include "app.h"
#include "bitmap_text.h"
#include <sstream>
#include "capsule2d.h"
#include "common_program.h"

#define WIDTH 800
#define HEIGHT 800

namespace zxd {

capsule2d capsule;
lightless_program prg;
vec4 enable_color = vec4(1, 0, 0, 1);
vec4 disable_color = vec4(0.5, 0.5, 0.5, 1);

//std::vector<GLubyte> masks = {0x7e, 0x30, 0x6d, 0x79, 0x33, 0x5b, 0x5f, 0x70, 0x7f, 0x7b};
std::vector<GLubyte> masks = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f};
GLint mask_index = 0;

class seven_segment
{
protected:
  mat4_vector m_transforms;

public:
  seven_segment()
  {
    m_transforms.resize(7);
  }

  void set_segment(GLint index, const mat4& m)
  {
    if(index >= 7)
      throw std::runtime_error("indoex overflow");

    m_transforms[index] = m;
  }

  void build_classic_display(float seg_width, float seg_radius)
  {
    GLfloat hw = seg_width * 0.5f;
    m_transforms.clear();
    m_transforms.reserve(7);
    m_transforms.push_back(glm::translate(vec3(0, seg_width, 0)));
    m_transforms.push_back(glm::rotate(glm::translate(vec3(hw, hw, 0)), fpi2, pza));
    m_transforms.push_back(glm::rotate(glm::translate(vec3(hw, -hw, 0)), fpi2, pza));
    m_transforms.push_back(glm::translate(vec3(0, -seg_width, 0)));
    m_transforms.push_back(glm::rotate(glm::translate(vec3(-hw, -hw, 0)), fpi2, pza));
    m_transforms.push_back(glm::rotate(glm::translate(vec3(-hw, hw, 0)), fpi2, pza));
    m_transforms.push_back(mat4(1));

    for(auto& item : m_transforms)
    {
      item = glm::scale(item, vec3(0.96, 0.96, 1));
    }
    
  }

  void draw(GLubyte mask)
  {
    for (int i = 0; i < 7; ++i) {
      vec4& color =  mask & (1<<i) ? enable_color : disable_color;
      glUniform4fv(prg.ul_color, 1, glm::value_ptr(color));
      glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(prg.vp_mat * m_transforms[i]));
      capsule.draw();
    }
  }

};

seven_segment classic;

class seven_segment_app : public app {
protected:
  bitmap_text m_text;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "seven_segment_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }
  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    prg.vp_mat = zxd::rect_ortho(5, 5, wnd_aspect());

    capsule.type(capsule2d::CT_POINT);
    capsule.width(1);
    capsule.radius(0.125);
    capsule.build_mesh();

    classic.build_classic_display(capsule.width(), capsule.radius());
  }

  virtual void update() {
    if(m_frame_number % 30 == 0)
      mask_index = ++mask_index % masks.size();
  }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    classic.draw(masks[mask_index]);

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
  zxd::seven_segment_app app;
  app.run();
}
