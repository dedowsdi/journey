/*  bezsurf.c
 *  This program renders a wireframe Bezier surface,
 *  using two-dimensional evaluators.
 *
 *  It's toally fine to switch ustride and vstride(and uorder, vorder).  works
 *  like sum from row to col or from col to row, it's like switch u, v dir,
 *  q(u,v) = p(v,u)
 *
 *  But the generated normal will rotate 180!!!, be very careful.
 *
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/freeglut.h>
#include <stdlib.h>
#include <GL/freeglut_ext.h>

GLuint ustride = 12;
GLuint vstride = 3;
GLuint uorder = 5;
GLuint vorder = 4;
GLboolean uonly = GL_FALSE;  // only render u dir line
GLboolean rotate = GL_TRUE;

// it's an even grid if you look from positive z
// clang-format off
GLfloat ctrlpoints[5][4][3] = {
   {{-1.5, -1.5, 4.0}, {-0.5, -1.5, 2.0}, {0.5, -1.5, -1.0}, {1.5, -1.5, 2.0}}, 
   {{-1.5, -0.5, 1.0}, {-0.5, -0.5, 3.0}, {0.5, -0.5, 0.0}, {1.5, -0.5, -1.0}}, 
   {{-1.5, 0.5, 4.0}, {-0.5, 0.5, 0.0}, {0.5, 0.5, 3.0}, {1.5, 0.5, 4.0}}, 
   {{-1.5, 1.5, -2.0}, {-0.5, 1.5, -2.0}, {0.5, 1.5, 0.0}, {1.5, 1.5, -1.0}},
   {{-1.5, 2.0, -2.0}, {-0.5, 2.0, -2.0}, {0.5, 2.0, 0.0}, {1.5, 2.0, -1.0}}
};
// clang-format on

void display(void) {
  int i, j;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glColor3f(1.0, 1.0, 1.0);
  glPushMatrix();
  if (rotate) glRotatef(85.0, 1.0, 1.0, 1.0);
  //can be replaced by glEvalMesh2
  for (j = 0; j <= 8; j++) {  // 8 curves along both directions
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= 30; i++)  // 30 samples per curve
      glEvalCoord2f((GLfloat)i / 30.0, (GLfloat)j / 8.0);
    glEnd();
    if (!uonly) {
      glBegin(GL_LINE_STRIP);
      for (i = 0; i <= 30; i++)
        glEvalCoord2f((GLfloat)j / 8.0, (GLfloat)i / 30.0);
      glEnd();
    }
  }

  glPopMatrix();

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 480);
  const GLubyte info[] =
    "qQ : switch u v stride \n"
    "wW : toggle render u dir line only \n"
    "eE : toggle rotate \n";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);

  glFlush();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  // this is 2by2 control nets, it's ok to switch ustride and vstride
  glMap2f(
    GL_MAP2_VERTEX_3, 0, 1, ustride, uorder, 0, 1, vstride, vorder, &ctrlpoints[0][0][0]);
  glEnable(GL_MAP2_VERTEX_3);
  glMapGrid2f(20, 0.0, 1.0, 20, 0.0, 1.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_FLAT);
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    glOrtho(-4.0, 4.0, -4.0 * (GLfloat)h / (GLfloat)w,
      4.0 * (GLfloat)h / (GLfloat)w, -4.0, 4.0);
  else
    glOrtho(-4.0 * (GLfloat)w / (GLfloat)h, 4.0 * (GLfloat)w / (GLfloat)h, -4.0,
      4.0, -4.0, 4.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
    case 'Q': {
      GLuint temp = ustride;
      ustride = vstride;
      vstride = temp;
      temp = uorder;
      uorder = vorder;
      vorder = temp;
      glMap2f(GL_MAP2_VERTEX_3, 0, 1, ustride, uorder, 0, 1, vstride, vorder,
        &ctrlpoints[0][0][0]);
      glutPostRedisplay();
    }

    break;
    case 'w':
    case 'W':
      uonly = !uonly;
      glutPostRedisplay();
      break;
    case 'e':
    case 'E':
      rotate = !rotate;
      glutPostRedisplay();
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
