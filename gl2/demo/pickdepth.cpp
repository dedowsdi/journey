/*
 * pickdepth.c
 * picking is demonstrated in this program.  in
 * rendering mode, three overlapping rectangles are
 * drawn.  when the left mouse button is pressed,
 * selection mode is entered with the picking matrix.
 * rectangles which are drawn under the cursor position
 * are "picked."  pay special attention to the depth
 * value range, which is returned.
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
  //change this won't affect zmin and zmax in hits, select mode always use
  //depth range 0,1 
  /*glDepthRange(0.0, 1.0); [> the default z mapping <]*/

}

/* the three rectangles are drawn.  in selection mode,
 * each rectangle is given the same name.  note that
 * each rectangle is drawn with a different z value.
 */
void draw_rects(GLenum mode) {
  if (mode == GL_SELECT) glLoadName(1);
  glBegin(GL_QUADS);
  glColor3f(1.0, 1.0, 0.0);
  glVertex3i(2, 0, 0);
  glVertex3i(2, 6, 0);
  glVertex3i(6, 6, 0);
  glVertex3i(6, 0, 0);
  glEnd();
  if (mode == GL_SELECT) glLoadName(2);
  glBegin(GL_QUADS);
  glColor3f(0.0, 1.0, 1.0);
  glVertex3i(3, 2, -1);
  glVertex3i(3, 8, -1);
  glVertex3i(8, 8, -1);
  glVertex3i(8, 2, -1);
  glEnd();
  if (mode == GL_SELECT) glLoadName(3);
  glBegin(GL_QUADS);
  glColor3f(1.0, 0.0, 1.0);
  glVertex3i(0, 2, -2);
  glVertex3i(0, 7, -2);
  glVertex3i(5, 7, -2);
  glVertex3i(5, 2, -2);
  glEnd();
  ZCGEA
}

/*  process_hits() prints out the contents of the
 *  selection array.
 */
void process_hits(GLint hits, GLuint buffer[]) {
  unsigned int i, j;
  GLuint names, *ptr;

  printf("hits = %d\n", hits);
  ptr = (GLuint *)buffer;
  for (i = 0; i < hits; i++) { /* for each hit  */
    names = *ptr;
    printf(" number of names for hit = %d\n", names);
    ptr++;
    printf("  z1 is %g;", (float)*ptr / 0xffffffff);
    ptr++;
    printf(" z2 is %g\n", (float)*ptr / 0xffffffff);
    ptr++;
    printf("   the name is ");
    for (j = 0; j < names; j++) { /* for each name */
      printf("%d ", *ptr);
      ptr++;
    }
    printf("\n");
  }
}

/*  pick_rects() sets up selection mode, name stack,
 *  and projection matrix for picking.  then the objects
 *  are drawn.
 */
#define BUFSIZE 512

void pick_rects(int button, int state, int x, int y) {
  GLuint select_buf[BUFSIZE];
  GLint hits;
  GLint viewport[4];

  if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

  glGetIntegerv(GL_VIEWPORT, viewport);

  glSelectBuffer(BUFSIZE, select_buf);
  (void)glRenderMode(GL_SELECT);

  glInitNames();
  glPushName(0);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  /*  create 5x5 pixel picking region near cursor location */
  gluPickMatrix(
    (GLdouble)x, (GLdouble)(viewport[3] - y - 1), 5.0, 5.0, viewport);
  glOrtho(0.0, 8.0, 0.0, 8.0, -0.5, 2.5);
  draw_rects(GL_SELECT);
  glPopMatrix();
  glFlush();

  hits = glRenderMode(GL_RENDER);
  process_hits(hits, select_buf);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  draw_rects(GL_RENDER);
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(0.0, 8.0, 0.0, 8.0, -0.5, 2.5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

/*   loop
 */
int main(int argc, char **argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(200, 200);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadgl();
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMouseFunc(pick_rects);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
