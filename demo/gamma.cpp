/* gamma.c
 *
 * Monitor will apply pow(color, gamma) to transform color from linear space to
 * srgb space. Gamma correction is ued to compensate this, it's done by
 * pow(color, 1/gamma). Lots of textures(such as diffuse) is created directly in
 * srgb space which means it doesn't need gamma correction, make sure it's
 * internalFormat is sRGB, opengl will apply pow(color, gamma) to it to
 * cancel gomma correction.
 */
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include <memory.h>
#include "common.h"

#define imageWidth 64
#define imageHeight 64
static GLubyte image[imageHeight][imageWidth][1];
static GLuint texName;
GLint intensity = 120;
GLint program;
GLboolean srgbTexture = 0;
GLint loc_iamge;

void makeImage(void) {
  memset(image, intensity, imageHeight * imageWidth * sizeof(GLubyte));
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  makeImage();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,
    GL_LUMINANCE, GL_UNSIGNED_BYTE, image);

  program = glCreateProgram();
  attachShaderFile(program, GL_FRAGMENT_SHADER, "data/shader/gamma.fs.glsl");
  ZCGE(glLinkProgram(program));
  setUniformLocation(&loc_iamge, program, "image");
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, texName);

  glBegin(GL_QUADS);

  glTexCoord2i(0, 0);
  glVertex2i(-1, -1);

  glTexCoord2i(1, 0);
  glVertex2i(1, -1);

  glTexCoord2i(1, 1);
  glVertex2i(1, 1);

  glTexCoord2i(0, 1);
  glVertex2i(-1, 1);

  glEnd();

  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 492);
  GLchar info[256];
  GLint currentProgram;
  glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
  glUseProgram(0);
  sprintf(info,
    " q : toggle gamma correction : %d\n"
    " w : toggle sRGB texture : %d\n"
    " eE : change intensity : %d",
    currentProgram != 0, srgbTexture, intensity);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
  glUseProgram(currentProgram);
  glEnable(GL_TEXTURE_2D);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void updateTexture() {
  GLenum internalFormat = srgbTexture ? GL_SRGB : GL_RGB;
  glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, imageWidth, imageHeight, 0,
    GL_LUMINANCE, GL_UNSIGNED_BYTE, image);
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q': {
      GLint currentProgram;
      glGetIntegerv(GL_CURRENT_PROGRAM, &currentProgram);
      if (currentProgram != 0) {
        glUseProgram(0);
      } else {
        glUseProgram(program);
        glUniform1i(loc_iamge, 0);
      }
      glutPostRedisplay();
      break;
    }
    case 'w': {
      srgbTexture = !srgbTexture;
      updateTexture();
      glutPostRedisplay();
      break;
    }
    case 'e': {
      intensity += 15;
      if (intensity > 255) {
        intensity = 255;
      }
      makeImage();
      updateTexture();
      glutPostRedisplay();
      break;
    }
    case 'E': {
      intensity -= 15;
      if (intensity < 0) {
        intensity = 0;
      }
      makeImage();
      updateTexture();
      glutPostRedisplay();
      break;
    }

    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
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
