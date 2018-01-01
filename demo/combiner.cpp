/*  combiner.c
 *  This program renders a variety of quads showing different
 *  effects of texture combiner functions.
 *
 *  The first row renders an untextured polygon (so you can
 *  compare the fragment colors) and then the 2 textures.
 *  The second row shows several different combiner functions
 *  on a single texture:  replace, modulate, add, add-signed,
 *  and subtract.
 *  The third row shows the interpolate combiner function
 *  on a single texture with a constant color/alpha value,
 *  varying the amount of interpolation.
 *  The fourth row uses multitexturing with two textures
 *  and different combiner functions.
 *  The fifth row are some combiner experiments:  using the
 *  scaling factor and reversing the order of subtraction
 *  for a combination function.
 *
 *  rgb = combineFunc(arg0, arg1, arg2) * RGB_SCALE
 *  alpha = combineFunc(arg0, arg1, arg2) * alpha_SCALE
 *
 *  RGB_SCALE or ALPHA_SCALE must be 1, 2 or 4.
 *
 *  combineFunc must be specified with glTexEnv(GL_TEX_ENV, GL_COMBINE_, ...)
 *
 *  arg0, arg1, arg2 of rgb must be specified with glTexEnv(GL_TEX_ENV,
 *  GL_SRCi_RGB, param), param can be GL_TEXTURE(current texel), GL_TEXTUREn,
 *  GL_CONSTANT(GL_TEXTURE_ENV_COLOR), GL_PRIMARY_COLOR(fragment color),
 *  GL_PREVIOUS(last texture unit output, it's the same as fragment color if
 *  current tex unit is the 1st one)
 *
 *  operand of arg must be specified with glTexEnv(GL_TEX_ENV, GL_OPERANDi_, ...)
 *
 */
#include "glad/glad.h"
#include <GL/freeglut.h>
#include <stdlib.h>
#include <stdio.h>
#include "common.h"

#define imageWidth 8
#define imageHeight 8
/*     arrays for two textures	    */
static GLubyte image0[imageHeight][imageWidth]
                     [4];  // white black horizontal strips
static GLubyte image1[imageHeight][imageWidth][4];  // cyan black vertical
                                                    // strips

static GLuint texName[4];

void makeImages(void) {
  int i, j, c;
  for (i = 0; i < imageHeight; i++) {
    for (j = 0; j < imageWidth; j++) {
      c = ((i & 2) == 0) * 255; /*  horiz b & w stripes  */
      image0[i][j][0] = (GLubyte)c;
      image0[i][j][1] = (GLubyte)c;
      image0[i][j][2] = (GLubyte)c;
      image0[i][j][3] = (GLubyte)255;
      c = ((j & 4) != 0) * 128; /*  wider vertical 50% cyan and black stripes */
      image1[i][j][0] = (GLubyte)0;
      image1[i][j][1] = (GLubyte)c;
      image1[i][j][2] = (GLubyte)c;
      image1[i][j][3] = (GLubyte)255;
    }
  }
}

void init(void) {

  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  makeImages();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  glGenTextures(4, texName);

  glBindTexture(GL_TEXTURE_2D, texName[0]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, image0);

  glBindTexture(GL_TEXTURE_2D, texName[1]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, imageWidth, imageHeight, 0, GL_RGBA,
    GL_UNSIGNED_BYTE, image1);

  /*  smooth-shaded polygon with multiple texture coordinates  */
  glNewList(1, GL_COMPILE);
  glBegin(GL_QUADS);
  glMultiTexCoord2f(GL_TEXTURE0, 0.0, 0.0);
  glMultiTexCoord2f(GL_TEXTURE1, 0.0, 0.0);
  glColor3f(0.5, 1.0, 0.25);
  glVertex3f(0.0, 0.0, 0.0);
  glMultiTexCoord2f(GL_TEXTURE0, 0.0, 2.0);
  glMultiTexCoord2f(GL_TEXTURE1, 0.0, 2.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(0.0, 1.0, 0.0);
  glMultiTexCoord2f(GL_TEXTURE0, 2.0, 2.0);
  glMultiTexCoord2f(GL_TEXTURE1, 2.0, 2.0);
  glColor3f(1.0, 1.0, 1.0);
  glVertex3f(1.0, 1.0, 0.0);
  glMultiTexCoord2f(GL_TEXTURE0, 2.0, 0.0);
  glMultiTexCoord2f(GL_TEXTURE1, 2.0, 0.0);
  glColor3f(1.0, 0.5, 0.25);
  glVertex3f(1.0, 0.0, 0.0);
  glEnd();
  glEndList();
}

void display(void) {
  static GLfloat constColor[4] = {
    0.0, 0.0, 0.0, 0.0}; /*  for use as constant texture color  */

  glClear(GL_COLOR_BUFFER_BIT);

  glDisable(GL_TEXTURE_2D); /*  untextured polygon--see the "fragment" colors */
  glPushMatrix();
  glTranslatef(0.0, 5.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glEnable(GL_TEXTURE_2D);
  /*  draw ordinary textured polys; 1 texture unit; combine mode disabled  */
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, texName[0]);
  glTranslatef(1.0, 5.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glPushMatrix();
  glBindTexture(GL_TEXTURE_2D, texName[1]);
  glTranslatef(2.0, 5.0, 0.0);
  glCallList(1);
  glPopMatrix();

  /*  different combine modes enabled; 1 texture unit
   *  defaults are:
   *  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
   */
  glBindTexture(GL_TEXTURE_2D, texName[0]);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  glPushMatrix();
  glTranslatef(1.0, 4.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  //GL_PREVIOUS works the same as GL_PRIMARY_COLOR here, as there are only 1 texture unit
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS); 
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  glPushMatrix();
  glTranslatef(2.0, 4.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
  glPushMatrix();
  glTranslatef(3.0, 4.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);
  glPushMatrix();
  glTranslatef(4.0, 4.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_SUBTRACT);
  glPushMatrix();
  glTranslatef(5.0, 4.0, 0.0);
  glCallList(1);
  glPopMatrix();

  /*  interpolate combine with constant color; 1 texture unit
   *  use different alpha values for constant color
   *  defaults are:
   *  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
   */
  constColor[3] = 0.2; //this alpha will be used as arg2
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constColor);
  glBindTexture(GL_TEXTURE_2D, texName[0]);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_INTERPOLATE);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE2_RGB, GL_CONSTANT);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND2_RGB, GL_SRC_ALPHA);
  glPushMatrix();
  glTranslatef(1.0, 3.0, 0.0);
  glCallList(1);
  glPopMatrix();

  constColor[3] = 0.4;
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constColor);
  glPushMatrix();
  glTranslatef(2.0, 3.0, 0.0);
  glCallList(1);
  glPopMatrix();

  constColor[3] = 0.6;
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constColor);
  glPushMatrix();
  glTranslatef(3.0, 3.0, 0.0);
  glCallList(1);
  glPopMatrix();

  constColor[3] = 0.8;
  glTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, constColor);
  glPushMatrix();
  glTranslatef(4.0, 3.0, 0.0);
  glCallList(1);
  glPopMatrix();

  /*  combine textures 0 & 1
   *  defaults are:
   *  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
   *  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
   */

  glActiveTexture(GL_TEXTURE0);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texName[0]);
  //reset tex env mode of tex unit 0 to default
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glActiveTexture(GL_TEXTURE1);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, texName[1]);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);

  /*int i;*/
  /*glGetTexEnviv(GL_TEXTURE_ENV, GL_SOURCE0_RGB, &i);*/
  /*printf("SOURCE0_RGB : %x\n", i);*/
  /*glGetTexEnviv(GL_TEXTURE_ENV, GL_OPERAND0_RGB, &i);*/
  /*printf("OPERAND0_RGB : %x\n\n", i);*/
  /*glGetTexEnviv(GL_TEXTURE_ENV, GL_SOURCE1_RGB, &i);*/
  /*printf("SOURCE1_RGB : %x\n", i);*/
  /*glGetTexEnviv(GL_TEXTURE_ENV, GL_OPERAND1_RGB, &i);*/
  /*printf("OPERAND1_RGB : %x\n", i);*/

  // reset source and operand to default.(which will be changed later in
  // display)
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_PREVIOUS);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);

  glPushMatrix();
  glTranslatef(1.0, 2.0, 0.0);
  glCallList(1);
  glPopMatrix();

  /*  try different combiner modes of texture unit 1  */
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  glPushMatrix();
  glTranslatef(2.0, 2.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD);
  glPushMatrix();
  glTranslatef(3.0, 2.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD_SIGNED);
  glPushMatrix();
  glTranslatef(4.0, 2.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_SUBTRACT);
  glPushMatrix();
  glTranslatef(5.0, 2.0, 0.0);
  glCallList(1);
  glPopMatrix();

  /*  some experiments */

  /*  see the effect of RGB_SCALE   */
  glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 2.0);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_REPLACE);
  glPushMatrix();
  glTranslatef(1.0, 1.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);
  glPushMatrix();
  glTranslatef(2.0, 1.0, 0.0);
  glCallList(1);
  glPopMatrix();
  glTexEnvf(GL_TEXTURE_ENV, GL_RGB_SCALE, 1.0);

  /*  using SOURCE0 and SOURCE1, reverse the order of subtraction Arg1-Arg0  */

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
  glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_SUBTRACT);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND0_RGB, GL_SRC_COLOR);
  glTexEnvf(GL_TEXTURE_ENV, GL_SOURCE1_RGB, GL_TEXTURE);
  glTexEnvf(GL_TEXTURE_ENV, GL_OPERAND1_RGB, GL_SRC_COLOR);
  glPushMatrix();
  glTranslatef(5.0, 1.0, 0.0);
  glCallList(1);
  glPopMatrix();

  glActiveTexture(GL_TEXTURE1); /*  deactivate multitexturing  */
  glDisable(GL_TEXTURE_2D);
  glActiveTexture(GL_TEXTURE0); /*  activate single texture unit  */

  glFlush();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0.0, 7.0, 0.0, 7.0);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;
    default:
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(400, 400);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
