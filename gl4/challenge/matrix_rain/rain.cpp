#include "rain.h"

#include <algorithm>

#include "bitmap_text.h"
#include "program.h"

#define BUFFER_OFFSET(bytes) ((GLubyte *)NULL + (bytes))

namespace zxd
{

namespace
{
class matrix_rain_program : public program
{
public:
  void update_uniforms(GLuint tui_font) {
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
    glUniform1i(ul_font_map, tui_font);
  }

  void reshape(GLuint wnd_width, GLuint wnd_height) {
    mvp_mat = glm::ortho(
      0.0f, (GLfloat)wnd_width, 0.0f, (GLfloat)wnd_height, -1.0f, 1.0f);
  }

private:
  GLint ul_font_map;
  void attach_shaders() override
  {
    attach(GL_VERTEX_SHADER, "shader4/matrix_rain.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "shader4/matrix_rain.fs.glsl");
  }

  void bind_uniform_locations() override
  {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_font_map, "font_map");
  }

  void bind_attrib_locations() override
  {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "color");
  }
} prg;
}

//--------------------------------------------------------------------
cell::cell(uint32_t c, const vec4 color):
  m_character(c),
  m_color(color)
{

}

//--------------------------------------------------------------------
void cell::character(uint32_t v)
{
  m_character = v;
  m_reverse_horizontal = glm::linearRand(0.0f, 1.0f) < 0.5f;
  m_reverse_vertical = glm::linearRand(0.0f, 1.0f) < 0.5f;
}

//--------------------------------------------------------------------
rain::rain(matrix_rain* m, GLuint left):
  m_matrix_rain(m),
  m_pos(vec2(left, 0))
{
  reset();
}

//--------------------------------------------------------------------
void rain::update()
{
  // above window
  if(m_pos.y >= m_matrix_rain->height())
  {
    m_pos.y -= line_spacing();
    return;
  }

  for (auto& cell : m_cells) {
    if(glm::linearRand(0.0f, 1.0f) < 0.04f)
    {
      cell.character(m_matrix_rain->random_character());
    }
  }

  // growing
  if(m_pos.y > 0)
  {
    m_pos.y -= m_matrix_rain->horizontal_spacing();

    m_cells.front().color(vec4(0, 1, 0, 1));
    m_cells.emplace_front(m_matrix_rain->random_character(), vec4(1, 1, 1, 1));
    return;
  }

  // fading
  GLfloat alpha_step = 0.2;
  for (auto iter = m_cells.rbegin(); iter != m_cells.rend(); ++iter) {
    iter->add_alpha(-alpha_step);
    alpha_step *= 0.75;
  }

  m_cells.remove_if(
      [](const auto& cell)->bool
      {
      return cell.alpha() <= 0;
      });

  if(m_cells.empty())
    reset();
}

//--------------------------------------------------------------------
GLfloat rain::line_spacing()
{
  return m_matrix_rain->horizontal_spacing();
}

//--------------------------------------------------------------------
void rain::collect_mesh(rain_vertices& vertices)
{
  auto& printer = m_matrix_rain->printer();
  auto spacing = m_matrix_rain->horizontal_spacing();
  auto pos = m_pos;
  auto wnd_height = m_matrix_rain->height();
  for(auto& item : m_cells)
  {
    if(pos.y > wnd_height)
      break;

    if(pos.y >= 0)
    {
      auto& glyph = printer->get_glyph(item.character());

      auto s_min = item.reverse_horizontal() ? glyph.s_max : glyph.s_min;
      auto s_max = item.reverse_horizontal() ? glyph.s_min : glyph.s_max;
      auto t_min = item.reverse_vertical() ? glyph.t_max : glyph.t_min;
      auto t_max = item.reverse_vertical() ? glyph.t_min : glyph.t_max;

      rain_vertex v0 = {pos + vec2(glyph.x_min, glyph.y_min), vec2(s_min, t_min), item.color()};
      rain_vertex v1 = {pos + vec2(glyph.x_max, glyph.y_min), vec2(s_max, t_min), item.color()};
      rain_vertex v2 = {pos + vec2(glyph.x_max, glyph.y_max), vec2(s_max, t_max), item.color()};
      rain_vertex v3 = {pos + vec2(glyph.x_min, glyph.y_max), vec2(s_min, t_max), item.color()};

      vertices.push_back(v0);
      vertices.push_back(v1);
      vertices.push_back(v2);

      vertices.push_back(v0);
      vertices.push_back(v2);
      vertices.push_back(v3);

    }
    pos.y += spacing;
  }
}

//--------------------------------------------------------------------
void rain::reset()
{
  m_pos.y = glm::linearRand(0,  100) * m_matrix_rain->horizontal_spacing() + m_matrix_rain->height();
  m_cells.clear();
  m_cells.emplace_back(m_matrix_rain->random_character(), vec4(1, 1, 1, 1));
}

//--------------------------------------------------------------------
matrix_rain::matrix_rain(const std::shared_ptr<bitmap_text>& v, GLuint wnd_width, GLuint wnd_height,
int vertical_spacing/* = 0*/):
  m_printer(v),
  m_height(wnd_height)
{
  // arrange rains
  m_code_points = v->code_points();
  auto w = m_printer->max_advance() + vertical_spacing;
  auto num_rains = wnd_width / w;
  m_rains.reserve(num_rains);
  GLfloat left = (wnd_width - num_rains * m_printer->max_advance() - (num_rains - 1) * vertical_spacing) * 0.5f;
  for (int i = 0; i < num_rains; ++i) {
    m_rains.push_back(rain(this, left + i * w));
  }

  glGenVertexArrays(1, &m_vao);

  glGenBuffers(1, &m_vbo);

  glBindVertexArray(m_vao);
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(rain_vertex) * 10000 * 6, 0, GL_DYNAMIC_DRAW);

  auto offset = 0;
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(rain_vertex), BUFFER_OFFSET(offset));
  glEnableVertexAttribArray(0);

  offset += sizeof(vec4);
  glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(rain_vertex), BUFFER_OFFSET(offset));
  glEnableVertexAttribArray(1);


  prg.reshape(wnd_width, wnd_height);
}

//--------------------------------------------------------------------
void matrix_rain::update()
{
  for (auto& item : m_rains) {
    item.update();
  }
}

//--------------------------------------------------------------------
void matrix_rain::draw()
{
  rain_vertices vertices;
  vertices.reserve(10000*6);
  for (auto& item : m_rains) {
    item.collect_mesh(vertices);
  }
  glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(rain_vertex) * vertices.size(), &vertices.front());

  glBindTexture(GL_TEXTURE_2D, m_printer->texture());
  prg.use();
  prg.update_uniforms(0);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBindVertexArray(m_vao);
  glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}


//--------------------------------------------------------------------
GLfloat matrix_rain::horizontal_spacing() const
{
  return m_printer->linespace();
}

//--------------------------------------------------------------------
uint32_t matrix_rain::random_character() const
{
  return m_code_points[glm::linearRand(0.0f, 0.999999f) * m_code_points.size()];
}
}
