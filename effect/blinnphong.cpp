#include "glad/glad.h"
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

GLboolean useProgram = GL_TRUE;
vec3 cameraPos = vec3(0, -3, 3);

std::vector<zxd::LightSource> lights;
zxd::LightModel lightModel;
zxd::Material material;

struct MyProgram : public zxd::Program {
  // GLint loc_eye;

  virtual void doUpdateFrame() {
    projMatrix = glm::perspective(glm::radians(45.0f), wndAspect, 0.1f, 30.0f);
    viewMatrix = glm::lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 0, 1));
    viewMatrixInverseTranspose = glm::inverse(glm::transpose(viewMatrix));

    material.updateUniforms();
    lightModel.updateUniforms();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].updateUniforms(viewMatrix, viewMatrixInverseTranspose);
    }
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

    // setUniformLocation(&loc_eye, "eye");
    setUniformLocation(&loc_modelViewMatrix, "modelViewMatrix");
    setUniformLocation(
      &loc_modelViewMatrixInverseTranspose, "modelViewMatrixInverseTranspose");
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
  }
} myProgram;

void render(zxd::Program& program) {
  mat4 model = mat4(1.0f);
  if (useProgram) {
    myProgram.updateFrame();
    myProgram.updateModel(model);
  } else {
    // setup matrix
    glUseProgram(0);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(value_ptr(myProgram.projMatrix));

    mat4 modelViewMatrix = model * myProgram.viewMatrix;
    glMatrixMode(GL_MODELVIEW);
    glLoadMatrixf(value_ptr(modelViewMatrix));

    // set up light and material
    lightModel.pipeline();
    for (int i = 0; i < lights.size(); ++i) {
      lights[i].pipeline(i, myProgram.viewMatrix);
    }
    material.pipeline();
  }
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
      " q  : use program : %d \n"
      " w  : local viewer : %d \n"
      " eE : light model ambient viewer : %.2f %.2f %.2f %.2f \n"
      " jJ : material emission : %.2f %.2f %.2f %.2f \n"
      " kK : material diffuse : %.2f %.2f %.2f %.2f \n" 
      " lL : material specular : %.2f %.2f %.2f %.2f \n" 
      " ;: : material ambient : %.2f %.2f %.2f %.2f \n" 
      " mM : material shininess : %.2f \n" ,
      useProgram, lightModel.localViewer, 
      lightModel.ambient[0], lightModel.ambient[1], lightModel.ambient[2], lightModel.ambient[3], 
      material.emission[0], material.emission[1], material.emission[2], material.emission[3], 
      material.diffuse[0], material.diffuse[1], material.diffuse[2], material.diffuse[3], 
      material.specular[0], material.specular[1], material.specular[2], material.specular[3], 
      material.ambient[0], material.ambient[1], material.ambient[2], material.ambient[3], 
      material.shininess
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

  zxd::LightSource dirLight;
  dirLight.position = vec4(1, 0, 0, 0);
  dirLight.diffuse = vec4(1, 1, 1, 1);
  dirLight.specular = vec4(1, 1, 1, 1);

  zxd::LightSource pointLight;
  pointLight.position = vec4(0, 0, 5, 1);
  pointLight.diffuse = vec4(0, 1, 0, 1);
  pointLight.specular = vec4(1, 1, 1, 1);

  zxd::LightSource spotLight;
  spotLight.position = vec4(-5, 0, 0, 1);
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
    case 'q': {
      useProgram = !useProgram;
      glutPostRedisplay();
    } break;

    case 'w':
      lightModel.localViewer ^= 1;
      glutPostRedisplay();
      break;

    case 'e':
      lightModel.ambient += 0.05;
      lightModel.ambient = glm::clamp(lightModel.ambient, 0.0f, 1.0f);
      glutPostRedisplay();
      break;
    case 'E':
      lightModel.ambient -= 0.05;
      lightModel.ambient = glm::clamp(lightModel.ambient, 0.0f, 1.0f);
      glutPostRedisplay();
      break;

    case 'j':
      material.emission += 0.05;
      material.emission = glm::clamp(material.emission, 0.0f, 1.0f);
      glutPostRedisplay();
      break;
    case 'J':
      material.emission -= 0.05;
      material.emission = glm::clamp(material.emission, 0.0f, 1.0f);
      glutPostRedisplay();
      break;

    case 'k':
      material.diffuse += 0.05;
      material.diffuse = glm::clamp(material.diffuse, 0.0f, 1.0f);
      glutPostRedisplay();
      break;
    case 'K':
      material.diffuse -= 0.05;
      material.diffuse = glm::clamp(material.diffuse, 0.0f, 1.0f);
      glutPostRedisplay();
      break;

    case 'l':
      material.specular += 0.05;
      material.specular = glm::clamp(material.specular, 0.0f, 1.0f);
      glutPostRedisplay();
      break;
    case 'L':
      material.specular -= 0.05;
      material.specular = glm::clamp(material.specular, 0.0f, 1.0f);
      glutPostRedisplay();
      break;

    case ';':
      material.ambient += 0.05;
      material.ambient = glm::clamp(material.ambient, 0.0f, 1.0f);
      glutPostRedisplay();
      break;

    case ':':
      material.ambient -= 0.05;
      material.ambient = glm::clamp(material.ambient, 0.0f, 1.0f);
      glutPostRedisplay();
      break;

    case 'm':
      material.shininess += 5;
      material.shininess = glm::clamp(material.shininess, 0.0f, 1000.0f);
      glutPostRedisplay();
      break;
    case 'M':
      material.shininess -= 5;
      material.shininess = glm::clamp(material.shininess, 0.0f, 1000.0f);
      glutPostRedisplay();
      break;
    case 27:
      exit(0);
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
