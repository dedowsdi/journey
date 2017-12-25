#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "program.h"
#include "glm.h"
#include <sstream>

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLfloat wndAspect = 1;

GLboolean useProgram = GL_TRUE;
vec3 cameraPos = vec3(0, -3, 3);

struct LightSource {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 position;  // local
  vec3 spotDirection;
  float spotExponent;
  float spotCutoff;
  float spotCosCutoff;
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;

  GLint loc_ambient;
  GLint loc_diffuse;
  GLint loc_specular;
  GLint loc_position;
  GLint loc_spotDirection;
  GLint loc_spotExponent;
  GLint loc_spotCutoff;
  GLint loc_spotCosCutoff;
  GLint loc_constantAttenuation;
  GLint loc_linearAttenuation;
  GLint loc_quadraticAttenuation;

  LightSource()
      : ambient(0, 0, 0, 1),
        diffuse(0, 0, 0, 1),
        specular(0, 0, 0, 1),
        position(0, 0, 0, 1),
        spotDirection(0, 0, -1),
        spotExponent(0),
        spotCutoff(180),
        spotCosCutoff(-1),
        constantAttenuation(1),
        linearAttenuation(0),
        quadraticAttenuation(0) {}

  void bindUniformLocations(GLuint program, const std::string& name) {
    setUniformLocation(&loc_ambient, program, name + ".ambient");
    setUniformLocation(&loc_diffuse, program, name + ".diffuse");
    setUniformLocation(&loc_specular, program, name + ".specular");
    setUniformLocation(&loc_position, program, name + ".position");
    setUniformLocation(&loc_spotDirection, program, name + ".spotDirection");
    setUniformLocation(&loc_spotExponent, program, name + ".spotExponent");
    setUniformLocation(&loc_spotCutoff, program, name + ".spotCutoff");
    setUniformLocation(&loc_spotCosCutoff, program, name + ".spotCosCutoff");
    setUniformLocation(
      &loc_constantAttenuation, program, name + ".constantAttenuation");
    setUniformLocation(
      &loc_linearAttenuation, program, name + ".linearAttenuation");
    setUniformLocation(
      &loc_quadraticAttenuation, program, name + ".quadraticAttenuation");
  }

  void updateUniforms(
    const mat4& viewMatrix, const mat4& viewMatrixTransposeInverse) {
    vec4 viewPosition = viewMatrix * position;
    vec3 viewSpotDirection = mat3(viewMatrixTransposeInverse) * spotDirection;

    glUniform4fv(loc_ambient, 1, value_ptr(ambient));
    glUniform4fv(loc_diffuse, 1, value_ptr(diffuse));
    glUniform4fv(loc_specular, 1, value_ptr(specular));
    glUniform4fv(loc_position, 1, value_ptr(viewPosition));
    glUniform3fv(loc_spotDirection, 1, value_ptr(viewSpotDirection));
    glUniform1f(loc_spotExponent, spotExponent);
    glUniform1f(loc_spotCutoff, spotCutoff);
    glUniform1f(loc_spotCosCutoff, spotCosCutoff);
    glUniform1f(loc_constantAttenuation, constantAttenuation);
    glUniform1f(loc_linearAttenuation, linearAttenuation);
    glUniform1f(loc_quadraticAttenuation, quadraticAttenuation);
  }

  void pipeline(GLuint index, const mat4& viewMatrix) {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadMatrixf(value_ptr(viewMatrix));

    GLuint glLightIndex = GL_LIGHT0 + index;
    glLightfv(glLightIndex, GL_AMBIENT, value_ptr(ambient));
    glLightfv(glLightIndex, GL_DIFFUSE, value_ptr(diffuse));
    glLightfv(glLightIndex, GL_SPECULAR, value_ptr(specular));
    glLightfv(glLightIndex, GL_POSITION, value_ptr(position));
    glLightfv(glLightIndex, GL_SPOT_DIRECTION, value_ptr(spotDirection));
    glLightf(glLightIndex, GL_SPOT_EXPONENT, spotExponent);
    glLightf(glLightIndex, GL_SPOT_CUTOFF, spotCutoff);
    glLightf(glLightIndex, GL_CONSTANT_ATTENUATION, constantAttenuation);
    glLightf(glLightIndex, GL_LINEAR_ATTENUATION, linearAttenuation);
    glLightf(glLightIndex, GL_QUADRATIC_ATTENUATION, quadraticAttenuation);

    glPopMatrix();

    glEnable(GL_LIGHTING);
    glEnable(glLightIndex);
  }
};

struct LightModel {
  vec4 ambient;
  GLboolean localViewer;

  GLint loc_ambient;
  GLint loc_localViewer;

  LightModel() : ambient(vec4(0.2f, 0.2f, 0.2f, 1.0f)), localViewer(0) {}

  void bindUniformLocations(GLint program, const std::string& name) {
    setUniformLocation(&loc_ambient, program, name + ".ambient");
    setUniformLocation(&loc_localViewer, program, name + ".localViewer");
  }
  void updateUniforms() {
    glUniform4fv(loc_ambient, 1, value_ptr(ambient));
    glUniform1i(loc_localViewer, localViewer);
  }
  void pipeline() {
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, value_ptr(ambient));
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, localViewer);
  }
};

struct Material {
  vec4 emission;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;

  GLint loc_emission;
  GLint loc_ambient;
  GLint loc_diffuse;
  GLint loc_specular;
  GLint loc_shininess;

  Material()
      : emission(0.0, 0.0, 0.0, 1.0),
        ambient(0.2, 0.2, 0.2, 1.0),
        diffuse(0.8, 0.8, 0.8, 1.0),
        specular(0.0, 0.0, 0.0, 1.0),
        shininess(0) {}
  void bindUniformLocations(GLuint program, const std::string& name) {
    setUniformLocation(&loc_emission, program, name + ".emission");
    setUniformLocation(&loc_ambient, program, name + ".ambient");
    setUniformLocation(&loc_diffuse, program, name + ".diffuse");
    setUniformLocation(&loc_specular, program, name + ".specular");
    setUniformLocation(&loc_shininess, program, name + ".shininess");
  }

  void updateUniforms() {
    glUniform4fv(loc_emission, 1, value_ptr(emission));
    glUniform4fv(loc_ambient, 1, value_ptr(ambient));
    glUniform4fv(loc_diffuse, 1, value_ptr(diffuse));
    glUniform4fv(loc_specular, 1, value_ptr(specular));
    glUniform1f(loc_shininess, shininess);
  }

  void pipeline() {
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, value_ptr(emission));
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, value_ptr(ambient));
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, value_ptr(diffuse));
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, value_ptr(specular));
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);
  }
};

std::vector<LightSource> lights;
LightModel lightModel;
Material material;

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

    // glUniform3fv(loc_eye, 1, value_ptr(localEye));
    glUniformMatrix4fv(loc_modelViewMatrixInverseTranspose, 1, 0,
      value_ptr(modelViewMatrixInverseTranspose));
    glUniformMatrix4fv(loc_modelViewMatrix, 1, 0, value_ptr(modelViewMatrix));
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, value_ptr(modelViewProjMatrix));
  }
  virtual void attachShaders() {
    attachShaderFile(GL_VERTEX_SHADER, "data/shader/blinn.vs.glsl");
    StringVector sv;
    sv.push_back("#define LIGHT_COUNT 3\n");
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

  LightSource dirLight;
  dirLight.position = vec4(1, 0, 0, 0);
  dirLight.diffuse = vec4(1, 1, 1, 1);
  dirLight.specular = vec4(1, 1, 1, 1);

  LightSource pointLight;
  pointLight.position = vec4(0, 0, 5, 0);
  pointLight.diffuse = vec4(0, 1, 0, 1);
  pointLight.specular = vec4(1, 1, 1, 1);

  LightSource spotLight;
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
