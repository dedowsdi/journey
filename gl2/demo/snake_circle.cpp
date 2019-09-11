/*
 * issue:
 *  the circle line is jagged even i turn on multisample and line smooth
 */
#include <app.h>
#include <glm.h>

namespace zxd {

class app0 : public app {
private:

  GLfloat m_spacing;
  GLfloat m_rotate_speed;
  GLfloat m_radius_speed;
  GLfloat m_radius = 0;

  vec2_vector m_vertices;
  GLboolean m_updating = GL_TRUE;

  void init_info() override{
    app::init_info();
    m_info.wnd_width = 800;
    m_info.wnd_height = 600;
    m_info.samples = 8;
    m_info.display_mode = GLUT_RGB | GLUT_DOUBLE | GLUT_MULTISAMPLE;
  }

  void create_scene(void) override {

    m_spacing = 4;
    m_rotate_speed = 2 ;
    m_radius_speed = 3 / 60.0;

    glClearColor(0.0, 0.0, 1.0, 0.0);
    glLineWidth(5.0);
    glShadeModel(GL_FLAT);

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glEnable(GL_MULTISAMPLE);

    // 1st point
    m_vertices.push_back(glm::vec2());

  }

  void display(void) override {

    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_LINE_STRIP);
    for (int i = 3; i < m_vertices.size(); ++i) {
      glVertex2f(m_vertices[i].x, m_vertices[i].y);
    }
    glEnd();

    this->drawFpsText();
    glutSwapBuffers();
  }

  void reshape(int w, int h) override{
    app::reshape(w, h);

    GLfloat ar = static_cast<GLfloat>(w)/h;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if(ar >= 1)
      gluOrtho2D(-100, 100, -100/ar, 100/ar);
    else
      gluOrtho2D(-100*ar, 100*ar, -100, 100);
  }

  void update() override
  {
    if(!m_updating)
      return;

    static GLfloat total_time = 0.0f; // in seconds
    glm::vec2 vertex(m_radius * glm::cos(total_time * m_rotate_speed), m_radius * glm::sin(total_time * m_rotate_speed));
    m_vertices.push_back(vertex);

    m_radius += m_radius_speed;
    total_time += m_delta_time;

    if(m_radius > 100 * wnd_aspect() || m_radius > 100 / wnd_aspect())
      m_updating = false;
  }

};
}

int main(int argc, char** argv) {
  zxd::app0 ap;
  ap.run(argc, argv);
  return 0;
}
