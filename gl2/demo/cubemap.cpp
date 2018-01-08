/*  cubemap.c
 *
 *  This program demonstrates cube map textures.
 *  Six different colored checker board textures are
 *  created and applied to a lit sphere. There is a sphere with cubemap texture
 *  in a skybox.
 *
 *  Note:
 *    GL_NORMAL_MAP works on eye space, it use:
 *      tex = inverseTranspose(modelView) * n
 *    no matter how you rotate your camera or object, the same
 *    normal in view space always retrieve the same color from cube map.
 *    If you want to get different image when you move your camera, you should
 *    reset cube map ? 
 *
 *    Object must has identity transform for cube env map to work?
 *
 */

#include "glad/glad.h"
#include <stdlib.h>
#include <stdio.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include "common.h"

GLuint skyboxSize = 50;

// clang-format off
  GLfloat vertices[][3] = {
    {-1,-1,1},  {-1,-1,1},
    {1,-1,1},   {1,-1,1},
    {1,1,1},    {1,1,1},
    {-1,1,1},   {-1,1,1},
    {-1,-1,-1}, {-1,-1,-1},
    {1,-1,-1},  {1,-1,-1},
    {1,1,-1},   {1,1,-1},
    {-1,1,-1},  {-1,1,-1}
  };
  GLubyte indices[][4] = {
    {0,1,2,3}, //front
    {1,5,6,2}, //right
    {3,2,6,7}, //top
    {4,0,3,7}, //left
    {4,5,1,0}, //bottom
    {7,6,5,4} //back
  };
// clang-format on

#define imageSize 4
static GLubyte image1[imageSize][imageSize][4];
static GLubyte image2[imageSize][imageSize][4];
static GLubyte image3[imageSize][imageSize][4];
static GLubyte image4[imageSize][imageSize][4];
static GLubyte image5[imageSize][imageSize][4];
static GLubyte image6[imageSize][imageSize][4];

static GLdouble ztrans = -20;
GLint yaw = 0, pitch = 0, yawObj = 0;  // yaw in world , pitch in view

GLuint vbo, ibo, tbo;
GLint polygonMode = GL_FILL;

void makeImages(void) {
  int i, j, c;

  for (i = 0; i < imageSize; i++) {
    for (j = 0; j < imageSize; j++) {
      c = ((((i & 0x1) == 0) ^ ((j & 0x1) == 0))) * 255;
      image1[i][j][0] = (GLubyte)c;
      image1[i][j][1] = (GLubyte)c;
      image1[i][j][2] = (GLubyte)c;
      image1[i][j][3] = (GLubyte)255;

      image2[i][j][0] = (GLubyte)c;
      image2[i][j][1] = (GLubyte)c;
      image2[i][j][2] = (GLubyte)0;
      image2[i][j][3] = (GLubyte)255;

      image3[i][j][0] = (GLubyte)c;
      image3[i][j][1] = (GLubyte)0;
      image3[i][j][2] = (GLubyte)c;
      image3[i][j][3] = (GLubyte)255;

      image4[i][j][0] = (GLubyte)0;
      image4[i][j][1] = (GLubyte)c;
      image4[i][j][2] = (GLubyte)c;
      image4[i][j][3] = (GLubyte)255;

      image5[i][j][0] = (GLubyte)255;
      image5[i][j][1] = (GLubyte)c;
      image5[i][j][2] = (GLubyte)c;
      image5[i][j][3] = (GLubyte)255;

      image6[i][j][0] = (GLubyte)c;
      image6[i][j][1] = (GLubyte)c;
      image6[i][j][2] = (GLubyte)255;
      image6[i][j][3] = (GLubyte)255;
    }
  }
}

void createSkyBox() {
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  // array buffer
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  // index buffer
  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(
    GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
}

void init(void) {
  /*glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);*/
  GLfloat diffuse[4] = {1.0, 1.0, 1.0, 1.0};

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glShadeModel(GL_SMOOTH);

  makeImages();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, imageSize,
    imageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, image1);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, imageSize,
    imageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, image4);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, imageSize,
    imageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, image2);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, imageSize,
    imageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, image5);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, imageSize,
    imageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, image3);
  glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, imageSize,
    imageSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, image6);

  // generate s,t,r texture, use normal in view space as texcoord
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glEnable(GL_TEXTURE_CUBE_MAP);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_AUTO_NORMAL);
  glEnable(GL_NORMALIZE);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

  createSkyBox();
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glPolygonMode(GL_FRONT_AND_BACK, polygonMode);
  glPushMatrix();

  glTranslatef(0.0, 0.0, ztrans);
  glRotatef(pitch, 1, 0, 0);
  glRotatef(yaw, 0, 1, 0);

  GLdouble modelView[16];
  glGetDoublev(GL_MODELVIEW_MATRIX, modelView);

  // draw skybox
  glColor3f(1.0f, 1.0f, 1.0f);
  glDisable(GL_TEXTURE_GEN_S);
  glDisable(GL_TEXTURE_GEN_T);
  glDisable(GL_TEXTURE_GEN_R);
  glPushMatrix();
  glDisable(GL_LIGHTING);
  glScalef(skyboxSize, skyboxSize, skyboxSize);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glVertexPointer(3, GL_FLOAT, 6 * 4, BUFFER_OFFSET(0));
  // tex coord buffer
  glTexCoordPointer(3, GL_FLOAT, 6 * 4, BUFFER_OFFSET(3 * 4));
  glDrawElements(GL_QUADS, 24, GL_UNSIGNED_BYTE, BUFFER_OFFSET(0));
  glEnable(GL_LIGHTING);
  glPopMatrix();
  ZCGEA

  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);

  // draw scene

  glPushMatrix();
  glRotatef(yawObj, 0, 1, 0);

  glMatrixMode(GL_TEXTURE);
  glPushMatrix();
  /*glLoadTransposeMatrixd(modelView);*/
  /*glRotatef(-yaw, 0, 1, 0);*/
  /*glRotatef(-pitch, 1, 0, 0);*/
  glutSolidSphere(5.0, 40, 20);
  /*glutSolidTeapot(5);*/
  /*glutSolidCube(5);*/
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);

  glPopMatrix();

  // draw scene

  glPopMatrix();

  glDisable(GL_TEXTURE_CUBE_MAP);
  glDisable(GL_LIGHTING);
  glColor3f(0.3f, 0.3f, 0.3f);
  glWindowPos2i(20, 480);
  const GLubyte info[] =
    "q : use GL_NORMAL_MAP generate s,t,r\n"
    "w : use GL_REFLECTION_MAP to generate s,t,r\n"
    "eE : change skybox size\n"
    "rR : change camera distance\n"
    "uU : yaw world\n"
    "iI : pitch camera\n"
    "oO : yaw object\n"
    "pP : polygon mode\n";
  glutBitmapString(GLUT_BITMAP_9_BY_15, info);
  glEnable(GL_LIGHTING);
  glEnable(GL_TEXTURE_CUBE_MAP);

  glutSwapBuffers();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(40.0, (GLfloat)w / (GLfloat)h, 1.0, 800.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 'q':
    case 'Q':
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
      glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_NORMAL_MAP);
      glutPostRedisplay();
      break;
    case 'w':
    case 'W':
      glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
      glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
      glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
      glutPostRedisplay();
      break;
    case 'e':
      skyboxSize += 5;
      glutPostRedisplay();
      break;
    case 'E':
      if (skyboxSize > 5) {
        skyboxSize -= 5;
        glutPostRedisplay();
      }
      break;
    case 'r':
      ztrans = ztrans - 0.2;
      glutPostRedisplay();
      break;
    case 'R':
      ztrans = ztrans + 0.2;
      glutPostRedisplay();
      break;
    case 'u':
      yaw += 5;
      glutPostRedisplay();
      break;
    case 'U':
      yaw -= 5;
      glutPostRedisplay();
      break;
    case 'i':
      pitch += 5;
      glutPostRedisplay();
      break;
    case 'I':
      pitch -= 5;
      glutPostRedisplay();
      break;
    case 'o':
      yawObj += 5;
      glutPostRedisplay();
      break;
    case 'O':
      yawObj -= 5;
      glutPostRedisplay();
      break;
    case 'p': {
      polygonMode = GL_POINT + (polygonMode + 1 - GL_POINT) % 3;
      glutPostRedisplay();
    } break;
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
