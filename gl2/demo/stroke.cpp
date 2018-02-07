/*
 *  stroke.c
 *  this program demonstrates some characters of a
 *  stroke (vector) font.  the characters are represented
 *  by display lists, which are given numbers which
 *  correspond to the ASCII values of the characters.
 *  use of glCallLists() is demonstrated.
 */
#include "app.h"

namespace zxd {

#define PT 1  // point
#define STROKE 2
#define END 3

// clang-format off
typedef struct charpoint {
  GLfloat x, y;
  int type;
} CP;

// 8*10
CP adata[] = {
  {0, 0, PT},
  {0, 9, PT},
  {1, 10, PT},
  {4, 10, PT},
  {5, 9, PT},
  {5, 0, STROKE},
  {0, 5, PT},
  {5, 5, END}
};

CP edata[] = {
  {5, 0, PT},
  {0, 0, PT},
  {0, 10, PT},
  {5, 10, STROKE},
  {0, 5, PT},
  {4, 5, END}
};

CP pdata[] = {
  {0, 0, PT},
  {0, 10, PT},
  {4, 10, PT},
  {5, 9, PT},
  {5, 6, PT},
  {4, 5, PT},
  {0, 5, END}
}; 

CP rdata[] = {
  {0, 0, PT},
  {0, 10, PT},
  {4, 10, PT},
  {5, 9, PT},
  {5, 6, PT},
  {4, 5, PT},
  {0, 5, STROKE},
  {3, 5, PT},
  {5, 0, END}
};

CP sdata[] = {
  {0, 1, PT},
  {1, 0, PT},
  {4, 0, PT},
  {5, 1, PT},
  {5, 4, PT},
  {4, 5, PT},
  {1, 5, PT},
  {0, 6, PT},
  {0, 9, PT},
  {1, 10, PT},
  {4, 10, PT},
  {5, 9, END}
};
// clang-format on
const char *test1 = "A SPARE SERAPE APPEARS AS";
const char *test2 = "APES PREPARE RARE PEPPERS";

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "stroke";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 440;
    m_info.wnd_height = 120;
  }

  /*  draw_letter() interprets the instructions from the array
   *  for that letter and renders the letter with line segments.
   */
  void draw_letter(CP *l) {
    glBegin(GL_LINE_STRIP);
    while (1) {
      switch (l->type) {
        case PT:
          glVertex2fv(&l->x);
          break;
        case STROKE:
          glVertex2fv(&l->x);
          glEnd();
          glBegin(GL_LINE_STRIP);
          break;
        case END:
          glVertex2fv(&l->x);
          glEnd();
          glTranslatef(8.0, 0.0, 0.0);
          return;
      }
      l++;
    }
  }

  /*  create a display list for each of 6 characters	*/
  void create_scene(void) {
    GLuint base;

    glShadeModel(GL_FLAT);

    // 128 lists for all asc2 characters
    base = glGenLists(128);
    glListBase(base);  // set base to the 1st display list,

    glNewList(base + 'A', GL_COMPILE);
    draw_letter(adata);
    glEndList();

    glNewList(base + 'E', GL_COMPILE);
    draw_letter(edata);
    glEndList();

    glNewList(base + 'P', GL_COMPILE);
    draw_letter(pdata);
    glEndList();

    glNewList(base + 'R', GL_COMPILE);
    draw_letter(rdata);
    glEndList();

    glNewList(base + 'S', GL_COMPILE);
    draw_letter(sdata);
    glEndList();

    glNewList(base + ' ', GL_COMPILE);
    glTranslatef(8.0, 0.0, 0.0);
    glEndList();
  }

  // use char in string as display list index
  void print_stroked_string(const char *s) {
    GLsizei len = strlen(s);
    glCallLists(len, GL_BYTE, (GLbyte *)s);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(1.0, 1.0, 1.0);
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);
    glTranslatef(10.0, 30.0, 0.0);
    print_stroked_string(test1);
    glPopMatrix();
    glPushMatrix();
    glScalef(2.0, 2.0, 2.0);
    glTranslatef(10.0, 13.0, 0.0);
    print_stroked_string(test2);
    glPopMatrix();
    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
  }

  /*  main loop
   *  open window with initial window size, title bar,
   *  RGBA display mode, and handle input events.
   */
};
}
int main(int argc, char **argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
