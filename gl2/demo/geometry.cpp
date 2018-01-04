#include "glad/glad.h"
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "glEnumString.h"

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512

GLfloat light_position0[] = {1.0, -1.0, 1.0, 0.0};

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glutSolidSphere(1, 16, 16);

  glPushMatrix();
  glTranslatef(2, 0, 0);
  glutSolidCone(1.0, 1.0, 8, 8);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(4, 0, 0);
  glutSolidCylinder(1, 1, 8, 8);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-2, 0, 0);
  glutSolidCube(1);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(-4, 0, 0);
  glutSolidTorus(0.5, 1, 7, 16);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(0, 0, 2);
  glPopMatrix();

  GLint polygonMode[2];
  glGetIntegerv(GL_POLYGON_MODE, polygonMode);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 492);
  GLchar info[256];
  sprintf(info, "q : polygonMode : %s \n", glPolygonModeToString(polygonMode[0]));
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glutSwapBuffers();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

  GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular0[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat light_ambient0[] = {0.0, 0.0, 0.0, 1.0};

  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);

  GLfloat mat_ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat mat_diffuse[] = {0.8, 0.8, 0.8, 0.8};
  GLfloat mat_specular[] = {0.8, 0.8, 0.8, 1.0};
  GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat mat_shininess = 30;

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, (GLfloat)w / h, 0.1, 50);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, -10, 10, 0, 0, 0, 0, 0, 1);

  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
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
    case 'q': {
      GLint polygonMode[2];
      glGetIntegerv(GL_POLYGON_MODE, polygonMode);
      polygonMode[0] = GL_POINT + (polygonMode[0] + 1 - GL_POINT) % 3;
      glPolygonMode(GL_FRONT_AND_BACK, polygonMode[0]);
      glutPostRedisplay();
    } break;
    default:
      break;
  }
}
void idle() {}
void passiveMotion(int x, int y) {}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
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
