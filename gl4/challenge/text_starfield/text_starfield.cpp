#include <sstream>

#include "app.h"
#include "bitmap_text.h"
#include "freetype_text.h"
#include "bitmap_text.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd {

const GLint width = 800;
const GLint height = 800;
const GLint num_stars = 1000;

freetype_text ft("font/DejaVuSansMono.ttf");
std::shared_ptr<bitmap_text> bt;

class text_starfield_program : public program
{
public:
  GLint ul_diffuse_map;

protected:

  void attach_shaders()
  {
    attach(GL_VERTEX_SHADER, "shader4/text_starfield.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/text_starfield.fs.glsl");
  }

  void bind_uniform_locations()
  {
    uniform_location(&ul_vp_mat, "vp_mat");
    uniform_location(&ul_m_mat, "m_mat");
    uniform_location(&ul_diffuse_map, "diffuse_map");
  }

  void bind_attrib_locations()
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "texcoord");
  }

} prg;

std::vector<uint32_t> codepoints;
uint32_t randomCharacter()
{
  return codepoints[glm::linearRand(0.0, 0.999999) * codepoints.size()];
}

struct star_vertex
{
  vec4 vertex;
  vec2 texcoord;
};

class Star
{
public:
  uint32_t m_codepoint;
  GLfloat m_speed;
  mat4 m_transform;
  GLuint m_vao, m_vbo;
  Star()
  {
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &m_vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    GLint stride = sizeof(star_vertex);
    glBufferData(GL_ARRAY_BUFFER, stride * 4, 0, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, BUFFER_OFFSET(sizeof(vec4)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    reset();
  }

  void update(GLfloat dt)
  {
    m_transform[3][2] += dt * m_speed;
    if(m_transform[3][2] > 0)
    {
      reset();
    }
  }

  const mat4& transform() const { return m_transform; }
  void transform(const mat4& v){ m_transform = v; }

  void reset()
  {
    m_transform = glm::translate(vec3(glm::linearRand(vec2(-100), vec2(100)), -100));
    m_speed = glm::linearRand(1.0f, 50.00f);
    //m_transform.x = glm::linearRand(-1.0, 1.0);
    //m_transform.y = glm::linearRand(-1.0, 1.0);
    //m_transform.z = 1;
    //m_transform = mat4(1.0f);
    auto& glyph = bt->get_glyph(randomCharacter());
    bool reverse_horizontal = glm::linearRand(0.0f, 1.0f) < 0.5f;
    bool reverse_vertical = glm::linearRand(0.0f, 1.0f) < 0.5f;
    auto s_min = reverse_horizontal ? glyph.s_max : glyph.s_min;
    auto s_max = reverse_horizontal ? glyph.s_min : glyph.s_max;
    auto t_min = reverse_vertical ? glyph.t_max : glyph.t_min;
    auto t_max = reverse_vertical ? glyph.t_min : glyph.t_max;

    star_vertex vertices[4];
    vertices[0] = {vec4(-1, -1, 0, 1), vec2(s_min, t_min)};
    vertices[1] = {vec4( 1, -1, 0, 1), vec2(s_max, t_min)};
    vertices[2] = {vec4( 1,  1, 0, 1), vec2(s_max, t_max)};
    vertices[3] = {vec4(-1,  1, 0, 1), vec2(s_min, t_max)};

    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), glm::value_ptr(vertices[0].vertex));
  }

  void draw()
  {
    glUniformMatrix4fv(prg.ul_m_mat, 1, 0, glm::value_ptr(m_transform));
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
  }

};

using Stars = std::vector<Star>;

class text_starfield : public app {
private:
  bitmap_text m_text;
  GLfloat m_font_aspect_ratio;
  Stars m_stars;

public:
  void init_info() override;
  void create_scene() override;

  void update() override;

  void display() override;

  void glfw_resize(GLFWwindow *wnd, int w, int h) override;

  void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) override;
};

void text_starfield::init_info() {
  app::init_info();
  m_info.title = "text_starfield";
  m_info.wnd_width = width;
  m_info.wnd_height = height;
}

void text_starfield::create_scene() {
  glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

  ft.init();
  ft.set_height(18);
  ft.reset_content_to_all();

  bt = ft.create_bitmap_text(2048, 2048);
  m_font_aspect_ratio = static_cast<GLfloat>(bt->height()) / bt->max_advance();
  codepoints = bt->code_points();

  m_text.init();

  prg.p_mat = glm::perspective<GLfloat>(45, wnd_aspect(), 1.0, 1000);
  prg.v_mat = translate(vec3(0, 0, -1));

  prg.init();
  m_stars.resize(num_stars);
}

void text_starfield::update() {
  for(auto& item : m_stars) {
    item.update(m_delta_time);
  }
}

void text_starfield::display() {
  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  prg.use();
  glBindTexture(GL_TEXTURE_2D, bt->texture());
  glUniformMatrix4fv(prg.ul_vp_mat, 1, 0, glm::value_ptr(prg.p_mat * prg.v_mat));
  glUniform1i(prg.ul_diffuse_map, 0);

  for(auto& item : m_stars)
  {
    item.draw();
  }

  glDisable(GL_BLEND);

  if(!m_display_help)
    return;

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  std::stringstream ss;
  ss << "";
  m_text.print(ss.str(), 10, m_info.wnd_height - 20);
  glDisable(GL_BLEND);
}

void text_starfield::glfw_resize(GLFWwindow *wnd, int w, int h) {
  app::glfw_resize(wnd, w, h);
  m_text.reshape(m_info.wnd_width, m_info.wnd_height);
}

void text_starfield::glfw_key(
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

}

int main(int argc, char *argv[]) {
  zxd::text_starfield app;
  app.run();
}
