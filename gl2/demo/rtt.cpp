/*
 * rtt
 *
 * ned a_r_b_framebuffer_object.
 *
 * you can attact render buffer or 2d texture to framebuffer attachment.
 *
 * don't ever use seperated stencil buffer, if you need stencil, always use
 * internal farmat GL_DEPTH24_STENCIL8.
 *
 * if you need to render both stencil and depth , attach a depth stencil
 * renderbuffer, then read depth component and stencil index.
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include "common.h"
#include <GL/freeglut_ext.h>

#define IMAGE_WIDTH 256
#define IMAGE_HEIGHT 256

GLUquadricObj* qobj;  // usded to create disk
GLuint color_tex;
GLuint depth_tex;
GLuint stencil_tex;

GLuint fbo;
GLuint rbo;  // stencil depth render buffer
GLfloat depth_data[IMAGE_WIDTH * IMAGE_HEIGHT];
GLubyte stencil_data[IMAGE_WIDTH * IMAGE_HEIGHT];
GLboolean use_depth_stencil_buffer = GL_FALSE;

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  qobj = gluNewQuadric();

  GLfloat light_position0[] = {1.0, 1.0, 1.0, 0.0};  // 0 w means dir light
  GLfloat lmodel_ambient[] = {0.2, 0.2, 0.2, 1.0};   // global ambient

  glGenFramebuffers(1, &fbo);

  // color texture
  glGenTextures(1, &color_tex);
  glBindTexture(GL_TEXTURE_2D, color_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, IMAGE_WIDTH, IMAGE_HEIGHT, 0, GL_RGB,
    GL_UNSIGNED_BYTE, 0);

  // depth texture
  glGenTextures(1, &depth_tex);
  glBindTexture(GL_TEXTURE_2D, depth_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, IMAGE_WIDTH, IMAGE_HEIGHT,
    0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

  // stencil texture
  glGenTextures(1, &stencil_tex);
  glBindTexture(GL_TEXTURE_2D, stencil_tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, IMAGE_WIDTH, IMAGE_HEIGHT, 0,
    GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, 0);

  // depth stencil buffer
  glGenRenderbuffers(1, &rbo);
  glBindRenderbuffer(GL_RENDERBUFFER, rbo);
  glRenderbufferStorage(
    GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, IMAGE_WIDTH, IMAGE_HEIGHT);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
}

void render_scene() {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  glDisable(GL_LIGHTING);
  glDepthMask(0);
  glPushMatrix();
  glLoadIdentity();

  // build stencil. ref and mask are set to 0xff for visualization.
  glEnable(GL_STENCIL_TEST);
  glStencilFunc(GL_ALWAYS, 0xff, 0xff);
  glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);
  gluDisk(qobj, 0, 1, 32, 1);
  glPopMatrix();
  glDepthMask(1);

  glEnable(GL_LIGHTING);
  glStencilFunc(GL_NOTEQUAL, 0xff, 0xff);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
  glColor3f(1.0, 1.0, 1.0);
  glutSolidSphere(3, 32, 23);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);

  glViewport(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
  // draw scene at low left
  render_scene();

  // render to textures
  if (use_depth_stencil_buffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);
    glFramebufferRenderbuffer(
      GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    render_scene();
    glReadPixels(
      0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_DEPTH_COMPONENT, GL_FLOAT, depth_data);
    glReadPixels(0, 0, IMAGE_WIDTH, IMAGE_HEIGHT, GL_STENCIL_INDEX,
      GL_UNSIGNED_BYTE, stencil_data);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_LIGHTING);

    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // color
    glViewport(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    draw_tex_rect(-1, -1, 1, 1);
    glDisable(GL_TEXTURE_2D);

    // depth
    glWindowPos2i(IMAGE_WIDTH, IMAGE_HEIGHT);
    glDrawPixels(IMAGE_WIDTH, IMAGE_HEIGHT, GL_LUMINANCE, GL_FLOAT, depth_data);

    // stencil
    glWindowPos2i(0, IMAGE_HEIGHT);
    glDrawPixels(
      IMAGE_WIDTH, IMAGE_HEIGHT, GL_LUMINANCE, GL_UNSIGNED_BYTE, stencil_data);

    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  } else {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_tex, 0);
    //glFramebufferTexture2D(
      //GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);
    //glFramebufferTexture2D(
      //GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, stencil_tex, 0);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, depth_tex, 0);
    render_scene();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glDisable(GL_LIGHTING);

    glPushMatrix();
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();

    // color
    glEnable(GL_TEXTURE_2D);
    glViewport(IMAGE_WIDTH, 0, IMAGE_WIDTH, IMAGE_HEIGHT);
    glBindTexture(GL_TEXTURE_2D, color_tex);
    draw_tex_rect(-1, -1, 1, 1);

    // depth + stecil ?
    glViewport(IMAGE_WIDTH, IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT);
    glBindTexture(GL_TEXTURE_2D, depth_tex);
    draw_tex_rect(-1, -1, 1, 1);

    // stencil
    // glViewport(0, IMAGE_HEIGHT, IMAGE_WIDTH, IMAGE_HEIGHT);
    // glBindTexture(GL_TEXTURE_2D, stencil_tex);
    // draw_tex_rect(-1, -1, 1, 1);

    glDisable(GL_TEXTURE_2D);

    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
  }

  glViewport(0, 0, IMAGE_WIDTH * 2, IMAGE_HEIGHT * 2);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_LIGHTING);
  glDisable(GL_STENCIL_TEST);
  char info[256];
  sprintf(info, "q : use depth24stencil8 buffer : %d\n", use_depth_stencil_buffer);

  glWindowPos2i(20, IMAGE_HEIGHT * 2 - 20);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const unsigned char*)info);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-5, 5, -5, 5, -20, 20);
  glMatrixMode(GL_MODELVIEW);  // current matrix
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      use_depth_stencil_buffer = use_depth_stencil_buffer ^ 1;
      glutPostRedisplay();
      break;
    case 'f':
      glutPostRedisplay();
      break;
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH | GLUT_STENCIL);
  glutInitWindowSize(IMAGE_WIDTH * 2, IMAGE_HEIGHT * 2);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadgl();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
