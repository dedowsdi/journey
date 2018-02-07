/*
 * copyright (c) 1993-2003, silicon graphics, inc.
 * all rights reserved
 *
 * permission to use, copy, modify, and distribute this software for any
 * purpose and without fee is hereby granted, provided that the above
 * copyright notice appear in all copies and that both the copyright
 * notice and this permission notice appear in supporting documentation,
 * and that the name of silicon graphics, inc. not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS" AND
 * WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL SILICON
 * GRAPHICS, INC.  BE LIABLE TO YOU OR ANYONE ELSE FOR ANY DIRECT,
 * SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER, INCLUDING WITHOUT LIMITATION, LOSS OF
 * PROFIT, LOSS OF USE, SAVINGS OR REVENUE, OR THE CLAIMS OF THIRD
 * PARTIES, WHETHER OR NOT SILICON GRAPHICS, INC.  HAS BEEN ADVISED OF
 * THE POSSIBILITY OF SUCH LOSS, HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE POSSESSION, USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 *
 * US government users restricted rights
 * use, duplication, or disclosure by the government is subject to
 * restrictions set forth in FAR 52.227.19(c)(2) or subparagraph
 * (c)(1)(ii) of the rights in technical data and computer software
 * clause at DFARS 252.227-7013 and/or in similar or successor clauses
 * in the FAR or the DOD or NASA FAR supplement.  unpublished - rights
 * reserved under the copyright laws of the united states.
 *
 * contractor/manufacturer is:
 *	silicon graphics, inc.
 *	1500 crittenden lane
 *	mountain view, CA  94043
 *	united state of america
 *
 * open_g_l(R) is a registered trademark of silicon graphics, inc.
 */

/*
 *  aaindex.c
 *  this program draws shows how to draw anti-aliased lines in color
 *  index mode. it draws two diagonal lines to form an X; when 'r'
 *  is typed in the window, the lines are rotated in opposite
 *  directions.
 */
#include "app.h"

namespace zxd {

#define RAMPSIZE 16
#define RAMP1START 32
#define RAMP2START 48

float rot_angle = 0.;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "aaindex";
    m_info.display_mode = GLUT_SINGLE | GLUT_INDEX;
    m_info.wnd_width = 200;
    m_info.wnd_height = 200;
  }

  /*  initialize antialiasing for color index mode,
   *  including loading a green color ramp starting
   *  at RAMP1START, and a blue color ramp starting
   *  at RAMP2START. the ramps must be a multiple of 16.
   */
  void create_scene(void) {
    int i;

    for (i = 0; i < RAMPSIZE; i++) {
      GLfloat shade;
      shade = (GLfloat)i / (GLfloat)RAMPSIZE;
      glutSetColor(RAMP1START + (GLint)i, 0., shade, 0.);
      glutSetColor(RAMP2START + (GLint)i, 0., 0., shade);
    }

    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_DONT_CARE);
    glLineWidth(1.5);

    glClearIndex((GLfloat)RAMP1START);
  }

  /*  draw 2 diagonal lines to form an X
   */
  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glIndexi(RAMP1START);
    glPushMatrix();
    glRotatef(-rot_angle, 0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2f(-0.5, 0.5);
    glVertex2f(0.5, -0.5);
    glEnd();
    glPopMatrix();

    glIndexi(RAMP2START);
    glPushMatrix();
    glRotatef(rot_angle, 0.0, 0.0, 0.1);
    glBegin(GL_LINES);
    glVertex2f(0.5, 0.5);
    glVertex2f(-0.5, -0.5);
    glEnd();
    glPopMatrix();

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      gluOrtho2D(-1.0, 1.0, -1.0 * (GLfloat)h / (GLfloat)w,
        1.0 * (GLfloat)h / (GLfloat)w);
    else
      gluOrtho2D(-1.0 * (GLfloat)w / (GLfloat)h, 1.0 * (GLfloat)w / (GLfloat)h,
        -1.0, 1.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      case 'r':
      case 'R':
        rot_angle += 20.;
        if (rot_angle >= 360.) rot_angle = 0.;
        break;
      default:
        break;
    }
  }

  /*  main loop
   *  open window with initial window size, title bar,
   *  color index display mode, and handle input events.
   */
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
