/*
 *  vbo.c
 *  this program demonstrates vertex buffer usage
 *
 *  buffer object allow data to be stored in the server and transformed only
 *  once.
 *  if you want to use texcoord or other vertex dates, you can place them in the
 *  same buffer as position or in a seperated buffer.
 */
#include "app.h"

namespace zxd {

#define COLORS 2
#define VERTICES 0
#define INDICES 1
#define NUM_BUFFERS 3

GLuint buffers[NUM_BUFFERS];

// clang-format off
GLfloat vertices[][3] = {
  { -1.0, -1.0, -1.0 },
  { 1.0,  -1.0, -1.0 },
  { 1.0,  1.0, -1.0 },
  { -1.0, 1.0, -1.0 },
  { -1.0, -1.0, 1.0 },
  { 1.0,  -1.0, 1.0 },
  { 1.0,  1.0, 1.0 },
  { -1.0, 1.0, 1.0 }
};
GLfloat colors[][3] = {
  { 0, 0, 0 },
  { 0.5,  0, 0 },
  { 0.5,  0.5, 0 },
  { 0, 0.5, 0 },
  { 0, 0, 0.5 },
  { 0.5,  0, 0.5 },
  { 0.5,  0.5, 0.5 },
  { 0, 0.5, 0.5 }
};
GLubyte indices[][4] = {
  { 0, 1, 2, 3 },
  { 4, 7, 6, 5 },
  { 0, 4, 5, 1 },
  { 3, 2, 6, 7 },
  { 0, 3, 7, 4 },
  { 1, 5, 6, 2 }
};

// clang-format on

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "vbo";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 512;
    m_info.wnd_height = 512;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.5, 1.0, 1.0);
    glShadeModel(GL_FLAT);

    glGenBuffers(NUM_BUFFERS, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[VERTICES]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    // pointer is treated as byte offset into the buffer object's data store if
    // some valid buffer is bound to GL_ARRAY_BUFFER
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnableClientState(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[COLORS]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glColorPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnableClientState(GL_COLOR_ARRAY);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[INDICES]);
    glBufferData(
      GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glEnable(GL_CULL_FACE);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // pointer works the same as it in glVertexPointer
    glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, (double)w / h, 0.1, 100);

    glMatrixMode(GL_MODELVIEW);
    gluLookAt(-5, 5, 10, 0, 0, 0, 0, 1, 0);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 27:
        exit(0);
        break;
    }
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
