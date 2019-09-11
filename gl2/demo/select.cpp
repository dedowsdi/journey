/*
 * select.c
 * this is an illustration of the selection mode and name stack, which detects
 * whether objects which collide with a viewing volume.  first, four triangles
 * and a rectangular box representing a viewing volume are drawn (draw_scene
 * routine).  the green triangle and yellow triangles appear to lie within the
 * viewing volume, but the red triangle appears to lie outside it.  then the
 * selection mode is entered (select_objects routine).  drawing to the screen
 * ceases.  to see if any collisions occur, the four triangles are called.  in
 * this example, the green triangle causes one hit with the name 1, and the
 * yellow triangles cause one hit with the name 3.
 *
 * hit:
 *
 *   whenever a name-stack manipulation command is executed or glRenderMode() is
 *   called, open_g_l writes a hit record into the selection array if there's
 * been
 *   a hit since the last time the stack was manipulated or glRenderMode() was
 *   called. glDepthRange won't affect the window coordinate z.
 *
 *   select buffer is an uint array of hits, each hits has following items:
 *     the number of the names on the name stack when the hit occurred
 *     minmum and maximum window coordinate z(0 to 1), multiplied by 2^32-1
 *     contents of name stack, bottom to top.
 *
 */
#include <app.h>

namespace zxd {

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "select";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH;
    m_info.wnd_width = 200;
    m_info.wnd_height = 200;
  }

  /* draw a triangle with vertices at (x1, y1), (x2, y2)
   * and (x3, y3) at z units away from the origin.
   */
  void draw_triangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3,
    GLfloat y3, GLfloat z) {
    glBegin(GL_TRIANGLES);
    glVertex3f(x1, y1, z);
    glVertex3f(x2, y2, z);
    glVertex3f(x3, y3, z);
    glEnd();
  }

  /* draw a rectangular box with these outer x, y, and z values */
  void draw_view_volume(
    GLfloat x1, GLfloat x2, GLfloat y1, GLfloat y2, GLfloat z1, GLfloat z2) {
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_LINE_LOOP);
    glVertex3f(x1, y1, -z1);
    glVertex3f(x2, y1, -z1);
    glVertex3f(x2, y2, -z1);
    glVertex3f(x1, y2, -z1);
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3f(x1, y1, -z2);
    glVertex3f(x2, y1, -z2);
    glVertex3f(x2, y2, -z2);
    glVertex3f(x1, y2, -z2);
    glEnd();

    glBegin(GL_LINES); /*  4 lines	*/
    glVertex3f(x1, y1, -z1);
    glVertex3f(x1, y1, -z2);
    glVertex3f(x1, y2, -z1);
    glVertex3f(x1, y2, -z2);
    glVertex3f(x2, y1, -z1);
    glVertex3f(x2, y1, -z2);
    glVertex3f(x2, y2, -z1);
    glVertex3f(x2, y2, -z2);
    glEnd();
  }

  /* draw_scene draws 4 triangles and a wire frame
   * which represents the viewing volume.
   */
  void draw_scene(void) {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, 4.0 / 3.0, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(7.5, 7.5, 12.5, 2.5, 2.5, -5.0, 0.0, 1.0, 0.0);
    glColor3f(0.0, 1.0, 0.0); /*  green triangle	*/
    draw_triangle(2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -5.0);
    glColor3f(1.0, 0.0, 0.0); /*  red triangle	*/
    draw_triangle(2.0, 7.0, 3.0, 7.0, 2.5, 8.0, -5.0);
    glColor3f(1.0, 1.0, 0.0); /*  yellow triangles	*/
    draw_triangle(2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -1.0);
    draw_triangle(2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -9.0);
    draw_view_volume(0.0, 5.0, 0.0, 5.0, 0.0, 10.0);
  }

  /* process_hits prints out the contents of the selection array
   */
  void process_hits(GLint hits, GLuint buffer[]) {
    unsigned int i, j;
    GLuint names, *ptr;

    printf("hits = %d\n", hits);
    ptr = (GLuint*)buffer;
    for (i = 0; i < hits; i++) { /*  for each hit  */
      names = *ptr;
      printf(" number of names for hit = %d\n", names);
      ptr++;
      printf("  z1 is %g;", (float)*ptr / 0xffffffff);
      ptr++;
      printf(" z2 is %g\n", (float)*ptr / 0xffffffff);
      ptr++;
      printf("   the name is ");
      for (j = 0; j < names; j++) { /*  for each name */
        printf("%d ", *ptr);
        ptr++;
      }
      printf("\n");
    }
  }

/* select_objects "draws" the triangles in selection mode,
 * assigning names for the triangles.  note that the third
 * and fourth triangles share one name, so that if either
 * or both triangles intersects the viewing/clipping volume,
 * only one hit will be registered.
 */
#define BUFSIZE 512

  void select_objects(void) {
    GLuint select_buf[BUFSIZE];
    GLint hits;

    glSelectBuffer(BUFSIZE, select_buf);  // must be called before glRenderMode
    (void)glRenderMode(GL_SELECT);

    // set up view volume
    glPushMatrix();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(
      0.0, 5.0, 0.0, 5.0, 0.0, 10.0);  // the same as volume drawed eariler
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    /*
     * whenever a name-stack manipulation command is executed or glRenderMode()
     * is
     * called, open_g_l writes a hit record into the selection array if there's
     * been
     * a hit since the last time the stack was manipulated or glRenderMode() was
     * called.
     * */
    glInitNames();
    glPushName(0);
    glPushName(1);
    draw_triangle(2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -5.0);
    glLoadName(2);
    draw_triangle(2.0, 7.0, 3.0, 7.0, 2.5, 8.0, -5.0);
    glLoadName(3);
    draw_triangle(2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -1.0);
    draw_triangle(2.0, 2.0, 3.0, 2.0, 2.5, 3.0, -9.0);
    glPopMatrix();
    glFlush();

    hits = glRenderMode(GL_RENDER);
    process_hits(hits, select_buf);
  }

  void create_scene(void) {
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_FLAT);
  }

  void display(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    draw_scene();
    select_objects();
    glFlush();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }

  /*  main loop  */
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
