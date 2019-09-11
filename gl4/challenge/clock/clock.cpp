#include <app.h>
#include <bitmap_text.h>
#include <sstream>
#include <common_program.h>
#include <capsule2d.h>
#include <chrono>
#include <circle.h>

#define WIDTH 800
#define HEIGHT 800

namespace zxd
{

lightless_program prg;
capsule2d arm_geometry;
circle circle_geometry;

glm::mat4 v_mat;
glm::mat4 p_mat;
glm::mat4 vp_mat;

class clock
{
  GLfloat m_size = 1;
  GLfloat m_hour;
  GLfloat m_minute;
  GLfloat m_second;
  GLfloat m_milisecond;

  vec4 m_circle_color = vec4(1,0,1,1);
  vec4 m_hour_color = vec4(1,0,0,1);
  vec4 m_minute_color = vec4(0,1,0,1);
  vec4 m_second_color = vec4(0,0,1,1);
  vec4 m_diamond_color = vec4(0,1,1,1);

  capsule2d m_hour_arm;
  capsule2d m_minute_arm;
  capsule2d m_second_arm;
  circle m_diamond;
  circle m_circle;

public:

  GLfloat hour() const { return m_hour; }
  void hour(GLfloat v){ m_hour = v; }
  GLfloat hour12() const {return m_hour <= 12 ? m_hour : m_hour - 12;}

  GLfloat minute() const { return m_minute; }
  void minute(GLfloat v){ m_minute = v; }

  GLfloat second() const { return m_second; }
  void second(GLfloat v){ m_second = v; }

  GLfloat milisecond() const { return m_milisecond; }
  void milisecond(GLfloat v){ m_milisecond = v; }

  GLint ihour() const{return static_cast<int>(m_hour);}
  GLint iminute() const{return static_cast<int>(m_minute);}
  GLint isecond() const{return static_cast<int>(m_second);}

  GLfloat size() const { return m_size; }
  void size(GLfloat v){ m_size = v; }

  void as_since_midnight()
  {
    std::time_t t = std::time(0);
    std::tm* tmt = std::localtime(&t);
    tmt->tm_hour = tmt->tm_min = tmt->tm_sec = 0;
    auto d = std::chrono::system_clock::now() - std::chrono::system_clock::from_time_t(std::mktime(tmt));
    std::chrono::duration<double, std::ratio<1, 1>> s(d);

    m_hour = s.count() / 3600;
    m_minute = (s.count() - ihour() * 3600) / 60;
    m_second = s.count() - ihour() * 3600 - iminute() * 60;
    m_milisecond = m_second - static_cast<int>(m_second);
  }

  void draw()
  {
    if(!m_hour_arm.is_inited())
      create_meshes();

    draw_circle();
    for (int i = 0; i < 12; ++i)
    {
      draw_diamond(fpi2 - i * f2pi / 12);
    }


    glEnable(GL_BLEND);
    glBlendColor(0,0,0,0.5);
    glBlendFunc(GL_CONSTANT_ALPHA, GL_ONE_MINUS_CONSTANT_ALPHA);
    draw_arm(m_hour_arm, fpi2 - f2pi*hour12()/12, m_hour_color);
    draw_arm(m_minute_arm, fpi2 - f2pi*minute()/60, m_minute_color);
    draw_arm(m_second_arm, fpi2 - f2pi*second()/60, m_second_color);
    glDisable(GL_BLEND);
  }

protected:

  void draw_arm(capsule2d& arm, GLfloat angle, const vec4& color)
  {
    mat4 mvp_mat = vp_mat * glm::rotate(angle, pza) 
      * glm::translate(vec3(arm.width() * 0.4, 0,0));
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(color));
    arm.draw();
  }

  void draw_circle()
  {
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(vp_mat));
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(m_circle_color));
    glLineWidth(5);
    m_circle.draw();
  }

  void draw_diamond(GLfloat angle)
  {
    mat4 mvp_mat = vp_mat * glm::rotate(angle, pza)
      * glm::translate(vec3(m_size * 0.45, 0,0));
    glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    glUniform4fv(prg.ul_color, 1, glm::value_ptr(m_diamond_color));
    m_diamond.draw();
  }

  void create_meshes()
  {
    m_hour_arm.radius(m_size * 0.02);
    m_hour_arm.width(m_size * 0.5 * 0.6);
    m_hour_arm.build_mesh();

    m_minute_arm.radius(m_size * 0.016);
    m_minute_arm.width(m_size * 0.5 * 0.75);
    m_minute_arm.build_mesh();

    m_second_arm.radius(m_size * 0.012);
    m_second_arm.width(m_size * 0.5 * 0.9);
    m_second_arm.build_mesh();

    m_diamond.radius(m_size * 0.012);
    m_diamond.build_mesh();

    m_circle.radius(m_size * 0.5 * 0.98);
    m_circle.type(circle::LINE);
    m_circle.slice(128);
    m_circle.build_mesh();
  }

};

class clock_app : public app
{
protected:
  clock m_clock;

public:
  virtual void init_info()
  {
    app::init_info();
    m_info.title = "clock_app";
    m_info.wnd_width = WIDTH;
    m_info.wnd_height = HEIGHT;
    m_info.samples = 4;
  }
  virtual void create_scene()
  {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


    prg.init();
    p_mat = glm::ortho<GLfloat>(0, wnd_width(), 0, wnd_height());
    vp_mat = glm::translate(p_mat, vec3(WIDTH*0.5, HEIGHT*0.5, 0));

    //arm_geometry.radius(50);
    //arm_geometry.width(300);
    arm_geometry.build_mesh();

    //circle_geometry.build_mesh();
    m_clock.size(WIDTH*0.9);
  }

  virtual void update()
  {
    m_clock.as_since_midnight();
  }

  virtual void display()
  {
    glClear(GL_COLOR_BUFFER_BIT);

    prg.use();
    m_clock.draw();
    //mat4 translate = glm::translate(vec3(WIDTH*0.5, HEIGHT*0.5, 0));
    //mat4 mvp_mat = prg.vp_mat * translate;
    //glUniformMatrix4fv(prg.ul_mvp_mat, 1, 0, glm::value_ptr(mvp_mat));
    //glUniform4f(prg.ul_color, 1, 1, 1, 1);
    
    //m_clock.draw(translate);

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //arm_geometry.draw();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::stringstream ss;
    ss << "";
    m_text.print(ss.str(), 10, wnd_height()- 20);
    glDisable(GL_BLEND);
  }

  virtual void glfw_resize(GLFWwindow *wnd, int w, int h)
  {
    app::glfw_resize(wnd, w, h);
  }

  virtual void glfw_key(
      GLFWwindow *wnd, int key, int scancode, int action, int mods)
  {
    if (action == GLFW_PRESS)
    {
      switch (key)
      {
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

int main(int argc, char *argv[])
{
  zxd::clock_app app;
  app.run();
}
