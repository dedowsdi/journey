/*
 *  hdr.c
 *  Need Arb_framebuffer_object Arb_texture_float
 *
 *  HDR(high dynamic range) is used to break the limit of [0,1] in framebuffer.
 *  You should use GL_RGBAF* family internal format for framebuffer color
 *  attachment, it's also called floating point framebuffer.
 *
 *  The procedure of transform color back to [0,1] is called tone map.
 *  Reinhard toon map:
 *    x/(1+x)
 *  simple exposre toon map:
 *    1 - e^(-x * exposure)
 *
 *  Any function that result in [0,1] should work.
 */
#include <GL/glew.h>
#include <GL/glut.h>
#include <stdlib.h>
#include "common.h"
#include <GL/freeglut_ext.h>
GLUquadricObj* qobj;

GLuint fbo;
GLuint colorTex;
GLuint program;
GLboolean useHdr = 0;

GLint method = 0;
GLfloat exposure = 1.0;

GLint loc_method;
GLint loc_exposure;

const char* methods[] = {"Reinhard", "exposure"};

#define checkImageWidth 256
#define checkImageHeight 256
static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

static GLuint checkerTex;

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
  qobj = gluNewQuadric();
  gluQuadricTexture(qobj, true);

  // spot 0
  GLfloat light_position0[] = {0.0, 0.0, 0.05, 1.0};
  GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_ambient0[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat light_attenuation0[] = {1, 0, 0};
  GLfloat spot_dir0[] = {0, 0, -1};
  GLfloat spot_cutoff0 = 90;
  GLfloat spot_exponent0 = 1;

  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, light_attenuation0[0]);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light_attenuation0[1]);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light_attenuation0[2]);
  glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_dir0);
  glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, spot_cutoff0);
  glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, spot_exponent0);

  // point 1
  GLfloat light_position1[] = {0.0, 0.0, 0.01, 1.0};
  GLfloat light_diffuse1[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular1[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_ambient1[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat light_attenuation1[] = {0, 1, 10};

  glLightfv(GL_LIGHT1, GL_POSITION, light_position1);
  glLightfv(GL_LIGHT1, GL_DIFFUSE, light_diffuse1);
  glLightfv(GL_LIGHT1, GL_SPECULAR, light_specular1);
  glLightfv(GL_LIGHT1, GL_AMBIENT, light_ambient1);
  glLightf(GL_LIGHT1, GL_CONSTANT_ATTENUATION, light_attenuation1[0]);
  glLightf(GL_LIGHT1, GL_LINEAR_ATTENUATION, light_attenuation1[1]);
  glLightf(GL_LIGHT1, GL_QUADRATIC_ATTENUATION, light_attenuation1[2]);

  GLfloat lmodel_ambient[] = {0, 0, 0, 1.0};  // global ambient
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
  glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

  GLfloat mat_ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_specular[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat mat_shininess = 2.0;

  // material
  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

  // texture
  makeCheckImage();
  glGenTextures(1, &checkerTex);
  glBindTexture(GL_TEXTURE_2D, checkerTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, checkImageWidth, checkImageWidth, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

  // fbo
  glGenTextures(1, &colorTex);
  glBindTexture(GL_TEXTURE_2D, colorTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexImage2D(
    GL_TEXTURE_2D, 0, GL_RGBA16F, 512, 512, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);

  glGenFramebuffers(1, &fbo);

  // hdr program
  program = glCreateProgram();
  attachShaderFile(program, GL_FRAGMENT_SHADER, "data/shader/hdr.fs.glsl");
  ZCGE(glLinkProgram(program));
  setUnifomLocation(&loc_method, program, "method");
  setUnifomLocation(&loc_exposure, program, "exposure");

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHT1);
}

void render() {
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(0);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, checkerTex);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  gluDisk(qobj, 0, 1, 32, 4);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  if (useHdr) {
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glFramebufferTexture2D(
      GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTex, 0);

    render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // tone map
    glUseProgram(program);
    ZCGE(glUniform1i(loc_method, method));
    ZCGE(glUniform1f(loc_exposure, exposure));

    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, colorTex);
    drawTexRect(-1, -1, 1, 1);
    glUseProgram(0);
  } else {
    render();
  }

  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 492);
  GLchar info[256];

  sprintf(info,
    "q : toggle hdr : %d\n"
    "w : toon map : %s\n"
    "eE : exposure : %.2f",
    useHdr, methods[method], exposure);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
  glEnable(GL_TEXTURE_2D);

  glFlush();
  ZCGEA;
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    glOrtho(-1, 1, -1 * (GLfloat)h / (GLfloat)w, 1 * (GLfloat)h / (GLfloat)w,
      -10.0, 10.0);
  else
    glOrtho(-1 * (GLfloat)w / (GLfloat)h, 1 * (GLfloat)w / (GLfloat)h, -1, 1,
      -10.0, 10.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    case 'q':
      useHdr ^= 1;
      glutPostRedisplay();
      break;
    case 'w':
      method = (method + 1) % 2;
      glutPostRedisplay();
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
