#include "glad/glad.h"
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include <common.h>
#include <program.h>
#include <glm.h>

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wnd_aspect = 1;

struct my_program : public zxd::program {
  my_program() {
    p_mat = glm::perspective(glm::radians(45.0f), wnd_aspect, 0.1f, 30.0f);
    v_mat = glm::lookAt(vec3(0, -5, 0), vec3(0, 0, 0), vec3(0, 0, 1));
  }

  virtual void update_frame() {

  }

  virtual void update_uniforms(const glm::mat4& _m_mat) {
    m_mat = _m_mat;

  }

  virtual void attach_shaders() {

  }

  virtual void bind_uniform_locations() {

  }

  virtual void bind_attrib_locations() {

  }
} my_program;

void render(zxd::program& program) {
  my_program.update_frame();

  mat4 model;
  my_program.update_uniforms(model);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  render(my_program);

  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 492);
  GLchar info[256];

  sprintf(info, " \n");
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glutSwapBuffers();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);

  my_program.init();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  wnd_aspect = GLfloat(w) / h;
}

void mouse(int button, int state, int x, int y) {
  switch (button) {
    default:
      break;
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}
void idle() {}
void passive_motion(int x, int y) {}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize(WINDOWS_WIDTH, WINDOWS_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadGL();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(passive_motion);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  glutMainLoop();

  return 0;
}
