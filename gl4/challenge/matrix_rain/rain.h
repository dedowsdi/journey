#ifndef GL4_MATRIX_RAIN_RAIN_CPP
#define GL4_MATRIX_RAIN_RAIN_CPP

#include <vector>
#include <list>
#include <memory>
#include "glm.h"

namespace zxd
{

struct rain_vertex
{
  vec2 pos;
  vec2 texcoord;
  vec4 color;
};

using rain_vertices = std::vector<rain_vertex>;

class bitmap_text;
class cell
{
public:
  cell(uint32_t c, const vec4 color);

  uint32_t character() const { return m_character; }
  void character(uint32_t v);

  void add_alpha(GLfloat v){ m_color.w += v; }

  const vec4& color() const { return m_color; }
  void color(const vec4& v){ m_color = v; }

  GLfloat alpha() const {return m_color.w;}

  bool reverse_horizontal() const { return m_reverse_horizontal; }
  void reverse_horizontal(bool v){ m_reverse_horizontal = v; }

  bool reverse_vertical() const { return m_reverse_vertical; }
  void reverse_vertical(bool v){ m_reverse_vertical = v; }

private:
  bool m_reverse_horizontal;
  bool m_reverse_vertical;
  GLfloat m_alpha;
  uint32_t m_character;
  vec4 m_color;
};

using cells = std::list<cell>;

class matrix_rain;

class rain
{
public:
  rain(matrix_rain* m, GLuint left);

  void update();

  const vec2& pos() const { return m_pos; }
  void pos(const vec2& v){ m_pos = v; }
  GLfloat line_spacing();
  void collect_mesh(rain_vertices& vertices);

private:
  void reset();

  matrix_rain* m_matrix_rain;
  vec2 m_pos;
  cells m_cells; // front as head, back as tail
};

using rains = std::vector<rain>;

class matrix_rain
{
public:
  // use all characters in bitmap_text
  matrix_rain(const std::shared_ptr<bitmap_text>& v, GLuint wnd_width, GLuint wnd_height,
      int vertical_spacing = 0);
  void update();
  void draw();

  GLuint width() const { return m_width; }
  GLuint height() const { return m_height; }
  GLfloat horizontal_spacing() const;
  uint32_t random_character() const;

  const std::shared_ptr<bitmap_text>& printer() const { return m_printer; }
  void printer(const std::shared_ptr<bitmap_text>& v){ m_printer = v; }

private:
  GLuint m_width;
  GLuint m_height;
  GLuint m_vao;
  GLuint m_vbo;
  rains m_rains;
  std::shared_ptr<bitmap_text> m_printer;
  std::vector<uint32_t> m_code_points;
};

}

#endif /* GL4_MATRIX_RAIN_RAIN_CPP */
