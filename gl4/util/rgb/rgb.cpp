#include "app.h"
#include "bitmaptext.h"
#include <sstream>
#include "common.h"
#include <iomanip>
#include <fstream>
#include <algorithm>
#include <stdexcept>
using namespace glm;

namespace zxd {

// clang-format off
// predefined color
vec3_vector start_colors = vec3_vector( {
  {0.0  , 0.0  , 0.0  },
  {1.0  , 1.0  , 1.0  },
  {1.0  , 0.0  , 0.0  },
  {0.0  , 1.0  , 0.0  },
  {0.0  , 0.0  , 1.0  },
  {1.0  , 1.0  , 0.0  },
  {0.0  , 1.0  , 1.0  },
  {1.0  , 0.0  , 1.0  },
  {0.753, 0.753, 0.753},
  {0.502, 0.502, 0.502},
  {0.502, 0.0  , 0.0  },
  {0.502, 0.502, 0.0  },
  {0.0  , 0.502, 0.0  },
  {0.502, 0.0  , 0.502},
  {0.0  , 0.502, 0.502},
  {0.0  , 0.0  , 0.502},
  {1.0  , 1.0  , 1.0  }
});
// clang-format on

std::string input_modes[] = {"float", "byte", "hex", "normal"};

vec3 str_to_color(const std::string &s, GLint mode) {
  vec3 color(1);

  if (mode == 2) {  // read hex color
    std::string str(s);
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    std::stringstream ss(str);

    GLint value;
    if (ss >> std::hex >> value) {
      // get the least significant 3 bytes as b g r
      GLint i = 2;
      do {
        color[i--] = (value & 0xff) / 255.0f;
        value >>= 8;
      } while (i >= 0);
    } else {
      std::cerr << "failed to read " << input_modes[mode] << ss.str()
                << std::endl;
    }
  } else {  // others
    std::stringstream ss(s);
    if (ss >> color[0] && ss >> color[1] && ss >> color[2]) {
      if (mode == 0) {
      } else if (mode == 1) {
        color /= 255.0f;
      } else if (mode == 2) {
        color /= 255.0f;
      } else if (mode == 3) {
        color = glm::normalize(color);
        color = color * 0.5f + 0.5f;
      }
    } else {
      std::cerr << "failed to read " << ss.str() << std::endl;
    }
  }

  return color;
};

void read_start_colors(const std::string &filename) {
  // read data from file
  start_colors.clear();

  std::ifstream ifs(filename);
  if (!ifs) {
    std::stringstream ss;
    ss << "failed to open file " << filename << std::endl;
    throw std::runtime_error(ss.str());
  }

  GLint stage = 0;
  std::string line;
  GLint input_mode = 0;
  while (std::getline(ifs, line)) {
    if (line.empty() || line[0] == '#') {
      continue;
    }

    if (stage == 0) {
      // read data mode
      try {
        input_mode = std::stoi(line);
      } catch (const std::invalid_argument &e) {
        std::cout << e.what() << std::endl;
        std::cout << "failed to get input mode(0 or 1 or 2 or 3)" << std::endl;
        break;
      }
      ++stage;
      continue;
    }

    // read data line by line
    start_colors.push_back(str_to_color(line, input_mode));
  }

  std::cout << "read " << start_colors.size() << " " << input_modes[input_mode]
            << " colors from " << filename << std::endl;
}

class color_pane {
public:
  color_pane() : m_color(1, 1, 1){};
  color_pane(GLfloat r, GLfloat g, GLfloat b) : m_color(r, g, b){};
  color_pane(vec3 v) : m_color(v){};
  void add_red(GLfloat f) { m_color.r = glm::clamp(m_color.r + f, 0.0f, 1.0f); }
  void add_green(GLfloat f) {
    m_color.g = glm::clamp(m_color.g + f, 0.0f, 1.0f);
  }
  void add_blue(GLfloat f) {
    m_color.b = glm::clamp(m_color.b + f, 0.0f, 1.0f);
  }
  GLfloat red() { return m_color.r; }
  GLfloat green() { return m_color.g; };
  GLfloat blue() { return m_color.b; };

  GLfloat x() const { return m_x; }
  void x(GLfloat v) { m_x = v; }

  GLfloat y() const { return m_y; }
  void y(GLfloat v) { m_y = v; }

  const vec3 &color() const { return m_color; }
  void set_color(const vec3 &v) { m_color = v; }

protected:
  vec3 m_color;
  GLfloat m_x;
  GLfloat m_y;
};

typedef std::vector<color_pane> color_pane_vector;

struct rgb_program : public zxd::program {
  GLint ul_gamma;

  void update_uniforms(GLboolean gamma = 0) {
    glUniformMatrix4fv(ul_mvp_mat, 1, 0, value_ptr(mvp_mat));
    glUniform1i(ul_gamma, gamma);
  }

  virtual void attach_shaders() {
    attach(GL_VERTEX_SHADER, "data/shader/rgb.vs.glsl");
    attach(GL_FRAGMENT_SHADER, "data/shader/rgb.fs.glsl");
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_mvp_mat, "mvp_mat");
    uniform_location(&ul_gamma, "gamma");
  }

  virtual void bind_attrib_locations() {
    bind_attrib_location(0, "vertex");
    bind_attrib_location(1, "color");
  };
} rgb_program;

class rgb_app : public app {
protected:
  bitmap_text m_text;
  GLuint m_max_pane_per_line;
  GLuint m_current_pane_index;
  GLfloat m_pane_width;
  GLfloat m_pane_height;

  GLboolean m_gamma;
  GLboolean m_help;
  GLuint m_pane_vao;
  GLuint m_border_vao;
  GLuint m_pane_vertex_buffer;
  GLuint m_pane_color_buffer;
  GLuint m_border_vertex_buffer;
  GLuint m_border_color_buffer;
  GLfloat m_border_width;
  GLuint m_input_mode;

  color_pane_vector m_panes;

  vec2_vector m_pane_vertices;
  vec3_vector m_pane_colors;

  vec2_vector m_border_vertices;
  vec3_vector m_border_colors;

public:
  rgb_app() {}

protected:
  virtual void init_info() {
    app::init_info();
    m_info.title = "hello world";
    m_info.wnd_width = 1024;
    m_info.wnd_height = 512;
    m_info.samples = 4;
  }

  GLuint num_rows() {
    return glm::ceil(
      m_panes.size() / static_cast<GLfloat>(m_max_pane_per_line));
  }
  GLuint num_cols() {
    return m_panes.size() >= m_max_pane_per_line ? m_max_pane_per_line
                                                 : m_panes.size();
  }
  GLuint current_row() { return m_current_pane_index / m_max_pane_per_line; }
  GLuint current_col() {
    return m_panes.size() >= m_max_pane_per_line
             ? m_current_pane_index % m_max_pane_per_line
             : m_current_pane_index;
  }

  virtual void create_scene() {
    m_max_pane_per_line = 8;
    m_border_width = 20.0f;
    m_gamma = GL_FALSE;
    m_help = GL_TRUE;
    m_reading = GL_FALSE;
    m_input_mode = 0;

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_LINE_SMOOTH);
    m_text.init();
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);

    rgb_program.init();
    rgb_program.p_mat = ortho(0.0f, 1.0f, 0.0f, 1.0f);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glGenVertexArrays(1, &m_pane_vao);
    glGenVertexArrays(1, &m_border_vao);

    for (GLuint i = 0; i < start_colors.size(); ++i) {
      color_pane pane(start_colors[i]);
      m_panes.push_back(pane);

      for (int j = 0; j < 6; ++j) {
        m_pane_colors.push_back(start_colors[i]);
      }
    }

    m_current_pane_index = m_panes.size() - 1;
    for (int i = 0; i < 6; ++i) m_border_colors.push_back(vec3(0.0));

    glBindVertexArray(m_border_vao);

    glGenBuffers(1, &m_border_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_border_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(vec2), 0, GL_DYNAMIC_DRAW);
    glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &m_border_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_border_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_border_colors.size() * sizeof(vec3),
      value_ptr(m_border_colors.front()), GL_DYNAMIC_DRAW);  // will be updated
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);

    resize_pane();
    reset_pane_vertex_buffer();
    reset_pane_color_buffer();
    update_border_vertex_buffer();
  }

  color_pane &current_pane() { return m_panes[m_current_pane_index]; }

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(rgb_program);
    glViewport(
      0, m_info.wnd_height * 0.1, m_info.wnd_width, m_info.wnd_height * 0.9);

    // render pane
    glBindVertexArray(m_pane_vao);
    rgb_program.mvp_mat = rgb_program.p_mat;
    rgb_program.update_uniforms(m_gamma);
    glDrawArrays(GL_TRIANGLES, 0, m_pane_vertices.size());

    // render border on current selected color pane
    glLineWidth(m_border_width);
    glBindVertexArray(m_border_vao);
    const color_pane &pane = current_pane();
    rgb_program.mvp_mat =
      rgb_program.p_mat * translate(vec3(pane.x(), pane.y(), 0));
    rgb_program.update_uniforms(m_gamma);
    glDrawArrays(GL_LINE_LOOP, 0, 4);
    glLineWidth(1);

    glViewport(0, 0, m_info.wnd_width, m_info.wnd_height);
    // render text
    // for (int i = 0; i < m_panes.size(); ++i) {
    // color_pane &pane = m_panes[i];
    // const vec3 &color = pane.color();
    // std::stringstream ss;
    // ss.precision(3);
    // ss << color.r << " " << color.g << " " << color.b;
    // glEnable(GL_BLEND);
    // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // m_text.print(ss.str(), pane.x() * m_info.wnd_width + 5,
    // pane.y() * m_info.wnd_height + 8, vec4(0, 0, 0, 1));
    // glDisable(GL_BLEND);
    //}
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::stringstream ss;
    if (m_reading) {
      ss << "input " << input_modes[m_input_mode] << " color : " << m_input;
    } else {
      // clang-format off
      const vec3 &color = pane.color();
      ss.precision(3);
      ss.setf(std::ios_base::fixed);
      ss << color.r << " " << color.g << " " << color.b;
      ss << "          ";
      ss << std::setw(3) << static_cast<GLint>(color.r * 255) << " "
         << std::setw(3) << static_cast<GLint>(color.g * 255) << " "
         << std::setw(3) << static_cast<GLint>(color.b * 255) << " ";
      ss << "          ";
      ss << std::setw(2) << std::setfill('0') << std::hex << static_cast<GLint>(color.r * 255) << " "
         << std::setw(2) << std::setfill('0') << std::hex << static_cast<GLint>(color.g * 255) << " "
         << std::setw(2) << std::setfill('0') << std::hex << static_cast<GLint>(color.b * 255)
         << " ";
      ss << "          ";
      ss << color.r * 2 - 1 << " " << color.g * 2 - 1 << " " << color.b * 2 - 1;
      // clang-format on
    }
    m_text.print(ss.str(), 5, 10, vec4(1));

    if (m_help) {
      std::stringstream ss;
      ss << " h : toggle help" << std::endl;
      ss << " esc : exit" << std::endl;
      ss << " a : add new color pane" << std::endl;
      ss << " d : delete color pane" << std::endl;
      ss << " arrow : select color pane" << std::endl;
      ss << " g : gamma correction : " << (m_gamma == GL_FALSE ? 0 : 1)
         << std::endl;
      ss << " i : input mode : " << input_modes[m_input_mode] << std::endl;
      ss << " enter : start or finish input color" << std::endl;
      ss << " [ctrl|alt]qQ : red" << std::endl;
      ss << " [ctrl|alt]wW : green" << std::endl;
      ss << " [ctrl|alt]eE : blue" << std::endl;
      m_text.print(ss.str(), 1, m_info.wnd_height - 25, vec4(0, 0, 1, 1));
    }
    glDisable(GL_BLEND);
  }

  virtual void update() {}

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(w, h);
  }

  GLfloat quantity(int mods) {
    GLfloat quantity = 0.01;
    if (mods & GLFW_MOD_CONTROL) quantity = 0.001;
    // if (mods & GLFW_MOD_ALT) quantity = 0.001;
    if (mods & GLFW_MOD_SHIFT) quantity *= -1;
    return quantity;
  }

  void finishe_reading() {
    app::finishe_reading();
    std::stringstream ss(m_input);

    color_pane &pane = current_pane();
    vec3 color = str_to_color(m_input, m_input_mode);

    pane.set_color(glm::clamp(color, vec3(0), vec3(1)));
    update_pane_color_buffer();
  }

  virtual void glfw_key(
    GLFWwindow *wnd, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS) {
      switch (key) {
        case GLFW_KEY_ENTER:
        case GLFW_KEY_KP_ENTER:
          if (m_reading) {
            finishe_reading();
          } else {
            start_reading();
          }
          break;
        default:
          break;
      }
    }

    if (!m_reading) {
      if (action == GLFW_PRESS) {
        switch (key) {
          case GLFW_KEY_UP:
            up();
            break;
          case GLFW_KEY_DOWN:
            down();
            break;
          case GLFW_KEY_LEFT:
            left();
            break;
          case GLFW_KEY_RIGHT:
            right();
            break;
          case GLFW_KEY_A:
            add_pane();
            break;
          case GLFW_KEY_D:
            delete_current_pane();
            break;
          case GLFW_KEY_G:
            m_gamma = !m_gamma;
            break;
          case GLFW_KEY_H:
            m_help = !m_help;
            break;
          case GLFW_KEY_I:
            m_input_mode = (m_input_mode + 1) %
                           (sizeof(input_modes) / sizeof(input_modes[0]));
            break;

          default:
            break;
        }
      }

      if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        switch (key) {
          case GLFW_KEY_Q:
            current_pane().add_red(quantity(mods));
            update_pane_color_buffer();
            break;
          case GLFW_KEY_W:
            current_pane().add_green(quantity(mods));
            update_pane_color_buffer();
            break;
          case GLFW_KEY_E:
            current_pane().add_blue(quantity(mods));
            update_pane_color_buffer();
            break;
          default:
            break;
        }
      }
    }
    app::glfw_key(wnd, key, scancode, action, mods);
  }

  virtual void glfw_mouse_button(
    GLFWwindow *wnd, int button, int action, int mods) {
    app::glfw_mouse_button(wnd, button, action, mods);
  }

  virtual void glfw_mouse_move(GLFWwindow *wnd, double x, double y) {
    app::glfw_mouse_move(wnd, x, y);
  }

  virtual void glfw_mouse_wheel(
    GLFWwindow *wnd, double xoffset, double yoffset) {
    app::glfw_mouse_wheel(wnd, xoffset, yoffset);
  }

  // select panes
  void left() {
    if (m_current_pane_index % num_cols() != 0) --m_current_pane_index;
  }
  void right() {
    if (m_current_pane_index % num_cols() != num_cols() - 1 &&
        m_current_pane_index < m_panes.size() - 1)
      ++m_current_pane_index;
  }
  void down() {
    if (m_current_pane_index >= num_cols()) m_current_pane_index -= num_cols();
  }
  void up() {
    if (m_current_pane_index + num_cols() < m_panes.size())
      m_current_pane_index += num_cols();
  }
  // void select_pane(GLuint index);
  void add_pane() {
    GLuint row = num_rows() - 1;
    GLuint col = m_panes.size() % num_cols();
    GLboolean need_resize_pane = 0;

    if (m_max_pane_per_line > num_cols()) {
      need_resize_pane = 1;
    } else if (col == 0) {
      ++row;
      need_resize_pane = 1;
    }

    vec3 color(glm::linearRand(vec3(0.0), vec3(1.0)));
    for (int i = 0; i < 6; ++i) m_pane_colors.push_back(color);

    // add new color pane
    color_pane pane;
    GLfloat x = col * m_pane_width;
    GLfloat y = row * m_pane_height;
    pane.set_color(color);
    pane.x(x);
    pane.y(y);
    m_panes.push_back(pane);

    if (need_resize_pane) {
      resize_pane();
      update_border_vertex_buffer();
    } else {
      m_pane_vertices.push_back(vec2(x, y));
      m_pane_vertices.push_back(vec2(x + m_pane_width, y));
      m_pane_vertices.push_back(vec2(x + m_pane_width, y + m_pane_height));
      m_pane_vertices.push_back(vec2(x, y));
      m_pane_vertices.push_back(vec2(x + m_pane_width, y + m_pane_height));
      m_pane_vertices.push_back(vec2(x, y + m_pane_height));
    }
    reset_pane_vertex_buffer();
    reset_pane_color_buffer();

    m_current_pane_index = m_panes.size() - 1;
  }
  void delete_current_pane() {
    if (m_panes.size() == 1) return;

    m_panes.erase(m_panes.begin() + m_current_pane_index);
    auto iter0 = m_pane_vertices.begin() + m_current_pane_index * 6;
    m_pane_vertices.erase(iter0, iter0 + 6);

    auto iter1 = m_pane_colors.begin() + m_current_pane_index * 6;
    m_pane_colors.erase(iter1, iter1 + 6);

    resize_pane();
    update_border_vertex_buffer();
    reset_pane_vertex_buffer();
    reset_pane_color_buffer();
    if (m_current_pane_index == m_panes.size()) {
      m_current_pane_index = m_panes.size() - 1;
    }
  }

  void resize_pane() {
    GLuint col = 0;
    GLuint row = 0;

    m_pane_width = 1.0f / num_cols();
    m_pane_height = 1.0f / num_rows();

    for (GLuint i = 0; i < m_panes.size(); ++i) {
      if (col == m_max_pane_per_line) {
        col = 0;
        ++row;
      }

      color_pane &pane = m_panes[i];
      pane.x(m_pane_width * col);
      pane.y(m_pane_height * row);
      ++col;
    }

    m_pane_vertices.clear();
    // build pane mesh
    for (int i = 0; i < m_panes.size(); ++i) {
      color_pane &pane = m_panes[i];
      GLfloat x = pane.x();
      GLfloat y = pane.y();
      // std::cout << x << " : " << y << std::endl;
      m_pane_vertices.push_back(vec2(x, y));
      m_pane_vertices.push_back(vec2(x + m_pane_width, y));
      m_pane_vertices.push_back(vec2(x + m_pane_width, y + m_pane_height));
      m_pane_vertices.push_back(vec2(x, y));
      m_pane_vertices.push_back(vec2(x + m_pane_width, y + m_pane_height));
      m_pane_vertices.push_back(vec2(x, y + m_pane_height));
    }

    // build border mesh
    m_border_vertices.clear();
    m_border_vertices.push_back(vec2(0, 0));
    m_border_vertices.push_back(vec2(0 + m_pane_width, 0));
    m_border_vertices.push_back(vec2(0 + m_pane_width, 0 + m_pane_height));
    m_border_vertices.push_back(vec2(0, 0 + m_pane_height));
  }

  void reset_pane_vertex_buffer() {
    glBindVertexArray(m_pane_vao);

    if (glIsBuffer(m_pane_vertex_buffer))
      glDeleteBuffers(1, &m_pane_vertex_buffer);

    glGenBuffers(1, &m_pane_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_pane_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_pane_vertices.size() * sizeof(vec2),
      value_ptr(m_pane_vertices.front()), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    glEnableVertexAttribArray(0);
  }

  void reset_pane_color_buffer() {
    glBindVertexArray(m_pane_vao);

    if (glIsBuffer(m_pane_color_buffer))
      glDeleteBuffers(1, &m_pane_color_buffer);
    glGenBuffers(1, &m_pane_color_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, m_pane_color_buffer);
    glBufferData(GL_ARRAY_BUFFER, m_pane_colors.size() * sizeof(vec3),
      value_ptr(m_pane_colors.front()), GL_DYNAMIC_DRAW);  // will be updated
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(1);
  }

  void update_border_vertex_buffer() {
    glBindBuffer(GL_ARRAY_BUFFER, m_border_vertex_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vec2) * m_border_vertices.size(),
      value_ptr(m_border_vertices.front()));
  }

  void update_pane_color_buffer() {
    color_pane &pane = current_pane();
    for (int i = 0; i < 6; ++i) {
      m_pane_colors[m_current_pane_index * 6 + i] = pane.color();
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_pane_color_buffer);
    glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec3) * 6 * m_current_pane_index,
      sizeof(vec3) * 6, value_ptr(m_pane_colors[m_current_pane_index * 6]));
  }
};
}

int main(int argc, char *argv[]) {
  if (argc >= 2) {
    zxd::read_start_colors(argv[1]);
  }

  zxd::rgb_app app;
  app.run();
}
