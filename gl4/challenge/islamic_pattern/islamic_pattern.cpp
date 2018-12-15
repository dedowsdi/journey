#include "app.h"
#include "bitmaptext.h"
#include "common_program.h"
#include <sstream>

#define WIDTH 800
#define HEIGHT 800
#define POLYGON_WIDTH 200
#define BUFFER_OFFSET(x) ((GLubyte*)NULL + (x))

namespace zxd {

lightless_program prg;

vec4 corner_color = vec4(1,1,1,1);
vec4 x_color = vec4(1,0,0,1);
vec4 inner_color = vec4(0,0,1,1);

kcip kci_theta;
kcip kci_delta;
kcip kci_sides;

// always be right
class polygon
{
protected:
  GLuint m_vao = -1;
  GLuint m_vbo = -1;
  GLuint m_sides = 6;
  GLfloat m_start_angle = 0; // points will be created in cc order
  GLfloat m_theta = fpi / 3; // pi / 3
  GLfloat m_delta = 0; // normalized
  GLfloat m_radius = 1;
  vec2_vector m_vertices;

  vec4 m_corner_color;
  vec4 m_inner_color;
  vec4 m_x_color;

  GLuint m_num_corner_vertex = 0;
  GLuint m_num_inner_vertex = 0;
  GLuint m_num_x_vertex = 0;

public:

  void build_mesh()
  {
    m_vertices.clear();

    m_num_corner_vertex = m_sides;
    m_num_inner_vertex = 0;
    m_num_x_vertex = 0;
  
    GLfloat step_angle = f2pi / m_sides;
    //GLfloat c = cos(m_theta);
    GLfloat s = sin(m_theta);
    GLfloat t = tan(m_theta);

    // create corner. (polygon)
    for (int i = 0; i < m_sides; ++i) 
    {
      GLfloat angle = m_start_angle + step_angle * i;
      m_vertices.push_back(vec2(cos(angle), sin(angle)) * m_radius);
    }

    // create x
    if(m_theta < fpi2 && m_delta != 0)
    {
      m_num_x_vertex = 3 * m_sides;
      for (int i = 0; i < m_sides; ++i) {
        const vec2& point0 = m_vertices[i];
        const vec2& point1 = i == m_sides - 1 ? m_vertices[0] : m_vertices[i + 1];

        vec2 middle_point = (point0 + point1) * 0.5f;
        
        vec2 xpoint0 = glm::mix(middle_point, point0, m_delta); // left or bottom
        vec2 xpoint1 = glm::mix(middle_point, point1, m_delta); // right or top
        vec2 mp2p1 = point1 - middle_point;
        vec2 mp2p2 = glm::normalize(vec2(-mp2p1.y, mp2p1.x));
        vec2 xpoint2 = middle_point + mp2p2 * glm::length(xpoint1 - middle_point) * t;
        
        m_vertices.push_back(xpoint0);
        m_vertices.push_back(xpoint1);
        m_vertices.push_back(xpoint2);
        //std::cout << xpoint0 << std::endl;
        //std::cout << xpoint1 << std::endl;
        //std::cout << xpoint2 << std::endl;
      }
    }

    // create interior
    if(m_theta < fpi2)
    {
      m_num_inner_vertex = 2*m_sides + 2; // 2 for cener and close point
      m_vertices.push_back(vec2(0));

      for (int i = 0; i < m_sides; ++i) {
        // need to find two inner points for every side
        // 1. xpoint2 or middle_point
        // 2. intersection point between the 1st ray of edge1 and the 2nd ray of
        //    edge2
        const vec2& point0 = m_vertices[i];
        const vec2& point1 = m_vertices[(i+1)%m_num_corner_vertex];

        vec2 ipoint0 = m_delta != 0 ? m_vertices[m_sides + i * 3 + 2] : (point0 + point1) * 0.5f;
        vec2 p12c = glm::normalize(-point1);
        // use law of sine to find lenth of point1 and intersectoin point
        vec2 ip02p1 = point1 - ipoint0;
        GLfloat l = s * glm::length(ip02p1) / glm::sin(fpi - m_theta - angle() * 0.5f);
        vec2 ipoint1 = point1 + l * p12c;

        m_vertices.push_back(ipoint0);
        m_vertices.push_back(ipoint1);
      }

      // close fan
      m_vertices.push_back(m_vertices.at(m_num_x_vertex + m_num_corner_vertex + 1));

    }
    

    if(m_vao == -1)
    {
      glGenVertexArrays(1, &m_vao);
      glGenBuffers(1, &m_vbo);
    }

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(vec2), glm::value_ptr(m_vertices.front()), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray(0);
  }

  GLfloat angle()
  {
    return fpi - f2pi / m_sides;
  }

  void draw()
  {
    draw_corner();
    draw_x();
    draw_inner();
  }

  void draw_corner()
  {
    glBindVertexArray(m_vao);
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(m_corner_color));
    glDrawArrays(GL_TRIANGLE_FAN, 0, m_num_corner_vertex);
  }
  void draw_inner()
  {
    if(m_num_inner_vertex <= 0)
      return;

    glBindVertexArray(m_vao);
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(m_inner_color));
    glDrawArrays(GL_TRIANGLE_FAN, m_num_corner_vertex + m_num_x_vertex, m_num_inner_vertex);
  }

  void draw_x()
  {
    if(m_num_x_vertex <= 0)
      return;

    glBindVertexArray(m_vao);
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(m_x_color));
    glDrawArrays(GL_TRIANGLES, m_num_corner_vertex, m_num_x_vertex);
  }

  GLuint sides() const { return m_sides; }
  void sides(GLuint v){ m_sides = v; }

  GLfloat start_angle() const { return m_start_angle; }
  void start_angle(GLfloat v){ m_start_angle = v; }

  GLfloat radius() const { return m_radius; }
  void radius(GLfloat v){ m_radius = v; }

  GLfloat theta() const { return m_theta; }
  void theta(GLfloat v){ m_theta = v; }

  GLfloat delta() const { return m_delta; }
  void delta(GLfloat v){ m_delta = v; }

  const vec4& corner_color() const { return m_corner_color; }
  void corner_color(const vec4& v){ m_corner_color = v; }

  const vec4& inner_color() const { return m_inner_color; }
  void inner_color(const vec4& v){ m_inner_color = v; }

  const vec4& x_color() const { return m_x_color; }
  void x_color(const vec4& v){ m_x_color = v; }
};


class islamic_pattern_app : public app {
protected:
  bitmap_text m_text;
  polygon m_polygon;

public:
  virtual void init_info() {
    app::init_info();
    m_info.title = "islamic_pattern_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
  }

  virtual void create_scene() {
    glClearColor(0.0f, 0.5f, 1.0f, 1.0f);

    m_text.init();
    m_text.reshape(wnd_width(), wnd_height());

    prg.init();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    m_polygon.radius(POLYGON_WIDTH / glm::sqrt(2));
    m_polygon.corner_color(corner_color);
    m_polygon.x_color(x_color);
    m_polygon.inner_color(inner_color);
    m_polygon.delta(0.25);

    auto callback = std::bind(std::mem_fn(&islamic_pattern_app::update_polygon), this, std::placeholders::_1);
    kci_theta = m_control.add_control<GLfloat>(GLFW_KEY_Q, 60, 0, 90, 1, callback);
    kci_delta = m_control.add_control<GLfloat>(GLFW_KEY_W, 0, 0, 1, 0.01, callback);
    kci_sides = m_control.add_control(GLFW_KEY_E, 4, 4, 6, 2, callback);

    update_polygon(0);
  }

  void update_polygon(const kci* kci)
  {
    m_polygon.theta(glm::radians(kci_theta->get_float()));
    m_polygon.delta(kci_delta->get_float());
    m_polygon.sides(kci_sides->get_int());
    if(m_polygon.sides() == 4)
    {
      m_polygon.radius(POLYGON_WIDTH / glm::sqrt(2));
      m_polygon.start_angle(fpi4);
    }
    else
    {
      m_polygon.radius(POLYGON_WIDTH * 0.5f);
      m_polygon.start_angle(0);
    }

    m_polygon.build_mesh();
  }

  virtual void update() {}

  virtual void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    prg.fix2d_camera(0, WIDTH, 0, HEIGHT);

    if(m_polygon.sides() == 4)
    {
      // 4x4
      for (int y = 0; y < 4; ++y) 
      {
        for (int x = 0; x < 4; ++x) 
        {
          mat4 mvp_mat = prg.vp_mat * glm::translate(vec3(POLYGON_WIDTH * x, POLYGON_WIDTH * y, 0) + vec3(0.5f * POLYGON_WIDTH, 0.5f * POLYGON_WIDTH, 0));
          glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
          m_polygon.draw();
        }
      }
    }
    else if(m_polygon.sides() == 6)
    {
      GLfloat r = m_polygon.radius();
      GLfloat h = r * glm::sqrt(3);
      vec3 start = vec3(-0.75f * POLYGON_WIDTH, -0.5f * h, 0) + vec3(0.5f*POLYGON_WIDTH, 0.5f * h, 0);
      for (int y = 0; y < 6; ++y) 
      {
        for (int x = 0; x < 7; ++x) 
        {
          vec3 translation = vec3(r * 1.5 * x, h * y + (x&1 ? h * 0.5 : 0) , 0) + start ;
          mat4 mvp_mat = prg.vp_mat * glm::translate(translation);
          glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
          m_polygon.draw();
        }
      }

    }

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "q : theta " << kci_theta->get_float() << std::endl;
    ss << "w : delta " << kci_delta->get_float() << std::endl;
    ss << "e : sides " << kci_sides->get_int() << std::endl;
    m_text.print(ss.str(), 10, m_info.wnd_height - 20, vec4(1,0,0,1));
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h) {
    app::glfw_resize(wnd, w, h);
    m_text.reshape(m_info.wnd_width, m_info.wnd_height);
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
  zxd::islamic_pattern_app app;
  app.run();
}
