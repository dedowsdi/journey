/* normalmap.c
 *
 * Used to give high detail to low quality geomety.
 *
 * There are two types of normal map: model space , tangent space.
 * Benifit of tangent space:
 *   As z is always positive, you can only store xy components.
 *   Reuse the same normal map on different surfaces?
 *   The ability to modify the texture mapping?
 *
 * You need a fine grained mesh and a corse grained mesh.
 * The fine grained mesh will be used to create normal map, each triangle in
 * fine grained mesh needs to find it's "parent" triangle and it's tangent space
 * in corse grained mesh, the normal is then converted into this tangent space.
 *
 * In order to render normal map, i use tex*2 - 1 as ndc, so the corresponding
 * normal will be rendered at resolution * tex.
 */
#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include <memory.h>
#include "common.h"
#include "glm.h"

using namespace glm;

#define imageWidth 256
#define imageHeight 256

#define VAO_LOW 0
#define VAO_HIGH 1

static GLuint texName;
GLint render_normalmap_program;
GLint normalmap_program;
GLint loc_normalMap;

GLuint vaos[2];

// clang-format off
vec3 vertices0[12] = {
  vec3(0, 1, 0), vec3(-1, 0, 1), vec3(1 , 0, 1),
  vec3(0, 1, 0), vec3(1 , 0, 1), vec3(1 , 0, -1 ),
  vec3(0, 1, 0), vec3(1 , 0, -1 ), vec3(-1, 0, -1 ),
  vec3(0, 1, 0), vec3(-1, 0, -1 ), vec3(-1, 0, 1)
};
vec3 normals0[12];
vec2 texcoords0[12] = {
  vec2(0.5,0.5), vec2(0,0), vec2(1,0),
  vec2(0.5,0.5), vec2(1,0), vec2(1,1),
  vec2(0.5,0.5), vec2(1,1), vec2(0,1),
  vec2(0.5,0.5), vec2(0,1), vec2(0,0)
};

vec3 vertices1[4] = {
  vec3(-1, 0, 1 ),
  vec3(1 , 0, 1 ),
  vec3(1 , 0, -1 ),
  vec3(-1, 0, -1 )
};
vec3 normals1[4] = {
  vec3(0, 1, 0),
  vec3(0, 1, 0),
  vec3(0, 1, 0),
  vec3(0, 1, 0)
};
vec3 tangents1[4];
vec2 texcoords1[4] = {
   vec2(0,0), 
   vec2(1,0), 
   vec2(1,1), 
   vec2(0,1) 
};

// clang-format on

void generateNormalMap() {
  // render high detail geometry to generate normal map
  glViewport(0, 0, imageWidth, imageHeight);
  glBindVertexArray(vaos[VAO_HIGH]);

  // glEnable(GL_TEXTURE_2D);
  // render normal
  glUseProgram(render_normalmap_program);
  glDrawArrays(GL_TRIANGLES, 0, 12);
  glFlush();

  glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, imageWidth, imageHeight, 0);
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);

  glShadeModel(GL_SMOOTH);
  glEnable(GL_DEPTH_TEST);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  render_normalmap_program = glCreateProgram();
  attachShaderFile(render_normalmap_program, GL_VERTEX_SHADER,
    "data/shader/render_normalmap.vs.glsl");
  attachShaderFile(render_normalmap_program, GL_FRAGMENT_SHADER,
    "data/shader/render_normalmap.fs.glsl");
  ZCGE(glLinkProgram(render_normalmap_program));

  normalmap_program = glCreateProgram();
  attachShaderFile(
    normalmap_program, GL_VERTEX_SHADER, "data/shader/normalmap.vs.glsl");
  attachShaderFile(
    normalmap_program, GL_FRAGMENT_SHADER, "data/shader/normalmap.fs.glsl");
  ZCGE(glLinkProgram(normalmap_program));
  setUniformLocation(&loc_normalMap, normalmap_program, "normalMap");

  // init mesh data
  zxd::generateFaceNormals(vertices0, vertices0 + 12, normals0);

  // there are only 1 quad in low quality mesh
  mat4 tbn = zxd::getTangetnBasis(vertices1[0], vertices1[1], vertices1[2],
    texcoords1[0], texcoords1[1], texcoords1[2], &normals1[0]);
  mat4 invTbn = inverse(tbn);
  vec3 tangent = vec3(glm::column(tbn, 0));

  for (int i = 0; i < 4; ++i) tangents1[i] = tangent;

  // transform modle normals to tbn space
  for (int i = 0; i < 12; ++i)
    normals0[i] = zxd::transformVector(invTbn, normals0[i]);

  glGenVertexArrays(2, vaos);

  glBindVertexArray(vaos[VAO_HIGH]);
  {
    GLuint buffers[3];
    glGenBuffers(3, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices0), vertices0, GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnable(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals0), normals0, GL_STATIC_DRAW);
    glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnable(GL_NORMAL_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(texcoords0), texcoords0, GL_STATIC_DRAW);
    glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnable(GL_TEXTURE_COORD_ARRAY);
  }
  ZCGEA;

  glBindVertexArray(vaos[VAO_LOW]);
  {
    GLuint buffers[4];
    glGenBuffers(4, buffers);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);
    glVertexPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnable(GL_VERTEX_ARRAY);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(normals1), normals1, GL_STATIC_DRAW);
    glNormalPointer(GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnable(GL_NORMAL_ARRAY);

    glClientActiveTexture(GL_TEXTURE0);

    glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
    glBufferData(
      GL_ARRAY_BUFFER, sizeof(texcoords1), texcoords1, GL_STATIC_DRAW);
    glTexCoordPointer(2, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glClientActiveTexture(GL_TEXTURE1);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tangents1), tangents1, GL_STATIC_DRAW);
    glTexCoordPointer(3, GL_FLOAT, 0, BUFFER_OFFSET(0));
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  }
  ZCGEA;

  // texture for normal map
  glGenTextures(1, &texName);
  glBindTexture(GL_TEXTURE_2D, texName);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  // light and material
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();

  GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};

  GLfloat light_position0[] = {1.0, 1.0, 0.0, 0.0};  // 0 w means dir light
  GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};

  GLfloat lmodel_ambient[] = {0.1, 0.1, 0.1, 1.0};  // global ambient

  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);

  glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);

  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
}

void display(void) {
  glViewport(imageWidth, 0, imageWidth, imageHeight);
  glEnable(GL_SCISSOR_TEST);
  glScissor(imageWidth, 0, imageWidth, imageHeight * 2);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw low quality mesh with normal map
  glUseProgram(normalmap_program);

  glEnable(GL_TEXTURE_2D);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glBindTexture(GL_TEXTURE_2D, texName);
  glUniform1i(loc_normalMap, 0);

  glBindVertexArray(vaos[VAO_LOW]);
  glDrawArrays(GL_QUADS, 0, 4);

  glUseProgram(0);
  glDisable(GL_TEXTURE_2D);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 492);
  GLchar info[256];

  sprintf(info, " \n");
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);
  glEnable(GL_TEXTURE_2D);

  glFlush();
  ZCGEA
}

void reshape(int w, int h) {
  // create normal map
  generateNormalMap();

  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(45, 1, 1, 100);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0, 3, 3, 0, 0, 0, 0, 1, 0);
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
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(512, 256);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  initExtension();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutMainLoop();
  return 0;
}
