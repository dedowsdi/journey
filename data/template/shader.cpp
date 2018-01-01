#include "glad/glad.h"
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "program.h"
#include "glm.h"

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wndAspect = 1;

struct MyProgram : public zxd::Program {
  MyProgram() {
    projMatrix = glm::perspective(glm::radians(45.0f), wndAspect, 0.1f, 30.0f);
    viewMatrix = glm::lookAt(vec3(0, -5, 0), vec3(0, 0, 0), vec3(0, 0, 1));
  }

  virtual void doUpdateFrame() {

  }

  virtual void doUpdateModel() {

  }

  virtual void attachShaders() {

  }

  virtual void bindUniformLocations() {

  }

  virtual void bindAttribLocations() {

  }
} myProgram;

void render(zxd::Program& program) {
  myProgram.updateFrame();

  mat4 model;
  myProgram.updateModel(model);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  render(myProgram);

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

  myProgram.init();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  wndAspect = GLfloat(w) / h;
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
void passiveMotion(int x, int y) {}

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
  glutPassiveMotionFunc(passiveMotion);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  glutMainLoop();

  return 0;
}
