/*
 *  blur.cpp
 *
 *  gaussian function is a random variable. It's peak is at 0, which makes it
 *  perfect for blur.
 *
 *  Gaussian blur blend n*n pixels, which means you need to samples n*n samples
 *  to blend them, that's a lot of work, but it can be seperated into horizontal
 *  and vertical blur, which means 2 render pass, each pass blend n pixels, this
 *  is also called two-pass gaussian blur.
 *
 */
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include <GL/freeglut_ext.h>

/*	Create checkerboard texture	*/
#define checkImageWidth 128
#define checkImageHeight 128
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

GLuint checkerTex;
GLuint colorTex;
GLuint program;
GLuint fbo;

GLfloat mean = 0, deviation = 1, gaussianStep = 1;
GLuint radius = 5;
GLuint times = 5;

GLint loc_mean;
GLint loc_deviation;
GLint loc_gaussianStep;
GLint loc_horizontal;

GLuint pingpongFbo[2];
GLuint pingpongTex[2];

void makeCheckImage(void) {
  int i, j, c;

  for (i = 0; i < checkImageHeight; i++) {
    for (j = 0; j < checkImageWidth; j++) {
      c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
      checkImage[i][j][0] = (GLubyte)c;
      checkImage[i][j][1] = (GLubyte)c;
      checkImage[i][j][2] = (GLubyte)c;
      checkImage[i][j][3] = (GLubyte)255;
    }
  }
}

void compileProgram() {
  detachAllShaders(program);
  char preprocessor[256];
  sprintf(preprocessor,
    "#version 120\n"
    "#define radius %d\n",
    radius);
  char* s = preprocessor;

  attachShaderSourceAndFile(
    program, GL_FRAGMENT_SHADER, 1, &s, "data/shader/gaussian_blur.fs.glsl");
  ZCGE(glLinkProgram(program));
  setUniformLocation(&loc_mean, program, "mean");
  setUniformLocation(&loc_deviation, program, "deviation");
  setUniformLocation(&loc_horizontal, program, "horizontal");
  setUniformLocation(&loc_gaussianStep, program, "gaussianStep");
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  // glEnable(GL_DEPTH_TEST);

  makeCheckImage();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &checkerTex);
  glBindTexture(GL_TEXTURE_2D, checkerTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

  glGenTextures(1, &colorTex);
  glBindTexture(GL_TEXTURE_2D, colorTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  program = glCreateProgram();
  compileProgram();

  // create fbo used to get color attachment
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

  glGenFramebuffers(2, pingpongFbo);
  glGenTextures(2, pingpongTex);
  // create pingpong fbo used to blur texture
  for (int i = 0; i < 2; ++i) {
    glBindTexture(GL_TEXTURE_2D, pingpongTex[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(
      GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFbo[i]);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pingpongTex[i], 0);
  }
}

void display(void) {
  // render to texture
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glClear(GL_COLOR_BUFFER_BIT);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, checkerTex);
  drawTexRect(-1, -1, 1, 1);

  // blur
  glUseProgram(program);
  glUniform1f(loc_mean, mean);
  glUniform1f(loc_deviation, deviation);
  glUniform1f(loc_gaussianStep, gaussianStep);
  for (int i = 0; i < times * 2; ++i) {
    GLint index = i & 1;
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFbo[index]);
    GLuint tex = i == 0 ? colorTex : pingpongTex[index ^ 1];
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(loc_horizontal, index ^ 1);
    drawTexRect(-1, -1, 1, 1);
  }

  glUseProgram(0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, pingpongTex[1]);
  drawTexRect(-1, -1, 1, 1);

  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 0.0f, 0.0f);
  glWindowPos2i(10, 492);
  GLchar info[512];
  sprintf(info,
    "qQ : mean : %.2f \n"
    "wW : deviation : %.2f \n"
    "eE : gaussianStep : %.2f \n"
    "rR : times : %d \n"
    "uU : radius : %d \n",
    mean, deviation, gaussianStep, times, radius);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      mean += 0.1;
      glutPostRedisplay();
      break;

    case 'Q':
      mean -= 0.1;
      glutPostRedisplay();
      break;

    case 'w':
      deviation += 0.1;
      glutPostRedisplay();
      break;

    case 'W':
      deviation -= 0.1;
      glutPostRedisplay();
      break;

    case 'e':
      gaussianStep += 0.02;
      glutPostRedisplay();
      break;

    case 'E':
      gaussianStep -= 0.02;
      glutPostRedisplay();
      break;

    case 'r':
      times += 1;
      glutPostRedisplay();
      break;

    case 'R':
      if (times > 1) {
        times -= 1;
        glutPostRedisplay();
      }
      break;

    case 'u':
      radius += 1;
      compileProgram();
      glutPostRedisplay();
      break;

    case 'U':
      if (radius > 1) {
        radius -= 1;
        compileProgram();
        glutPostRedisplay();
      }
      break;

    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(512, 512);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  initExtension();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
