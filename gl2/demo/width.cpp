/*
 * line with width >=1 will spread in both direction.
 * point with size >=1 whill spread in all direction
 */
#include "app.h"
#include "glm.h"

namespace zxd {

class app0 : public app {
private:


  void init_info() override{
    app::init_info();
    m_info.wnd_width = 600;
    m_info.wnd_height = 600;
    m_info.display_mode = GLUT_RGB | GLUT_DOUBLE;
  }

  void create_scene(void) override {
    glClearColor(0, 0, 1, 0);
  }

  void drawLine(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLfloat lineWidth, const glm::vec3& color){
    glLineWidth(lineWidth);
    glColor3fv(glm::value_ptr(color));
    glBegin(GL_LINES);
    glVertex2f(x0, y0);
    glVertex2f(x1, y1);
    glEnd();
  }

  void drawPoint(GLfloat x0, GLfloat y0, GLfloat pointSize, const glm::vec3& color){
    glPointSize(pointSize);
    glColor3fv(glm::value_ptr(color));
    glBegin(GL_POINTS);
    glVertex2f(x0, y0);
    glEnd();
  }

  void display(void) override {
    glClear(GL_COLOR_BUFFER_BIT);
    glm::vec3 white(1, 1, 1);
    glm::vec3 black(0, 0, 0);

    drawLine(10, 10, 60, 10, 20, white);
    drawLine(10, 10, 60, 10, 1, black);

    drawLine(10, 20, 60, 60, 20, white);
    drawLine(10, 20, 60, 60, 1, black);

    drawPoint(10, 50, 20, white);
    drawPoint(10, 50, 1, black);

    glutSwapBuffers();

  }

  void reshape(int w, int h) override{
    app::reshape(w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 100, 0, 100);
  }

  void update() override
  {
  }

};
}

int main(int argc, char** argv) {
  zxd::app0 ap;
  ap.run(argc, argv);
  return 0;
}
