#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "program.h"
#include "glm.h"
#include "texutil.h"

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wndAspect = 1;

GLuint bitmapTex;
vec4 textColor(1.0f);
GLuint vao;
GLuint vbo;

struct MyProgram : public zxd::Program {
  GLint attrib_vertex;
  GLint loc_textColor;
  GLint loc_fontMap;
  MyProgram() {
    modelViewProjMatrix = glm::ortho(
      0.0f, (GLfloat)WINDOWS_WIDTH, 0.0f, (GLfloat)WINDOWS_HEIGHT, -1.0f, 1.0f);
  }
  virtual void doUpdateFrame() {
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
    glUniform4fv(loc_textColor, 1, value_ptr(textColor));
    glUniform1i(loc_fontMap, 0);
  }
  virtual void doUpdateModel() {}
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/bitmap_text.vs.glsl");
    attachShaderFile(GL_FRAGMENT_SHADER, "data/shader/bitmap_text.fs.glsl");
  }
  virtual void bindUniformLocations() {
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_textColor, "textColor");
    setUniformLocation(&loc_fontMap, "fontMap");
  }
  virtual void bindAttribLocations() {
    attrib_vertex = getAttribLocation("vertex");
  }
} myProgram;

// assume 16 * 16 characters in texture
void printText2D(const std::string& text, GLuint x, GLuint y, GLuint size) {
  myProgram.updateFrame();
  zxd::Vec4Vector vertices;  // {x, y, s, t}

  float texStep = 1 / 16.0f;
  GLuint nextY = y;
  GLuint nextX = x;

  // collect character quads
  for (int i = 0; i < text.size(); ++i) {
    char character = text[i];

    if (character == '\n') {
      nextX = x;
      nextY -= size;
      continue;
    }

    vec2 vertex0(nextX, nextY);
    vec2 texcoord0(
      (character & 0xf) * texStep, (15 - character / 16) * texStep);

    // clang-format off
    vertices.push_back(vec4(vertex0, texcoord0));
    vertices.push_back(
      vec4(vertex0 + vec2(size, 0),    texcoord0 + vec2(texStep, 0)));
    vertices.push_back(
      vec4(vertex0 + vec2(size, size), texcoord0 + vec2(texStep, texStep)));
    vertices.push_back(
      vec4(vertex0 + vec2(0,    size), texcoord0 + vec2(0,       texStep)));
    // clang-format on

    nextX += size;
  }

  glBindVertexArray(vao);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices),
    value_ptr(vertices[0]), GL_STATIC_DRAW);

  glVertexAttribPointer(myProgram.attrib_vertex, 4, GL_FLOAT, GL_FALSE,
    sizeof(glm::vec4), BUFFER_OFFSET(0));
  glEnableVertexAttribArray(myProgram.attrib_vertex);

  glBindTexture(GL_TEXTURE_2D, bitmapTex);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

  glDrawArrays(GL_QUADS, 0, vertices.size());
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  myProgram.updateFrame();
  printText2D("aaaaa\nbADSF\ncsdfsdfs\nqADFDFS", 10, 400, 32);

  glutSwapBuffers();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  myProgram.init();

  fipImage fip = zxd::fipLoadImage("data/Holstein.png");
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &bitmapTex);
  glBindTexture(GL_TEXTURE_2D, bitmapTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, fip.getWidth(), fip.getHeight(), 0,
    GL_RED, GL_UNSIGNED_BYTE, fip.accessPixels());

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);
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
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
  glutInitWindowSize(WINDOWS_WIDTH, WINDOWS_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  initExtension();
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
