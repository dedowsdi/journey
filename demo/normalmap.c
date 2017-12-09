/* normalmap.c
 *
 * Used to give high detail to low quality geomety.
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include <memory.h>
#include "common.h"

#define imageWidth 256
#define imageHeight 256

static GLubyte image[imageHeight][imageWidth][3];
static GLuint texName;
GLint generate_normalmap_program;
GLint normalmap_program;

GLuint buffer;

GLfloat vertices[5][9] = {
  {-1, -1, 1},
};

void generateNormalMap() {
  // render high detail geometry to generate normal map
  glViewport(0, 0, imageWidth, imageHeight);
  glMatrixMode(GL_PROJECTION);
  glOrtho(-100, 100, -100, 100, -100, 100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glShadeModel(GL_FLAT);
  glEnable(GL_DEPTH_TEST);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  generate_normalmap_program = glCreateProgram();
  attachShaderFile(generate_normalmap_program, GL_VERTEX_SHADER,
    "generate_normallmap.vs.glsl");
  attachShaderFile(generate_normalmap_program, GL_VERTEX_SHADER,
    "generate_normallmap.fs.glsl");

  // create normal map
  generateNormalMap();

  return;

  normalmap_program = glCreateProgram();
  attachShaderFile(normalmap_program, GL_VERTEX_SHADER, "normalmap.vs.glsl");
  attachShaderFile(normalmap_program, GL_FRAGMENT_SHADER, "normalmap.fs.glsl");

  // texture for normal map
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  /*glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0,*/
  /*GL_LUMINANCE, GL_UNSIGNED_BYTE, 0);*/
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, imageWidth, imageHeight, 0);
}

void display(void) {
  glViewport(imageWidth, imageHeight, imageWidth, imageHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(normalmap_program);
  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, texName);

  // draw low quality geometry
  glBegin(GL_QUADS);

  glEnd();

  glDisable(GL_TEXTURE_2D);
  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 492);
  GLchar info[256];

  sprintf(info, " \n");
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
  glEnable(GL_TEXTURE_2D);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  gluPerspective(45, 1, 1, 100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 2, 2, 0, 0, 0, 0, 1, 0);
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
  glutInitWindowSize(512, 256);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
