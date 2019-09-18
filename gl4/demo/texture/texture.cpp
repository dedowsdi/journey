#include <sstream>

#include <common_program.h>
#include <app.h>
#include <texutil.h>
#include <xyplane.h>
#include <glenumstring.h>

namespace zxd {

const GLint width = 800;
const GLint height = 800;

lightless_program prg;

mat4 p_mat;
mat4 v_mat;
mat4 m_mat;
GLuint tex;

xyplane quad(100, 100, 3);
kcip wrap_s;
kcip wrap_t;
kcip min_filter;
kcip mag_filter;
kcip border_color;

void reset_texture(const kci* p)
{
  glBindTexture(GL_TEXTURE_2D, tex);

  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_s->get_int_enum());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_t->get_int_enum());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter->get_int_enum());
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, mag_filter->get_int_enum());
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR,
    value_ptr(border_color->get<vec4>()));
}

class texture_app : public app {
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

void texture_app::init_info() {
  app::init_info();
  m_info.title = "texture_app";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

void texture_app::create_scene()
{
  prg.with_texcoord = true;
  prg.init();

  p_mat = rect_ortho(55, 55, wnd_aspect());
  v_mat = mat4(1);
  m_mat = mat4(1);

  quad.set_texcoord(vec2(0, 0), vec2(3));
  quad.build_mesh({attrib_semantic::vertex, attrib_semantic::texcoord});
  wrap_s = m_control.add_enum('Q',
    {GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT}, 0,
    reset_texture);
  wrap_t = m_control.add_enum('W',
    {GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER, GL_MIRRORED_REPEAT, GL_REPEAT}, 0,
    reset_texture);
  min_filter = m_control.add_enum('E',
    {GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST,
      GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR},
    0, reset_texture);
  mag_filter =
    m_control.add_enum('R', {GL_NEAREST, GL_LINEAR}, 0, reset_texture);
  border_color =
    m_control.add('U', vec4(1), vec4(0), vec4(1), vec4(0.1), reset_texture);

  glGenTextures(1, &tex);
  reset_texture(0);

  auto img = fipLoadResource32("texture/bricks2.jpg");
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, img.getWidth(), img.getHeight(), 0,
    GL_RGBA, GL_UNSIGNED_BYTE, img.accessPixels());
  glGenerateMipmap(GL_TEXTURE_2D);

}

void texture_app::update() {}

void texture_app::display() {
  glClear(GL_COLOR_BUFFER_BIT);

  prg.use();
  auto mvp_mat = p_mat * v_mat * m_mat;
  glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, value_ptr(mvp_mat));

  glBindTexture(GL_TEXTURE_2D, tex);
  quad.draw();

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "q : wrap_s : " << gl_tex_wrap_to_string(wrap_s->get_int_enum()) << std::endl;
  ss << "w : wrap_t : " << gl_tex_wrap_to_string(wrap_t->get_int_enum()) << std::endl;
  ss << "e : min_filter : " << gl_tex_filter_to_string(min_filter->get_int_enum())
     << std::endl;
  ss << "r : max_filter : " << gl_tex_filter_to_string(mag_filter->get_int_enum())
     << std::endl;
  ss << "u : border color : " << border_color->get<vec4>() << std::endl;
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void texture_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
}

void texture_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void texture_app::glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                               int mods)
{
  app::glfw_mouse_button(wnd, button, action, mods);
}

void texture_app::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
}

}

int main(int argc, char *argv[]) {
  zxd::texture_app app;
  app.run();
}
