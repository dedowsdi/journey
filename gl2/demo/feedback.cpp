/*
 * feedback.c
 * this program demonstrates use of open_g_l feedback.  first,
 * a lighting environment is set up and a few lines are drawn.
 * then feedback mode is entered, and the same lines are
 * drawn.  the results in the feedback buffer are printed.
 *
 * in both feedback and selection modes, infomation about objects is returned
 * after fragment shader ?, but prior to any fragment tests.
 *
 * point          GL_POINT_TOKEN               vertex(in window coordinate)
 * line           GL_LINE_TOKEN                vertex vertex
 *                GL_LINE_REST_TOKEN           vertex vertex
 * polygon        GL_POLYGON_TOKEN             vertex.....vertex (n)
 * bitmap         GL_BITMAP_TOKEN              vertex(raster position)
 * pixel rect     GL_DRAW_PIXEL_TOKEN          vertex
 *                GL_COPY_PIXEL_TOKEN          vertex
 * pass through   GL_PASS_THROUGH_TOKEN        user defined float number
 *
 */
#include "app.h"

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "feedback";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 100;
    m_info.wnd_height = 100;
  }

  /*  initialize lighting.
   */
  void create_scene(void) {
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
  }

  /* draw a few lines and two points, one of which will
   * be clipped.  if in feedback mode, a passthrough token
   * is issued between the each primitive.
   */
  void draw_geometry(GLenum mode) {
    glBegin(GL_LINE_STRIP);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(30.0, 30.0, 0.0);
    glVertex3f(50.0, 60.0, 0.0);
    glVertex3f(70.0, 40.0, 0.0);
    glEnd();
    if (mode == GL_FEEDBACK) glPassThrough(1.0);
    glBegin(GL_POINTS);
    glVertex3f(-100.0, -100.0, -100.0); /*  will be clipped  */
    glEnd();
    if (mode == GL_FEEDBACK) glPassThrough(2.0);
    glBegin(GL_POINTS);
    glNormal3f(0.0, 0.0, 1.0);
    glVertex3f(50.0, 50.0, 0.0);
    glEnd();
  }

  /* write contents of one vertex to stdout.	*/
  void print3_dcolor_vertex(GLint size, GLint *count, GLfloat *buffer) {
    int i;

    printf("  ");
    for (i = 0; i < 7; i++) {
      printf("%4.2f ", buffer[size - (*count)]);
      *count = *count - 1;
    }
    printf("\n");
  }

  /*  write contents of entire buffer.  (parse tokens!)	*/
  void print_buffer(GLint size, GLfloat *buffer) {
    GLint count;  // number of unprocessed data in buffer
    GLfloat token;

    count = size;
    while (count) {
      token = buffer[size - count];
      count--;
      if (token == GL_PASS_THROUGH_TOKEN) {
        printf("GL_PASS_THROUGH_TOKEN\n");
        printf("  %4.2f\n", buffer[size - count]);
        count--;
      } else if (token == GL_POINT_TOKEN) {
        printf("GL_POINT_TOKEN\n");
        print3_dcolor_vertex(size, &count, buffer);
      } else if (token == GL_LINE_TOKEN) {
        printf("GL_LINE_TOKEN\n");
        print3_dcolor_vertex(size, &count, buffer);
        print3_dcolor_vertex(size, &count, buffer);
      } else if (token == GL_LINE_RESET_TOKEN) {
        printf("GL_LINE_RESET_TOKEN\n");
        print3_dcolor_vertex(size, &count, buffer);
        print3_dcolor_vertex(size, &count, buffer);
      }
    }
  }

  void display(void) {
    GLfloat feed_buffer[1024];
    GLint size;

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0, 100.0, 0.0, 100.0, 0.0, 1.0);

    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT);
    draw_geometry(GL_RENDER);

    glFeedbackBuffer(1024, GL_3D_COLOR, feed_buffer);
    (void)glRenderMode(GL_FEEDBACK);
    draw_geometry(GL_FEEDBACK);

    size = glRenderMode(GL_RENDER);  // return number used floats in buffer
    print_buffer(size, feed_buffer);

    glFlush();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }

  /*  main loop  */
};
}
int main(int argc, char **argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
