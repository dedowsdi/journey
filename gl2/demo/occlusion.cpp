/*
 * occlusion.c
 * this program demonstrates occlusion query
 *
 * occlusion test works like bounding box test,
 * first you draw your "bounding box", check if any fragment can be drawn, if
 * nothing drawn, you can throw away your renderable.
 *
 */
#include <app.h>

namespace zxd {

GLuint queries[2];

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "occlusion";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    glGenQueries(2, queries);
    glEnable(GL_DEPTH_TEST);
  }

  void print_query(GLuint id) {}

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLint num_samples, available;

    // disable depth and color write when query
    glDepthMask(GL_FALSE);
    glColorMask(0, 0, 0, 0);

    glBeginQuery(GL_SAMPLES_PASSED, queries[0]);
    glutSolidCube(5);
    glEndQuery(GL_SAMPLES_PASSED);

    glDepthMask(GL_TRUE);
    glColorMask(1, 1, 1, 1);

    glGetQueryObjectiv(queries[0], GL_QUERY_RESULT, &num_samples);
    glGetQueryObjectiv(queries[0], GL_QUERY_RESULT_AVAILABLE, &available);
    printf("num_samples : %d, available : %s \n", num_samples,
      available ? "true" : "false");

    // if querry successed draw our renderable
    if (available) {
      glColor3f(0, 0, 1);
      glutSolidCube(4);
    }

    // try to draw a cube inside the 1st cube
    glDepthMask(GL_FALSE);
    glColorMask(0, 0, 0, 0);

    glBeginQuery(GL_SAMPLES_PASSED, queries[1]);
    glutSolidCube(3);
    glEndQuery(GL_SAMPLES_PASSED);

    glDepthMask(GL_TRUE);
    glColorMask(1, 1, 1, 1);

    glGetQueryObjectiv(queries[1], GL_QUERY_RESULT, &num_samples);
    glGetQueryObjectiv(queries[1], GL_QUERY_RESULT_AVAILABLE, &available);
    printf("num_samples : %d, available : %s \n", num_samples,
      available ? "true" : "false");

    // if querry successed draw our renderable
    if (available) {
      glColor3f(0, 0, 1);
      glutSolidCube(2);
    }

    glColor3f(1.0f, 1.0f, 1.0f);
    glWindowPos2i(10, 480);
    const GLubyte info[] = "qQ : toggle multisample \n";
    glutBitmapString(GLUT_BITMAP_9_BY_15, info);

    /*GLint box[4];*/
    /*glGetIntegerv(GL_SCISSOR_BOX, box);*/
    /*printf("%d, %d, %d, %d\n", box[0],box[1],box[2],box[3]);*/

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      glOrtho(-15, 15, -15, 15 * (GLfloat)h / (GLfloat)w, -100, 100);
    else
      glOrtho(-15, 15 * (GLfloat)w / (GLfloat)h, -15, 15, -100, 100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {
      default:
        break;
    }
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
