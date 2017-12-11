/*  texgen.c
 *  This program draws a texture mapped teapot with
 *  automatically generated texture coordinates.  The
 *  texture is rendered as stripes on the teapot.
 *  Initially, the object is drawn with texture coordinates
 *  based upon the object coordinates of the vertex
 *  and distance from the plane x = 0.  Pressing the 'e'
 *  key changes the coordinate generation to eye coordinates
 *  of the vertex.  Pressing the 'o' key switches it back
 *  to the object coordinates.  Pressing the 's' key
 *  changes the plane to a slanted one (x + y + z = 0).
 *  Pressing the 'x' key switches it back to x = 0.
 *
 *  GL_OBJECT_LINEAR:
 *    s = objectPlane∙objVertex
 *
 *  GL_EYE_LINEAR
 *    s = eyePlane*invMv(modelView when glTexGen called)∙eyeVertex
 *
 *    you can set modelView to identity to generate tex in eyespace
 *    you can set modelView to View,
   *      s = eyePlane * invView *  eyeVertex
 *          = eyePlane * worldVertex
 *        which means tex generated in world space
 *     don't set modelView to be the same as rederable modelView, it'll degraded
 *     to object linear.
 *
 */

#define GL_GLEXT_PROTOTYPES
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut_ext.h>

#define stripeImageWidth 32
GLubyte stripeImage[4 * stripeImageWidth];

#ifdef GL_VERSION_1_1
static GLuint texName;
#endif

// 1 dimension,  4 red + 28 green
void makeStripeImage(void) {
  int j;

  for (j = 0; j < stripeImageWidth; j++) {
    stripeImage[4 * j] = (GLubyte)((j <= 4) ? 255 : 0);
    stripeImage[4 * j + 1] = (GLubyte)((j > 4) ? 255 : 0);
    stripeImage[4 * j + 2] = (GLubyte)0;
    stripeImage[4 * j + 3] = (GLubyte)255;
  }
}

/*  planes for texture coordinate generation  */
static GLfloat yzPlane[] = {1.0, 0.0, 0.0, 0.0};
static GLfloat slanted[] = {1.0, 1.0, 1.0, 0.0};
static GLfloat* currentCoeff;
static GLenum currentPlane;
static GLint currentGenMode;

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  makeStripeImage();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

#ifdef GL_VERSION_1_1
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_1D, texName);
#endif
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#ifdef GL_VERSION_1_1
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, stripeImageWidth, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, stripeImage);
#else
  glTexImage1D(GL_TEXTURE_1D, 0, 4, stripeImageWidth, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, stripeImage);
#endif

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  currentCoeff = yzPlane;
  currentGenMode = GL_OBJECT_LINEAR;
  currentPlane = GL_OBJECT_PLANE;
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, currentGenMode);
  glTexGenfv(GL_S, currentPlane, currentCoeff);

  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_1D);
  glEnable(GL_CULL_FACE);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  glFrontFace(GL_CW);  // why???????????
  glMaterialf(GL_FRONT, GL_SHININESS, 64.0);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  glRotatef(45.0, 0.0, 0.0, 1.0);
#ifdef GL_VERSION_1_1
  glBindTexture(GL_TEXTURE_1D, texName);
#endif
  glutSolidTeapot(2.0);
  glPopMatrix();

  glDisable(GL_TEXTURE_1D);
  glDisable(GL_LIGHTING);
  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(20, 580);
  const GLubyte info[] =
    "q : set mode to GL_EYE_LINEAR, plane to GL_EYE_PLANE\n"
    "w : set mode to GL_OBJECT_LINEAR, plane to GL_OBJECT_PLANE\n"
    "e : set current plane to x + y + z = 0\n"
    "r : set current plane to x = 0";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_1D);

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    glOrtho(-3.5, 3.5, -3.5 * (GLfloat)h / (GLfloat)w,
      3.5 * (GLfloat)h / (GLfloat)w, -3.5, 3.5);
  else
    glOrtho(-3.5 * (GLfloat)w / (GLfloat)h, 3.5 * (GLfloat)w / (GLfloat)h, -3.5,
      3.5, -3.5, 3.5);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q':
    case 'Q':
      //modelView is identity right now
      currentGenMode = GL_EYE_LINEAR;
      currentPlane = GL_EYE_PLANE;
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, currentGenMode);
      glTexGenfv(GL_S, currentPlane, currentCoeff);
      glutPostRedisplay();
      break;
    case 'w':
    case 'W':
      currentGenMode = GL_OBJECT_LINEAR;
      currentPlane = GL_OBJECT_PLANE;
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, currentGenMode);
      glTexGenfv(GL_S, currentPlane, currentCoeff);
      glutPostRedisplay();
      break;
    case 'e':
    case 'E':
      currentCoeff = slanted;
      glTexGenfv(GL_S, currentPlane, currentCoeff);
      glutPostRedisplay();
      break;
    case 'r':
    case 'R':
      currentCoeff = yzPlane;
      glTexGenfv(GL_S, currentPlane, currentCoeff);
      glutPostRedisplay();
      break;
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(600, 600);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
