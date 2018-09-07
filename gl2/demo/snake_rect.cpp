#include "app.h"
#include "glm.h"

namespace zxd {

class app0 : public app {
private:

  GLfloat m_spacing;
  GLfloat m_speed;
  vec2_vector m_vertices;
  glm::vec2 m_dest; // dest of current vertex
  GLboolean m_updating = GL_TRUE;

  void init_info() override{
    app::init_info();
    m_info.wnd_width = 600;
    m_info.wnd_height = 600;
    m_info.display_mode = GLUT_RGB | GLUT_DOUBLE;
  }

  void create_scene(void) override {

    m_spacing = 4;
    m_speed = 2000/60.0;

    glClearColor(0.0, 0.0, 1.0, 0.0);
    glLineWidth(5.0);
    glShadeModel(GL_FLAT);

    // add three guide vertices for 1st three vertices
    m_vertices.push_back(glm::vec2(-100, 100));
    m_vertices.push_back(glm::vec2(100, 100));
    m_vertices.push_back(glm::vec2(100 - m_spacing, -100 ));

    // 1st point
    m_vertices.push_back(glm::vec2(-100 + m_spacing, -100 ));

    addNewVertex();
  }

  void addNewVertex(){

    //m_vertices.push_back(m_dest);

    const glm::vec2& v0 = m_vertices.back();
    const glm::vec2& v1 = m_vertices.at(m_vertices.size() - 2);
    const glm::vec2& v2 = m_vertices.at(m_vertices.size() - 4);
    m_dest = v0;

    // update dest point
    if(v0.y == v1.y)
    {
      int sign = v0.x < 0 ? -1 : 1;
      m_dest.y = v2.y + m_spacing * sign;
      if( std::abs(m_dest.y - v0.y) <= m_spacing)
      {
        m_updating = false;
        return;
      }

    }
    else
    {
      int sign = v0.y < 0 ? 1 : -1;
      m_dest.x = v2.x + m_spacing * sign;
      if( std::abs(m_dest.x - v0.x) <= m_spacing)
      {
        m_updating = false;
        return;
      }
    }
    std::cout << "add new vertex, reset target to" <<m_dest.x << ":" << m_dest.y <<  std::endl;

    m_vertices.push_back(v0);
  }

  void display(void) override {

    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(1, 1, 1);
    glBegin(GL_LINE_STRIP);
    for (int i = 3; i < m_vertices.size(); ++i) {
      glVertex2f(m_vertices[i].x, m_vertices[i].y);
    }
    glEnd();

    glColor3f(1, 0, 0);
    glPointSize(5);
    glBegin(GL_POINTS);
    for (int i = 3; i < m_vertices.size(); ++i) {
      glVertex2f(m_vertices[i].x, m_vertices[i].y);
    }
    glEnd();

    this->drawFpsText();
    glutSwapBuffers();
  }

  void reshape(int w, int h) override{
    app::reshape(w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-100, 100, -100, 100);
  }

  void update() override
  {
    if(!m_updating)
      return;

    glm::vec2& v0 = m_vertices.back();
    glm::vec2 step = m_dest - v0;
    if(step.x != 0 )
      step.x = m_speed * step.x / std::abs(step.x);
    else
      step.y = m_speed * step.y / std::abs(step.y);

    v0 += step;
    if(glm::dot(step, m_dest - v0) <= 0)
    {
      v0 = m_dest;
      addNewVertex();
    }

  }

};
}

int main(int argc, char** argv) {
  zxd::app0 ap;
  ap.run(argc, argv);
  return 0;
}
