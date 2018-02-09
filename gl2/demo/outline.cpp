/*
 * outline.c.
 *
 * create outline in two pass:
 *
 *  1. draw normal object, create stencil.
 *  2. apply a slight scale to draw normal object again, use stencil to maskout
 *     the overlapping part.
 *
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "glenumstring.h"
#include "glm.h"

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512

GLenum outline_polygon_mode = GL_FILL;
glm::vec4 outline_color(0.0f, 0.0f, 1.0f, 1.0f);
GLfloat outline_scale = 1.01f;
GLfloat outline_line_width = 1.0;
GLfloat outline_point_size = 1.0;

void render() { glutSolidSphere(5, 32, 32); }

void display(void) {
  glDepthMask(GL_TRUE);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  // render object, render stencil.
  glEnable(GL_LIGHTING);
  glStencilFunc(GL_ALWAYS, 1, 1);
  glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
  glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
  render();

  // use stencil to render outline.
  glDisable(GL_LIGHTING);
  glDepthMask(GL_FALSE);
  glStencilFunc(GL_NOTEQUAL, 1, 1);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glColor4fv(glm::value_ptr(outline_color));
  glPolygonMode(GL_FRONT_AND_BACK, outline_polygon_mode);
  glPointSize(outline_point_size);
  glLineWidth(outline_line_width);

  glPushMatrix();
  glScalef(outline_scale, outline_scale, outline_scale);
  render();
  glPopMatrix();

  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_LIGHTING);
  glWindowPos2i(10, 492);
  GLchar info[256];

  // clang-format off
  sprintf(info, 
      "q  : outline polygon mode : %s \n"
      "w  : multisample : %d \n"
      "uU  : outline color : %.2f, %.2f, %.2f, %.2f \n"
      "iI : outline scale : %.2f \n" 
      "oO : outline point size : %.2f \n"
      "pP : outline line width : %.2f \n" ,
      gl_polygon_mode_to_string(outline_polygon_mode),
      glIsEnabled(GL_MULTISAMPLE),
      outline_color[0], outline_color[1], outline_color[2], outline_color[3], 
      outline_scale,
      outline_point_size,
      outline_line_width
      );
  // clang-format on
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glutSwapBuffers();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHT0);
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_DEPTH_TEST);

  GLfloat light_position0[] = {1.0, 1.0, 1.0, 0.0};
  GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_ambient0[] = {0.0, 0.0, 0.0, 1.0};

  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);

  GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SINGLE_COLOR);
  glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, 0);
  glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

  GLfloat mat_ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat mat_diffuse[] = {0.8, 0.8, 0.8, 0.8};
  GLfloat mat_specular[] = {0.5, 0.5, 0.5, 1.0};
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
  if (w <= h)
    glOrtho(-10.0, 10.0, -10.0 * (GLfloat)h / (GLfloat)w,
      10.0 * (GLfloat)h / (GLfloat)w, -1.0, 1.0);
  else
    glOrtho(-10.0 * (GLfloat)w / (GLfloat)h, 10.0 * (GLfloat)w / (GLfloat)h,
      -10.0, 10.0, -1.0, 1.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  glTranslatef(0, 0, -5);
}

void mouse(int button, int state, int x, int y) {
  switch (button) {
    default:
      break;
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q':
    case 'Q':
      outline_polygon_mode = GL_POINT + (outline_polygon_mode - GL_POINT + 1) % 3;
      glutPostRedisplay();
      break;

    case 'w':
    case 'W':
      if (glIsEnabled(GL_MULTISAMPLE)) {
        glDisable(GL_MULTISAMPLE);
      } else {
        glEnable(GL_MULTISAMPLE);
      }
      glutPostRedisplay();
      break;

    case 'u':
    case 'U':
      outline_color = glm::linearRand(glm::vec4(0), glm::vec4(1));
      glutPostRedisplay();
      break;

    case 'i':
      outline_scale += 0.01;
      glutPostRedisplay();
      break;

    case 'I':
      outline_scale -= 0.01;
      if (outline_scale < 1) {
        outline_scale = 1;
      }
      glutPostRedisplay();
      break;

    case 'o':
      outline_point_size += 0.1;
      glutPostRedisplay();
      break;

    case 'O':
      outline_point_size -= 0.1;
      if (outline_point_size < 1) {
        outline_point_size = 1;
      }
      glutPostRedisplay();
      break;

    case 'p':
      outline_line_width += 0.1;
      glutPostRedisplay();
      break;

    case 'P':
      outline_line_width -= 0.1;
      if (outline_point_size < 1) {
        outline_point_size = 1;
      }
      glutPostRedisplay();
      break;


    case 27:
      exit(0);
      break;
  }
}
void idle() {}
void passive_motion(int x, int y) {}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_STENCIL | GLUT_MULTISAMPLE);
  glutInitWindowSize(WINDOWS_WIDTH, WINDOWS_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  zxd::loadgl();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(passive_motion);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  glutMainLoop();

  return 0;
}
