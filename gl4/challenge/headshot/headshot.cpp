#include <app.h>

#include <sstream>
#include <algorithm>
#include "../lightning/lightning.cpp"
#include <filter.h>
#include <quad.h>

namespace zxd {

GLuint create_texture(GLint width, GLint height, GLvoid* data = 0);

class headshot_app : public app {

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

  void reset_lightning();

  void update_lightning();

private:
  std::vector< std::unique_ptr<lightning> > _lightnings;
  kcip _min_subdivids;
  kcip _max_subdivids;
  kcip _max_fork_angle;
  kcip _max_jitter;
  kcip _billboard_width;
  kcip _color0;
  kcip _color1;
  kcip _blur_times;
  kcip _bloom_exposure;
  kcip _blend_previous;
  kcip _num_lightnings;
  mat4 _p_mat;
  mat4 _v_mat;
  GLuint _color_map;
  GLuint _diffuse_map;
  GLuint _brightness_map;
  GLuint _fbo;
  pingpong _blur_tex;
  brightness_color_filter _bc_filter;
  gaussian_blur_filter _gb_filter;
};

void headshot_app::init_info()
{
  app::init_info();
  m_info.title = "headshot_app";
  m_info.wnd_width = 512;
  m_info.wnd_height = 512;
}

void headshot_app::create_scene()
{
  auto callback = [this](auto kci) { this->reset_lightning(); };
  _min_subdivids = m_control.add('Q', 4, 3, 100, 1, callback);
  _max_subdivids = m_control.add('W', 16, 3, 100, 1, callback);
  _max_fork_angle = m_control.add('E', 45, 0, 360, 1, callback);
  _max_jitter = m_control.add('R', 0.5f, 0.0f, 1.0f, 0.1f, callback);
  _billboard_width = m_control.add('U', 0.5f, 0.1f, 200.0f, 1.0f, callback);
  _color0 = m_control.add('I', vec4(1), vec4(0), vec4(1), vec4(0.1), callback);
  _color1 = m_control.add('O', vec4(1, 0, 1, 1), vec4(0), vec4(1), vec4(0.1), callback);
  _blur_times = m_control.add('P', 16, 1, 100, 1);
  _bloom_exposure = m_control.add('X', 2.5f, 0.1f, 10.0f, 1.0f);
  _blend_previous = m_control.add_bool('B', false, callback);
  _num_lightnings = m_control.add('A', 16, 1, 100, 1, callback);

  _p_mat = glm::perspectiveNO(fpi4, wnd_aspect(), 0.1f, 1000.0f);
  _v_mat = glm::lookAt(vec3(0, -170, 0), vec3(0), vec3(0, 0, 1));
  set_v_mat(&_v_mat);

  reset_lightning();

  _color_map = create_texture(m_info.wnd_width, m_info.wnd_height, 0);
  _brightness_map = create_texture(m_info.wnd_width, m_info.wnd_height, 0);
  _diffuse_map = create_texture(m_info.wnd_width, m_info.wnd_height, 0);
  _blur_tex.ping( create_texture(m_info.wnd_width, m_info.wnd_height, 0) );
  _blur_tex.pong( create_texture(m_info.wnd_width, m_info.wnd_height, 0) );

  glGenFramebuffers(1, &_fbo);
}

void headshot_app::update()
{
}

void headshot_app::display()
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  mat4 mvp_mat = _p_mat * _v_mat;

  for (auto& l : _lightnings)
  {
    l->draw(mvp_mat);
  }

  if (_blend_previous->get_bool())
  {
    glEnable(GL_BLEND);
  }

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _diffuse_map);
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, m_info.wnd_width, m_info.wnd_height, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  _bc_filter.filter(_diffuse_map, _color_map, _brightness_map);
  _gb_filter.filter(_brightness_map, _blur_tex, _blur_times->get_int());

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  bloom(_color_map, _blur_tex.pong(), _bloom_exposure->get_float());
  glDisable(GL_BLEND);

  if (m_save_image)
  {
    save_colorbuffer("headshot.png");
    m_save_image = false;
  }

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);
  std::stringstream ss;
  ss << "fps : " << m_fps << "\n";
  ss << "q : min subdivides : " << _min_subdivids->get_int() << "\n";
  ss << "w : max subdivides : " << _max_subdivids->get_int() << "\n";
  ss << "e : max fork angle : " << _max_fork_angle->get_int() << "\n";
  ss << "r : max jitter : " << _max_jitter->get_float() << "\n";
  ss << "u : billboard width : " << _billboard_width->get_float() << "\n";
  ss << "i : color0 : " << _color0->get<vec4>() << "\n";
  ss << "o : color1 : " << _color1->get<vec4>() << "\n";
  ss << "p : blur times : " << _blur_times->get_int() << "\n";
  ss << "x : exposure : " << _bloom_exposure->get_float() << "\n";
  ss << "b : blend previous : " << _blend_previous->get_bool() << "\n";
  ss << "a : num lightning : " << _num_lightnings->get_int() << "\n";
  ss << "j : random format \n";
  ss << "k : random path \n";
  m_text.print(ss.str(), 10, m_info.wnd_height - 20, vec4(0, 1, 0, 1));
  glDisable(GL_BLEND);
}

//--------------------------------------------------------------------
void headshot_app::update_lightning()
{
  vec3 camera_pos = zxd::eye_pos(_v_mat);
  for (auto& l : _lightnings)
  {
    l->update_buffer(camera_pos);
  }
}

void headshot_app::reset_lightning()
{
  // create equalateral triangle lightning
  GLfloat sqrt3 = sqrt(3);
  std::vector<vec3> triangle = {
    vec3(0, 0, sqrt3/3) * 100.0f,
    vec3(-0.5, 0, -sqrt3/6) * 100.0f,
    vec3(0.5, 0, -sqrt3/6) * 100.0f,
  };

  _lightnings.clear();

  // main laterial
  for (unsigned i = 0; i < 3; ++i)
  {
      auto& v0 = triangle[i];
      auto& v1 = triangle[ (i+1)%3 ];
      auto l = std::make_unique<lightning>( "", std::vector<vec3>( {v0, v1} ) );
      l->set_billboard_width(_billboard_width->get_float() * 5.0f);
      l->set_color0(_color0->get<vec4>());
      l->set_color1(_color1->get<vec4>());
      _lightnings.push_back(std::move(l));
  }

  for (unsigned j = 0; j < _num_lightnings->get_int(); ++j)
  {
    for (auto i = 0; i < 3; ++i)
    {
      auto& v0 = triangle[i];
      auto& v1 = triangle[ (i+1)%3 ];

      // create random pattern
      std::string pattern;
      if (_min_subdivids->get_int() > _max_subdivids->get_int())
      {
        return;
      }

      auto n = glm::linearRand(_min_subdivids->get_int(), _max_subdivids->get_int());
      std::generate_n(std::back_inserter(pattern), n,
        []() { return glm::linearRand(0.0f, 1.0f) > 0.95f ? 'f' : 'j'; });

      auto l = std::make_unique<lightning>( pattern, std::vector<vec3>( {v0, v1} ) );
      l->set_max_fork_angle(_max_fork_angle->get_int() * fpi / 180.0f);
      l->set_max_jitter(_max_jitter->get_float());
      l->set_billboard_width(_billboard_width->get_float());
      l->set_color0(_color0->get<vec4>());
      l->set_color1(_color1->get<vec4>());
      _lightnings.push_back(std::move(l));
    }
  }

  update_lightning();

}

void headshot_app::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
}

void headshot_app::glfw_key(
  GLFWwindow *wnd, int key, int scancode, int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case 'J':
        reset_lightning();
        break;
      case 'K':
        update_lightning();
        break;
      default:
        break;
    }
  }
  app::glfw_key(wnd, key, scancode, action, mods);
}

void headshot_app::glfw_mouse_button(GLFWwindow *wnd, int button, int action,
                               int mods)
{
  app::glfw_mouse_button(wnd, button, action, mods);
}

void headshot_app::glfw_mouse_move(GLFWwindow *wnd, double x, double y)
{
  app::glfw_mouse_move(wnd, x, y);
}

GLuint create_texture(GLint width, GLint height, GLvoid* data)
{
  GLuint tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  return tex;
}

}

int main(int argc, char *argv[]) {
  zxd::headshot_app app;
  app.run();
}
