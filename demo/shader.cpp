/*
 * shader.c
 * This program demonstrates GLSL shader. Color is transformed as an extra
 * vertex attribute.
 *
 * There are two ways to use vertex attribute:
 *   if you use glVertex.. kind stuff, use glVertexAttrib
 *   if you use vertex array, use glVertexAttribPointer.
 *
 * If you want to user vertex buffer object, you don't have to put vertex
 * attibute in the same as as vertex.
 *
 * GLSL use varing qualified variable to output variable from vertex shader.
 *
 * Applications are allowed to bind more than one user-defined attribute
 * variable to the same generic vertex attribute index. This is called aliasing,
 * and it is allowed only if just one of the aliased attributes is active in the
 * executable program, or if no path through the shader consumes more than one
 * attribute of a set of attributes aliased to the same location.
 *
 */
#include <GL/glew.h>
#include <GL/freeglutt.h>
#include <stdlib.h>
#include "common.h"

GLint program;
GLint locColor;

GLfloat colors[][2] = {{1.0f, 0.0}, {0.0f, 1.0}, {0.0f, 0.0}};
GLfloat vertices[][2] = {{5, 5}, {25, 5}, {5, 25}};

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);

  const char* vs =
    "attribute vec2 color;   //vertex attribute                \n"
    "varying float blue;    //pass to fragment                 \n"
    "                                                          \n"
    "void main(){                                              \n"
    "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex; \n"
    "  gl_FrontColor = vec4(color, 0, 1);                      \n"
    "  vec3 blues = vec3(0, 0, 1);                             \n"
    "  blue = blues[gl_VertexID];                              \n"
    "}                                                         \n" ;

  const char* fs =
    "varying float blue;                                       \n"
    "void main(){                                              \n"
    "  gl_FragColor = vec4(gl_Color.rg, blue, 1);              \n"
    "}                                                         \n" ;

  program = glCreateProgram();
  attachShaderSource(program, GL_VERTEX_SHADER, 1, (char**)&vs);
  attachShaderSource(program, GL_FRAGMENT_SHADER, 1, (char**)&fs);
  ZCGE(glLinkProgram(program));
  ZCGE(locColor = glGetAttribLocation(program, "color"));
  ZCGE(glVertexAttribPointer(locColor, 2, GL_FLOAT, 0, 0, colors));
  ZCGE(glEnableVertexAttribArray(locColor));

  glVertexPointer(2, GL_FLOAT, 0, vertices);
  glEnableClientState(GL_VERTEX_ARRAY);
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(program);

  glDrawArrays(GL_TRIANGLES, 0, 3);

  /*glBegin(GL_TRIANGLES);*/
  /*glVertexAttrib3fv(locColor, colors[0]);*/
  /*glVertex2f(5.0, 5.0);*/
  /*glVertexAttrib3fv(locColor, colors[1]);*/
  /*glVertex2f(25.0, 5.0);*/
  /*glVertexAttrib3fv(locColor, colors[2]);*/
  /*glVertex2f(5.0, 25.0);*/
  /*glEnd();*/

  glFlush();
}

void reshape(int w, int h) {
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
  switch (key) {
    case 27:
      exit(0);
      break;
  }
}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
  glutInitWindowSize(500, 500);
  glutInitWindowPosition(100, 100);
  glutInitContextFlags(GLUT_DEBUG);
  glutCreateWindow(argv[0]);
  initExtension();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
