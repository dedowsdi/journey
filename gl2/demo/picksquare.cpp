/*
 * picksquare.c
 * Use of multiple names and picking are demonstrated.
 * A 3x3 grid of squares is drawn.  When the left mouse
 * button is pressed, all squares under the cursor position
 * have their color changed.
 */
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>

int board[3][3]; /*  amount of color for each square	*/

/*  Clear color value for every square on the board   */
void init(void) {
  int i, j;
  for (i = 0; i < 3; i++)
    for (j = 0; j < 3; j++) board[i][j] = 0;
  glClearColor(0.0, 0.0, 0.0, 0.0);
}

/*  The nine squares are drawn.  In selection mode, each
 *  square is given two names:  one for the row and the
 *  other for the column on the grid.  The color of each
 *  square is determined by its position on the grid, and
 *  the value in the board[][] array.
 */
void drawSquares(GLenum mode) {
  GLuint i, j;
  for (i = 0; i < 3; i++) {
    if (mode == GL_SELECT) glLoadName(i);
    for (j = 0; j < 3; j++) {
      if (mode == GL_SELECT) glPushName(j);
      glColor3f((GLfloat)i / 3.0, (GLfloat)j / 3.0, (GLfloat)board[i][j] / 3.0);
      glRecti(i, j, i + 1, j + 1);
      if (mode == GL_SELECT) glPopName();
    }
  }
}

/*  processHits prints out the contents of the
 *  selection array.
 */
void processHits(GLint hits, GLuint buffer[]) {
  unsigned int i, j;
  GLuint ii, jj, names, *ptr;

  printf("hits = %d\n", hits);
  ptr = (GLuint*)buffer;
  for (i = 0; i < hits; i++) { /*  for each hit  */
    names = *ptr;
    printf(" number of names for this hit = %d\n", names);
    ptr++;
    printf("  z1 is %g;", (float)*ptr / 0x7fffffff);
    ptr++;
    printf(" z2 is %g\n", (float)*ptr / 0x7fffffff);
    ptr++;
    printf("   names are ");
    for (j = 0; j < names; j++) { /*  for each name */
      printf("%d ", *ptr);
      if (j == 0)   /*  set row and column  */
        ii = *ptr;  // row
      else if (j == 1)
        jj = *ptr;  // col
      ptr++;
    }
    printf("\n");
    board[ii][jj] = (board[ii][jj] + 1) % 3;
  }
}

/*  pickSquares() sets up selection mode, name stack,
 *  and projection matrix for picking.  Then the
 *  objects are drawn.
 */
#define BUFSIZE 512

void pickSquares(int button, int state, int x, int y) {
  if (button != GLUT_LEFT_BUTTON || state != GLUT_DOWN) return;

  GLuint selectBuf[BUFSIZE];
  GLint hits;
  GLint viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);

  glSelectBuffer(BUFSIZE, selectBuf);
  (void)glRenderMode(GL_SELECT);

  glInitNames();
  glPushName(0);

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  /*  create 5x5 pixel picking region near cursor location	*/
  gluPickMatrix(
    (GLdouble)x, (GLdouble)(viewport[3] - y - 1), 5.0, 5.0, viewport);
  gluOrtho2D(0.0, 3.0, 0.0, 3.0);
  drawSquares(GL_SELECT);

  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glFlush();

  hits = glRenderMode(GL_RENDER);
  processHits(hits, selectBuf);
  glutPostRedisplay();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  drawSquares(GL_RENDER);
  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, w, h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, 3.0, 0.0, 3.0);
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

/* Main Loop */
int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(100, 100);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutReshapeFunc(reshape);
  glutDisplayFunc(display);
  glutMouseFunc(pickSquares);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
