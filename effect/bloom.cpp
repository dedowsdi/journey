/*
 *  bloom.cpp
 *  Need Arb_framebuffer_object, Ext_gpu_shader4, Arb_texture_float
 *
 *  bleeds around the light source.
 *
 *    1. generate hdr map and brightness map.
 *    2. gaussian blur brightness map to make it bleeds.
 *    3. blend(add) hdr map and brightness map, tone map back to ldr.
 *
 */
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"
#include <GL/freeglut_ext.h>

/*	Create checkerboard texture	*/
#define checkImageWidth 128
#define checkImageHeight 128
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

GLuint checkerTex;
GLuint colorTex, brightnessTex;
GLuint filterBrightessProgram, blurProgram, bloomProgram;
GLuint fbo;

GLuint times = 5;  // blur times
GLfloat threshold = 0.95;
GLfloat exposure = 1;
GLint loc_horizontal;
GLint loc_threshold;
GLint loc_exposure;
GLint loc_hdrMap;
GLint loc_brightnessMap;

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
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, checkImageWidth, checkImageHeight,
    0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

  glGenTextures(1, &colorTex);
  glBindTexture(GL_TEXTURE_2D, colorTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  glGenTextures(1, &brightnessTex);
  glBindTexture(GL_TEXTURE_2D, brightnessTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

  // create fbo used to get color texture and brightness texture
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, brightnessTex, 0);

  // Enable both attachments as draw buffers
  GLenum drawbuffers[] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
  glDrawBuffers(2, drawbuffers);

  // create pingpong fbo used to blur texture
  glGenFramebuffers(2, pingpongFbo);
  glGenTextures(2, pingpongTex);
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

  // brightness filter program
  filterBrightessProgram = glCreateProgram();
  attachShaderFile(filterBrightessProgram, GL_FRAGMENT_SHADER,
    "data/shader/filter_brightness.fs.glsl");
  ZCGE(glLinkProgram(filterBrightessProgram));
  setUniformLocation(&loc_threshold, filterBrightessProgram, "threshold");

  blurProgram = glCreateProgram();
  attachShaderFile(blurProgram, GL_FRAGMENT_SHADER,
    "data/shader/gaussian_blur_0_1_0.5_4.fs.glsl");
  ZCGE(glLinkProgram(blurProgram));
  setUniformLocation(&loc_horizontal, blurProgram, "horizontal");

  bloomProgram = glCreateProgram();
  attachShaderFile(
    bloomProgram, GL_FRAGMENT_SHADER, "data/shader/bloom.fs.glsl");
  ZCGE(glLinkProgram(bloomProgram));
  setUniformLocation(&loc_exposure, bloomProgram, "exposure");
  setUniformLocation(&loc_hdrMap, bloomProgram, "hdrMap");
  setUniformLocation(&loc_brightnessMap, bloomProgram, "brightnessMap");
}

void display(void) {
  // extract textures
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glUseProgram(filterBrightessProgram);
  glUniform1f(loc_threshold, threshold);

  glClear(GL_COLOR_BUFFER_BIT);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, checkerTex);
  glEnable(GL_TEXTURE_2D);

  drawTexRect(-1, -1, 1, 1);

  // blur brightness texture
  glUseProgram(blurProgram);
  for (int i = 0; i < times * 2; ++i) {
    GLint index = i & 1;
    glBindFramebuffer(GL_FRAMEBUFFER, pingpongFbo[index]);
    GLuint tex = i == 0 ? brightnessTex : pingpongTex[index ^ 1];
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(loc_horizontal, index ^ 1);
    drawTexRect(-1, -1, 1, 1);
  }

  // add bleeding brightness
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glUseProgram(bloomProgram);
  glUniform1f(loc_exposure, exposure);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, colorTex);
  glUniform1i(loc_hdrMap, 0);

  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, pingpongTex[1]);
  glUniform1i(loc_brightnessMap, 1);

  drawTexRect(-1, -1, 1, 1);

  glActiveTexture(GL_TEXTURE1);
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0);
  glDisable(GL_TEXTURE_2D);

  glUseProgram(0);
  glColor3f(1.0f, 0.0f, 0.0f);
  glWindowPos2i(10, 492);
  GLchar info[512];
  sprintf(info,
    "qQ : threshold : %.2f \n"
    "wW : times : %d \n"
    "eE : exposure : %.2f \n",
    threshold, times, exposure);
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
      threshold += 0.1;
      glutPostRedisplay();
      break;
    case 'Q':
      threshold -= 0.1;
      glutPostRedisplay();
      break;

    case 'w':
      times += 1;
      glutPostRedisplay();
      break;
    case 'W':
      if (times > 1) {
        times -= 1;
        glutPostRedisplay();
      }
      break;

    case 'e':
      exposure += 0.1;
      glutPostRedisplay();
      break;
    case 'E':
      exposure -= 0.1;
      glutPostRedisplay();
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
