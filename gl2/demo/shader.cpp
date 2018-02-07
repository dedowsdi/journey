/*
 * shader.c
 * this program demonstrates GLSL shader. color is transformed as an extra
 * vertex attribute.
 *
 * there are two ways to use vertex attribute:
 *   if you use glVertex.. kind stuff, use glVertexAttrib
 *   if you use vertex array, use glVertexAttribPointer.
 *
 * GLSL use varing qualified variable to output variable from vertex shader.
 *
 * applications are allowed to bind more than one user-defined attribute
 * variable to the same generic vertex attribute index. this is called aliasing,
 * and it is allowed only if just one of the aliased attributes is active in the
 * executable program, or if no path through the shader consumes more than one
 * attribute of a set of attributes aliased to the same location.
 *
 */
#include "app.h"
#include "program.h"

namespace zxd {

GLint al_color;

GLfloat colors[][2] = {{1.0f, 0.0}, {0.0f, 1.0}, {0.0f, 0.0}};
GLfloat vertices[][2] = {{5, 5}, {25, 5}, {5, 25}};

struct program0 : public program {
  GLint ul_iamge;
  virtual void attach_shaders() {
    const char* vs =
      "attribute vec2 color;   //vertex attribute                \n"
      "varying float blue;    //pass to fragment                 \n"
      "                                                          \n"
      "void main(){                                              \n"
      "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \n"
      "  gl_FrontColor = vec4(color, 0, 1);                      \n"
      "  vec3 blues = vec3(0, 0, 1);                             \n"
      "  blue = blues[gl_VertexID];                              \n"
      "}                                                         \n";

    const char* fs =
      "varying float blue;                                       \n"
      "void main(){                                              \n"
      "  gl_FragColor = vec4(gl_Color.rg, blue, 1);              \n"
      "}                                                         \n";

    string_vector sv;
    sv.push_back(vs);
    attach_shader_source(GL_VERTEX_SHADER, sv);

    sv.clear();
    sv.push_back(fs);
    attach_shader_source(GL_FRAGMENT_SHADER, sv);
  }
  virtual void bind_uniform_locations() {
    uniform_location(&ul_iamge, "image");
    uniform_location(&al_color, "color");
  };

  virtual void bind_attrib_locations() { al_color = attrib_location("color"); }
} prg0;

class app0 : public app {
  void init_info() {
    app::init_info();
    m_info.title = "shader";
    m_info.display_mode = GLUT_SINGLE | GLUT_RGB;
    m_info.wnd_width = 500;
    m_info.wnd_height = 500;
  }

  void create_scene(void) {
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

    prg0.init();

    ZCGE(glVertexAttribPointer(al_color, 2, GL_FLOAT, 0, 0, colors));
    ZCGE(glEnableVertexAttribArray(al_color));

    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);
  }

  void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(prg0.object);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    /*glBegin(GL_TRIANGLES);*/
    /*glVertexAttrib3fv(al_color, colors[0]);*/
    /*glVertex2f(5.0, 5.0);*/
    /*glVertexAttrib3fv(al_color, colors[1]);*/
    /*glVertex2f(25.0, 5.0);*/
    /*glVertexAttrib3fv(al_color, colors[2]);*/
    /*glVertex2f(5.0, 25.0);*/
    /*glEnd();*/

    glFlush();
  }

  void reshape(int w, int h) {
    app::reshape(w, h);
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (w <= h)
      gluOrtho2D(0.0, 30.0, 0.0, 30.0 * (GLfloat)h / (GLfloat)w);
    else
      gluOrtho2D(0.0, 30.0 * (GLfloat)w / (GLfloat)h, 0.0, 30.0);
    glMatrixMode(GL_MODELVIEW);
  }

  void keyboard(unsigned char key, int x, int y) {
    app::keyboard(key, x, y);
    switch (key) {}
  }
};
}
int main(int argc, char** argv) {
  zxd::app0 _app0;
  _app0.run(argc, argv);
  return 0;
}
