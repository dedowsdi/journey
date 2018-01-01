/* shadowmap.c
 *
 * This is directional shadow map, only works for directional light.
 *
 * create shadow texture:
 *   use ortho projection to create shadow map for dir light
 *
 *   record depthBSVP = bias * scale * lightProj * lightView
 *
 *   bias and scale are used to transform ndc to [0,1]
 *
 *   copy depth texture to iamge.
 *
 * use shadow texture:
 *
 *  (s,t,r,q)^t = bias * scale * lightProj * lightView * model * objVertex
 *
 *  method 1: generate in object space
 *
 *    use glTexGen to generate above tex in object space.
 *    As every renderable has it's own model matrix, you need to set different
 *    texgen for every renderable, it's tedious.
 *
 *  method 2: generate in eye space
 *    (s,t,r,q) = bias * scale * lightProj * lightView * worldVertex
 *              = bias * scale * lightProj * lightView * invCameraView *
 * cameraVertex
 *    As glTexGen in eye space use following equation:
 *
 *        p^t * invModelView * eyeVertex
 *
 *    we must call glTexGen after camera is in position, but before reaching
 *    renderable transform.
 *
 *
 *   In both method, after we obtain the preceding matrix M, call:
 *     glTexGen(GL_S, ... , row0 of M)
 *     glTexGen(GL_T, ... , row1 of M)
 *     glTexGen(GL_R, ... , row2 of M)
 *     glTexGen(GL_Q, ... , row3 of M)
 *
 * reult:
 *
 *   cmpResult =  r/q <= tex(s/q, t/q) ? 1 : 0
 *   according to depth texture mode,  cmpResult will be  mutiplied relevant
 *   component in fragment..
 *
 *   set GL_TEXTURE_COMPARE_MODE to GL_COMPARE_R_TO_TEXTURE
 *   set GL_TEXTURE_COMPARE_FUNC to GL_LEQUAL
 *   Set GL_DEPTH_TEXTURE_MODE to GL_LUMINANCE or GL_INTENSITY
 *
 *   issue : cast shadow on self.
 *
 */

#include "glad/glad.h"

#include "glad/glad.h"
#include <stdio.h>
#include <GL/freeglut.h>
#include <math.h>
#include <GL/freeglut_ext.h>
#define SHADOW_MAP_WIDTH 256
#define SHADOW_MAP_HEIGHT 256
#include "common.h"

#define PI 3.14159265359

GLdouble fovy = 60.0;
GLdouble nearPlane = 10.0;
GLdouble farPlane = 100.0;
GLdouble lightAspect = 1.0, lightNear = 1, lightFar = 80;
GLdouble lightTop = 8;
GLdouble depthBSVP[16];  // depth bias * scale * proj * view
GLdouble matView[16];

GLfloat angle = 0.0;
GLfloat torusAngle = 0.0;

GLfloat lightPos[] = {25.0, 25.0, 25.0, 1.0};
GLfloat lookat[] = {0.0, 0.0, 0.0};
GLfloat up[] = {0.0, 0.0, 1.0};  // gound on xy plane

GLboolean showShadow = GL_FALSE;
GLboolean alignCameraToLight = GL_FALSE;

void init(void) {
  glClearColor(0.0, 0.5, 1.0, 0.0);
  GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

  // init light
  glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
  glLightfv(GL_LIGHT0, GL_SPECULAR, white);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, white);

  glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

  // set up shadow map
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH,
    SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  // specify depth texture behavior
  glTexParameteri(
    GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);

  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, GL_EYE_LINEAR);

  glCullFace(GL_BACK);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHT0);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_2D);
  glEnable(GL_COLOR_MATERIAL);
  glEnable(GL_CULL_FACE);
  /*glEnable(GL_NORMALIZE);*/
  ZCGEA;
}

void reshape(int width, int height) {
  glViewport(0, 0, width, height);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(fovy, (GLdouble)width / height, nearPlane, farPlane);
  lightAspect = (GLdouble)width / height;
  glMatrixMode(GL_MODELVIEW);
  glTranslatef(0, 0, -30);
}

void idle(void) {
  angle += PI / 10000;
  torusAngle += .1;
  glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27: /* Escape */
      exit(0);
      break;

    case 'q': {
      static GLboolean textureOn = GL_TRUE;
      textureOn = !textureOn;
      if (textureOn)
        glEnable(GL_TEXTURE_2D);
      else
        glDisable(GL_TEXTURE_2D);
    } break;

    case 'w': {
      static GLboolean compareMode = GL_TRUE;
      compareMode = !compareMode;
      printf("Compare mode %s\n", compareMode ? "On" : "Off");
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
        compareMode ? GL_COMPARE_R_TO_TEXTURE : GL_NONE);
    } break;

    case 'e': {
      static GLboolean funcMode = GL_TRUE;
      funcMode = !funcMode;
      printf("Operator %s\n", funcMode ? "GL_LEQUAL" : "GL_GEQUAL");
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC,
        funcMode ? GL_LEQUAL : GL_GEQUAL);
    } break;

    case 'r':
      showShadow = !showShadow;
      break;

    case 'u': {
      int i;
      glGetTexParameteriv(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, &i);
      if (i == GL_LUMINANCE) {
        printf("GL_DEPTH_TEXTURE_MODE : INTENSITY\n");
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
      } else if (i == GL_INTENSITY) {
        printf("GL_DEPTH_TEXTURE_MODE : ALPHA\n");
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_ALPHA);
      } else {
        printf("GL_DEPTH_TEXTURE_MODE : LUMINANCE\n");
        glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
      }
    } break;
    case 'i':
      lightTop += 0.5;
      glutPostRedisplay();
      break;

    case 'I':
      lightTop -= 0.5;
      glutPostRedisplay();
      break;

    case 'j':
      lightNear += 0.5;
      glutPostRedisplay();
      break;

    case 'J':
      lightNear -= 0.5;
      glutPostRedisplay();
      break;

    case 'k':
      lightFar += 0.5;
      glutPostRedisplay();
      break;

    case 'K':
      lightFar -= 0.5;
      glutPostRedisplay();
      break;

    case 'o': {
      static GLboolean animate = GL_TRUE;
      animate = !animate;
      glutIdleFunc(animate ? idle : NULL);
    } break;
    case 'p': {
      alignCameraToLight = !alignCameraToLight;
      glutPostRedisplay();
    } break;
    default:
      break;
  }

  glutPostRedisplay();
}

void transposeMatrix(GLdouble m[16]) {
  GLdouble tmp;
#define Swap(a, b) \
  tmp = a;         \
  a = b;           \
  b = tmp
  Swap(m[1], m[4]);
  Swap(m[2], m[8]);
  Swap(m[3], m[12]);
  Swap(m[6], m[9]);
  Swap(m[7], m[13]);
  Swap(m[11], m[14]);
#undef Swap
}

void setupTexGen() {
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glEnable(GL_TEXTURE_GEN_Q);

  transposeMatrix(depthBSVP);
  glTexGendv(GL_S, GL_EYE_PLANE, &depthBSVP[0]);
  glTexGendv(GL_T, GL_EYE_PLANE, &depthBSVP[4]);
  glTexGendv(GL_R, GL_EYE_PLANE, &depthBSVP[8]);
  glTexGendv(GL_Q, GL_EYE_PLANE, &depthBSVP[12]);
}

void drawObjects(GLboolean shadowRender) {
  GLboolean textureOn = glIsEnabled(GL_TEXTURE_2D);

  // avoid shadow map take effect on 1st pass depth buffer
  if (shadowRender) glDisable(GL_TEXTURE_2D);

  if (!shadowRender) {
    // draw ground
    glNormal3f(0, 0, 1);
    glColor3f(1, 1, 1);
    glRectf(-30.0, -30.0, 30.0, 30.0);
  }
  // get model matrix

  glPushMatrix();
  glTranslatef(11, 11, 11);
  glRotatef(54.73, -5, 5, 0);
  glRotatef(torusAngle, 1, 0, 0);
  glColor3f(1, 0, 0);
  glutSolidTorus(1, 4, 8, 36);
  glPopMatrix();

  glPushMatrix();
  glTranslatef(2, 2, 2);
  glColor3d(0, 0, 1);
  glutSolidCube(4);
  glPopMatrix();

  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  glDisable(GL_TEXTURE_GEN_Q);

  // draw light
  glPushMatrix();
  glTranslatef(lightPos[0], lightPos[1], lightPos[2]);
  glColor3f(0, 1, 0);
  glutWireSphere(0.5, 6, 6);
  glPopMatrix();

  if (shadowRender && textureOn) glEnable(GL_TEXTURE_2D);
}

void generateShadowMap(void) {
  GLint viewport[4];

  // set viewport size to shadow map size
  glGetIntegerv(GL_VIEWPORT, viewport);
  glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDisable(GL_LIGHTING);
  glCullFace(GL_FRONT);
  /*glDrawBuffer(GL_NONE);*/
  /*glEnable(GL_POLYGON_OFFSET_FILL);*/
  /*glPolygonOffset(7, 1);*/

  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();

  // set up light view and projection
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glOrtho(-lightTop * lightAspect, lightTop * lightAspect, -lightTop, lightTop,
    lightNear, lightFar);

  gluLookAt(lightPos[0], lightPos[1], lightPos[2], lookat[0], lookat[1],
    lookat[2], up[0], up[1], up[2]);
  glGetDoublev(GL_MODELVIEW_MATRIX, depthBSVP);

  // render scene, generate depth buffer
  drawObjects(GL_TRUE);

  // get bias * scale * lightProj * lightView
  glLoadIdentity();
  glTranslated(0.5f, 0.5f, 0.5f);
  glScaled(0.5f, 0.5f, 0.5f);
  glMultMatrixd(depthBSVP);
  glGetDoublev(GL_MODELVIEW_MATRIX, depthBSVP);

  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  // copy depth buffer to shadow map
  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, SHADOW_MAP_WIDTH,
    SHADOW_MAP_HEIGHT, 0);

  glEnable(GL_LIGHTING);
  glCullFace(GL_BACK);
  /*glDrawBuffer(GL_BACK_LEFT);*/
  /*glPolygonOffset(0, 0);*/
  glDisable(GL_POLYGON_OFFSET_FILL);

  // restore viewport
  glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

  // draw shadow image
  if (showShadow) {
    int et2 = glIsEnabled(GL_TEXTURE_2D);
    if (et2) glDisable(GL_TEXTURE_2D);
    // depth
    GLfloat depthImage[SHADOW_MAP_WIDTH][SHADOW_MAP_HEIGHT];
    glReadPixels(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT,
      GL_FLOAT, depthImage);
    glWindowPos2f(viewport[2] / 2, 0);

    glDrawPixels(SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_DEPTH_COMPONENT,
      GL_FLOAT, depthImage);

    // alpha
    glReadPixels(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_ALPHA, GL_FLOAT,
      depthImage);
    glWindowPos2f(viewport[2] / 2, viewport[3] / 2);
    glDrawPixels(
      SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_ALPHA, GL_FLOAT, depthImage);

    // color
    GLfloat colorImage[SHADOW_MAP_WIDTH][SHADOW_MAP_HEIGHT][3];
    glReadPixels(
      0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_RGB, GL_FLOAT, colorImage);
    glWindowPos2f(0, viewport[3] / 2);
    glDrawPixels(
      SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, GL_RGB, GL_FLOAT, colorImage);

    if (et2) glEnable(GL_TEXTURE_2D);
    glutSwapBuffers();
  }
}

void display(void) {
  ZCGEA;
  GLfloat radius = 30;

  generateShadowMap();

  if (showShadow) return;

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glPushMatrix();
  glLoadIdentity();
  if (alignCameraToLight) {
    gluLookAt(lightPos[0], lightPos[1], lightPos[2], lookat[0], lookat[1],
      lookat[2], up[0], up[1], up[2]);
  } else {
    gluLookAt(radius * cos(angle), radius * sin(angle), 30, lookat[0],
      lookat[1], lookat[2], up[0], up[1], up[2]);
  }
  // render scene with shadow map
  setupTexGen();
  drawObjects(GL_FALSE);
  glPopMatrix();

  glColor3f(1.0f, 0.0f, 0.0f);
  glWindowPos2i(10, 492);
  char info[512];
  sprintf(info,
    "q : toggle GL_TEXTURE_2D\n"
    "w : toggle GL_TEXTURE_COMPARE_MODE\n"
    "e : toggle GL_TEXTURE_COMPARE_FUNC\n"
    "r : toggle scene depth alpha color views(ccw)\n"
    "u : toggle GL_DEPTH_TEXTURE_MODE\n"
    "iI: lightTop : %.2f\n"
    "jJ: lightNear : %.2f\n"
    "KK: lightFar : %.2f\n"
    "o : toggle animation\n"
    "o : toggle animation\n"
    "p : align camera to light\n",
    lightTop, lightNear, lightFar);
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glutSwapBuffers();
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
  glutInitWindowSize(512, 512);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);

  loadGL();
  init();

  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);

  glutMainLoop();

  return 0;
}
