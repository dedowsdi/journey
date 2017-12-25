#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "program.h"
#include "glm.h"
#include <sstream>
#include "light.h"

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wndAspect = 1;

vec3 cameraPos = vec3(0, -3, 3);
vec3 rimColor = vec3(1, 0, 0);
GLfloat rimPower = 10.0f;

std::vector<zxd::LightSource> lights;
zxd::LightModel lightModel;
zxd::Material material;

struct MyProgram : public zxd::Program {
  GLint loc_rimColor;
  GLint loc_rimPower;

  virtual void doUpdateFrame() {
    projMatrix = glm::perspective(glm::radians(45.0f), wndAspect, 0.1f, 30.0f);
    viewMatrix = glm::lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 0, 1));
    viewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix));

    material.updateUniforms();
    lightModel.updateUniforms();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].updateUniforms(viewMatrix, viewMatrixInverseTranspose);
    }

    glUniform3fv(loc_rimColor, 1, value_ptr(rimColor));
    glUniform1f(loc_rimPower, rimPower);
  }
  virtual void doUpdateModel() {
    // modelMatrixInverse = glm::inverse(modelMatrix);
    modelViewMatrix = viewMatrix * modelMatrix;
    modelViewMatrixInverseTranspose =
      glm::inverse(glm::transpose(modelViewMatrix));
    modelViewProjMatrix = projMatrix * modelViewMatrix;

    glUniformMatrix4fv(loc_modelViewMatrixInverseTranspose, 1, 0,
      value_ptr(modelViewMatrixInverseTranspose));
    glUniformMatrix4fv(loc_modelViewMatrix, 1, 0, value_ptr(modelViewMatrix));
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
  }
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/blinn.vs.glsl");
    StringVector sv;
    sv.push_back("#define LIGHT_COUNT 8\n");
    sv.push_back(readFile("data/shader/blinn.frag"));
    attachShaderSourceAndFile(
      GL_FRAGMENT_SHADER, sv, "data/shader/rim.fs.glsl");
  }
  virtual void bindUniformLocations() {
    lightModel.bindUniformLocations(object, "lightModel");
    for (int i = 0; i < lights.size(); ++i) {
      std::stringstream ss;
      ss << "lights[" << i << "]";
      lights[i].bindUniformLocations(object, ss.str());
    }
    material.bindUniformLocations(object, "material");

    // setUniformLocation(&loc_eye, "eye");
    setUniformLocation(&loc_modelViewMatrix, "modelViewMatrix");
    setUniformLocation(
      &loc_modelViewMatrixInverseTranspose, "modelViewMatrixInverseTranspose");
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_rimColor, "rimColor");
    setUniformLocation(&loc_rimPower, "rimPower");
  }
} myProgram;

void render(zxd::Program& program) {
  mat4 model = mat4(1.0f);
  myProgram.updateFrame();
  myProgram.updateModel(model);
  glutSolidSphere(1, 64, 64);
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
      "qQ : rimPower : %.2f\n"
      "w : rimColor : %.2f %.2f %.2f \n",
      rimPower, rimColor[0], rimColor[1], rimColor[2]
      );
  // clang-format on

  glUseProgram(0);

  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glutSwapBuffers();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);

  zxd::LightSource pointLight;
  pointLight.position = vec4(0, 0, 5, 0);
  pointLight.diffuse = vec4(1, 1, 1, 1);
  pointLight.specular = vec4(1, 1, 1, 1);

  lights.push_back(pointLight);

  material.shininess = 80;
  material.specular = vec4(1.0, 1.0, 1.0, 1.0);

  myProgram.init();
}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  wndAspect = static_cast<GLfloat>(w) / h;
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
      rimPower += 1;
      glutPostRedisplay();
      break;

    case 'Q':
      rimPower -= 1;
      if (rimPower < 0) {
        rimPower = 0;
      }
      glutPostRedisplay();
      break;

    case 'w':
    case 'W':
      rimColor = glm::linearRand(vec3(0), vec3(1));
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
  initExtension();
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
