#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"

GLint program;
GLint loc_time;
GLint loc_time_scale;
GLint loc_resolution;
GLint loc_length_scale;
GLint loc_origin;
GLint loc_use_origin;
GLint width = 1024;
GLint height = 576;

GLfloat time_scale = 5;
GLfloat length_scale = 60;
GLfloat fps = 0;
GLint follow_mouse = 0;

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  program = glCreateProgram();
  attachShaderFile(program, GL_FRAGMENT_SHADER, "data/shader/circular.fs.glsl");
  ZCGE(glLinkProgram(program))
  setUniformLocation(&loc_time, program, "time");
  setUniformLocation(&loc_resolution, program, "resolution");
  setUniformLocation(&loc_time_scale, program, "time_scale");
  setUniformLocation(&loc_length_scale, program, "length_scale");
  setUniformLocation(&loc_origin, program, "origin");
  setUniformLocation(&loc_use_origin, program, "use_origin");

  glUseProgram(program);
  glUniform1f(loc_length_scale, length_scale);
  glUniform1f(loc_time_scale, time_scale);
  glUniform1i(loc_use_origin, follow_mouse);
  ZCGEA;
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glRecti(-1, -1, 1, 1);

  glUseProgram(0);
  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, height - 20);
  GLchar info[512];
  sprintf(info,
    "fps : %.2f\n"
    "qQ : length scale %.2f\n"
    "wW : time scale %.2f\n"
    "e : toggle follow_mouse %d",
    updateFps(), length_scale, time_scale, follow_mouse);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
  glUseProgram(program);

  glFlush();
  ZCGEA
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  ZCGE(glUniform2f(loc_resolution, w, h));
  width = w;
  height = h;
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      length_scale += 0.5;
      glUniform1f(loc_length_scale, length_scale);
      break;
    case 'Q':
      length_scale -= 0.5;
      glUniform1f(loc_length_scale, length_scale);
      break;
    case 'w':
      time_scale += 0.5;
      glUniform1f(loc_time_scale, time_scale);
      break;
    case 'W':
      time_scale -= 0.5;
      glUniform1f(loc_time_scale, time_scale);
      break;
    case 'e':
      follow_mouse = !follow_mouse;
      glUniform1i(loc_use_origin, follow_mouse);
      break;
    default:
      break;
  }
}

void idle() {
  glUniform1f(loc_time, getTime());
  glutPostRedisplay();
}

void mouse(int button, int state, int x, int y) {
  if (follow_mouse == 1) {
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glUniform2f(loc_origin, x, viewport[3] - y - 1);
  }
}

void passiveMotion(int x, int y) {
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT, viewport);
  glUniform2f(loc_origin, x, viewport[3] - y - 1);
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(width, height);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  initExtension();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(passiveMotion);
  glutIdleFunc(idle);
  glutMainLoop();
  return 0;
}
