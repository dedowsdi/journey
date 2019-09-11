/*
 *  varray.c
 *  this program demonstrates vertex arrays.
 *
 *  dont change vertices data between glBegin and glEnd if you use
 *  glArrayElement to render vertex, it takes effect in nonsequential ways.
 *
 *  if you enable some array, but don't use it, you will be screwed!
 */
#include <app.h>

#define POINTER 1
#define INTERLEAVED 2

#define DRAWARRAY 1
#define ARRAYELEMENT 2
#define DRAWELEMENTS 3

namespace zxd {

int setup_method = POINTER;
int deref_method = DRAWARRAY;

// vertex, color.....array is client state

// clang-format off
 GLint vertices[] = {
                     25, 25,
                     100, 325,
                     175, 25,
                     175, 325,
                     250, 25,
                     325, 325};
 GLfloat colors[] = {
                     1.0, 0.2, 0.2,
                     0.2, 0.2, 1.0,
                     0.8, 1.0, 0.2,
                     0.75, 0.75, 0.75,
                     0.35, 0.35, 0.35,
                     0.5, 0.5, 0.5};
// clang-format on

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "varray";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 350;
    m_info.wnd_height = 350;
  }

  void setup_pointers(void) {
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_INT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);
  }

  void setup_interleave(void) {
    // clang-format off
  // both colors and vertices in this array
   static GLfloat intertwined[] =
      {1.0, 0.2, 1.0, 100.0, 100.0, 0.0,
       1.0, 0.2, 0.2, 0.0, 200.0, 0.0,
       1.0, 1.0, 0.2, 100.0, 300.0, 0.0,
       0.2, 1.0, 0.2, 200.0, 300.0, 0.0,
       0.2, 1.0, 1.0, 300.0, 200.0, 0.0,
       0.2, 0.2, 1.0, 200.0, 100.0, 0.0};
    // clang-format on

    // glInterleavedArrays enable related client states automatically.
    glInterleavedArrays(GL_C3F_V3F, 0, intertwined);
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);
    setup_pointers();
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    if (deref_method == DRAWARRAY) {
      glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    else if (deref_method == ARRAYELEMENT) {
      glBegin(GL_TRIANGLES);

      // vertices[5] = 175; //this has no effect
      glArrayElement(2);
      // vertices[5] = 25; //this will take effect on previous glArrayElement
      // call
      glArrayElement(3);
      glArrayElement(5);
      glEnd();
    } else if (deref_method == DRAWELEMENTS) {
      GLuint indices[4] = {0, 1, 3, 4};

      glDrawElements(GL_POLYGON, 4, GL_UNSIGNED_INT, indices);
    }
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
  }

  void mouse(int button, int state, int x, int y) {
    switch (button) {
      case GLUT_LEFT_BUTTON:
        if (state == GLUT_DOWN) {
          if (setup_method == POINTER) {
            setup_method = INTERLEAVED;
            setup_interleave();
          } else if (setup_method == INTERLEAVED) {
            setup_method = POINTER;
            setup_pointers();
          }
          glutPostRedisplay();
        }
        break;
      case GLUT_MIDDLE_BUTTON:
      case GLUT_RIGHT_BUTTON:
        if (state == GLUT_DOWN) {
          if (deref_method == DRAWARRAY)
            deref_method = ARRAYELEMENT;
          else if (deref_method == ARRAYELEMENT)
            deref_method = DRAWELEMENTS;
          else if (deref_method == DRAWELEMENTS)
            deref_method = DRAWARRAY;
          glutPostRedisplay();
        }
        break;
      default:
        break;
    }
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
  }
};
}

int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
