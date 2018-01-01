#include "glad/glad.h"
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "program.h"
#include "glm.h"
#include <sstream>
#include "light.h"
#include "sphere.h"
#include <vector>

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wndAspect = 1;

vec3 cameraPos = vec3(0, -30, 30);

std::vector<zxd::LightSource> lights;
zxd::LightModel lightModel;
zxd::Material material;

zxd::Sphere sphere(0.5, 16, 16);

GLuint numInstance = 1000;

struct MyProgram : public zxd::Program {
  // GLint loc_eye;

  virtual void doUpdateFrame() {
    projMatrix = glm::perspective(glm::radians(45.0f), wndAspect, 0.1f, 50.0f);
    viewMatrix = glm::lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 0, 1));
    viewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix));

    material.updateUniforms();
    lightModel.updateUniforms();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].updateUniforms(viewMatrix, viewMatrixInverseTranspose);
    }
  }
  virtual void doUpdateModel() {
    // everything is done on vertex attribute
  }
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/instance.vs.glsl");
    StringVector sv;
    sv.push_back("#define LIGHT_COUNT 8\n");
    sv.push_back(readFile("data/shader/blinn.frag"));
    attachShaderSourceAndFile(
      GL_FRAGMENT_SHADER, sv, "data/shader/blinn.fs.glsl");
  }
  virtual void bindUniformLocations() {
    lightModel.bindUniformLocations(object, "lightModel");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bindUniformLocations(object, ss.str());
    }
    material.bindUniformLocations(object, "material");
  }
} myProgram;

void render(zxd::Program& program) {
  myProgram.updateFrame();

  mat4 model = mat4(1.0f);
  myProgram.updateModel(model);
  sphere.draw(numInstance);
}

// update transform attribute
void resetMatrixAttribute() {
  myProgram.projMatrix =
    glm::perspective(glm::radians(45.0f), wndAspect, 0.1f, 50.0f);
  myProgram.viewMatrix = glm::lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 0, 1));

  std::vector<mat4> modelViewProjMatrixes;
  std::vector<mat4> modelViewMatrixes;
  std::vector<mat4> modelViewMatrixInverseTransposes;
  modelViewProjMatrixes.reserve(numInstance);
  modelViewMatrixes.reserve(numInstance);
  modelViewMatrixInverseTransposes.reserve(numInstance);

  for (int i = 0; i < numInstance; ++i) {
    mat4 modelMatrix = glm::translate(
      glm::linearRand(vec3(-10.0, -10.0, -10.0), vec3(10.0, 10.0, 10.0)));
    mat4 modelViewMatrix = myProgram.viewMatrix * modelMatrix;
    mat4 modelViewProjMatrix = myProgram.projMatrix * modelViewMatrix;
    mat4 modelViewMatrixInverseTranspose =
      glm::inverse(glm::transpose(modelViewMatrix));

    modelViewMatrixes.push_back(modelViewMatrix);
    modelViewProjMatrixes.push_back(modelViewProjMatrix);
    modelViewMatrixInverseTransposes.push_back(modelViewMatrixInverseTranspose);
  }

  glBindVertexArray(sphere.getVao());

  {
    GLint location = getAttribLocation(myProgram, "modelViewProjMatrix");
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelViewProjMatrixes.size() * sizeof(mat4),
      value_ptr(modelViewProjMatrixes[0]), GL_STATIC_DRAW);

    matrixAttribPointer(location);
  }

  {
    GLint location = getAttribLocation(myProgram, "modelViewMatrix");
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, modelViewMatrixes.size() * sizeof(mat4),
      value_ptr(modelViewMatrixes[0]), GL_STATIC_DRAW);

    matrixAttribPointer(location);
  }

  {
    GLint location =
      getAttribLocation(myProgram, "modelViewMatrixInverseTranspose");
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER,
      modelViewMatrixInverseTransposes.size() * sizeof(mat4),
      value_ptr(modelViewMatrixInverseTransposes[0]), GL_STATIC_DRAW);

    matrixAttribPointer(location);
  }
}

void display(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  render(myProgram);

  glDisable(GL_TEXTURE_2D);

  GLint program;
  glGetIntegerv(GL_CURRENT_PROGRAM, &program);

  glColor3f(1.0f, 1.0f, 1.0f);
  glWindowPos2i(10, 492);
  GLchar info[512];

  // clang-format off
  sprintf(info, 
      "q : numInstance : %d",
      numInstance
      );
  // clang-format on

  glUseProgram(0);

  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glutSwapBuffers();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  // glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  zxd::LightSource dirLight;
  dirLight.position = vec4(1, 0, 0, 0);
  dirLight.diffuse = vec4(1, 1, 1, 1);
  dirLight.specular = vec4(1, 1, 1, 1);

  zxd::LightSource pointLight;
  pointLight.position = vec4(0, 0, 5, 1);
  pointLight.diffuse = vec4(0, 1, 0, 1);
  pointLight.specular = vec4(1, 1, 1, 1);

  zxd::LightSource spotLight;
  spotLight.position = vec4(-30, 0, 0, 1);
  spotLight.diffuse = vec4(0, 0, 1, 1);
  spotLight.specular = vec4(0, 0, 1, 1);
  spotLight.spotDirection = vec3(vec3(0) - spotLight.position.xyz());
  spotLight.spotCutoff = 30;
  spotLight.spotCosCutoff = std::cos(spotLight.spotCutoff);
  spotLight.spotExponent = 3;

  lights.push_back(dirLight);
  lights.push_back(pointLight);
  lights.push_back(spotLight);

  material.shininess = 80;
  material.specular = vec4(1.0, 1.0, 1.0, 1.0);

  myProgram.init();

  sphere.buildVertex(getAttribLocation(myProgram, "vertex"));
  sphere.buildNormal(getAttribLocation(myProgram, "normal"));

  resetMatrixAttribute();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  wndAspect = static_cast<GLfloat>(w) / h;

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  if (w <= h)
    glOrtho(-10.0, 10.0, -10.0 * (GLfloat)h / (GLfloat)w,
      10.0 * (GLfloat)h / (GLfloat)w, -1.0, 1.0);
  else
    glOrtho(-10.0 * (GLfloat)w / (GLfloat)h, 10.0 * (GLfloat)w / (GLfloat)h,
      -10.0, 10.0, -1.0, 1.0);

  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouse(int button, int state, int x, int y) {
  switch (button) {
    default:
      break;
  }
}

void keyboard(unsigned char key, int x, int y) {
  switch (key) {
    case 27:
      exit(0);
      break;

    case 'q':
      numInstance += 100;
      resetMatrixAttribute();
      glutPostRedisplay();
      break;

    case 'Q':
      numInstance -= 100;
      resetMatrixAttribute();
      glutPostRedisplay();
      break;

    default:
      break;
  }
}
void idle() {}
void passiveMotion(int x, int y) {}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WINDOWS_WIDTH, WINDOWS_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  loadGL();
  init();
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(passiveMotion);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  glutMainLoop();

  return 0;
}
