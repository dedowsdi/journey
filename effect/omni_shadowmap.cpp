/*
 * Cast shadow for point type light.
 *
 * Similar to directional_shadowmap, except it use cube map as depth texture,
 * which means you need to render 6 times to create the depth cubemap.
 *
 * Depth component of result of perspective component isn't linear, it's
 * precision is very bad if object is not near the nearplane, so i store the
 * real linear depth in the depth map:
 *    depth = (zDistance - nearPlane) / (farPlane - nearPlane)
 *
 * To use the depth cubemap, you need a l2v(light to vertex) vector in world
 * space to get stored light space depth, you can also get current fragment
 * zDistance as max(abs(l2v.x), max(abs(l2v.y), abs(l2v.x)))
 *
 */

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "program.h"

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512

GLuint shadowWidth = WINDOWS_WIDTH;
GLuint shadowHeight = WINDOWS_HEIGHT;
GLfloat wndAspect = WINDOWS_WIDTH / WINDOWS_HEIGHT;
GLfloat bias = 0.005;

GLuint fbo;
GLuint depthCubeMap;
GLfloat lightNear = 0.1, lightFar = 30;

const char* filterStrings[] = {"NEAREST", "LINEAR"};
GLenum filters[] = {GL_NEAREST, GL_LINEAR};
GLint filter = 0;

vec4 light_position0(0, 0, 8, 1);
vec3 cameraPos = vec3(0, -15, 15);

struct RenderProgram : public zxd::Program {
  GLint loc_nearPlane;
  GLint loc_farPlane;

  mat4 lightViewMatrixes[6];
  GLint index;  // current rendering index of cubemap

  RenderProgram() {
    projMatrix =
      glm::perspective(glm::radians(90.0f), wndAspect, lightNear, lightFar);
    resetLightPosition();
  }

  void bindUniformLocations() {
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_nearPlane, "nearPlane");
    setUniformLocation(&loc_farPlane, "farPlane");
  }

  void attachShaders() {
    // render shadow program
    attachShaderFile(
      GL_VERTEX_SHADER, "data/shader/render_omni_shadowmap.vs.glsl");
    attachShaderFile(
      GL_FRAGMENT_SHADER, "data/shader/render_omni_shadowmap.fs.glsl");
  }

  void resetLightPosition() {
    vec3 lightPosition(
      light_position0[0], light_position0[1], light_position0[2]);

    // TODO understand up vector.
    lightViewMatrixes[0] =
      glm::lookAt(lightPosition, lightPosition + vec3(1, 0, 0), vec3(0, -1, 0));

    lightViewMatrixes[1] = glm::lookAt(
      lightPosition, lightPosition + vec3(-1, 0, 0), vec3(0, -1, 0));

    lightViewMatrixes[2] =
      glm::lookAt(lightPosition, lightPosition + vec3(0, 1, 0), vec3(0, 0, 1));

    lightViewMatrixes[3] = glm::lookAt(
      lightPosition, lightPosition + vec3(0, -1, 0), vec3(0, 0, -1));

    lightViewMatrixes[4] =
      glm::lookAt(lightPosition, lightPosition + vec3(0, 0, 1), vec3(0, -1, 0));

    lightViewMatrixes[5] = glm::lookAt(
      lightPosition, lightPosition + vec3(0, 0, -1), vec3(0, -1, 0));
  }

  virtual void doUpdateFrame() {}
  virtual void doUpdateModel() {
    modelViewProjMatrix = projMatrix * lightViewMatrixes[index] * modelMatrix;

    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, &modelViewProjMatrix[0][0]);
    glUniform1f(loc_nearPlane, lightNear);
    glUniform1f(loc_farPlane, lightFar);
  }
};
RenderProgram renderProgram;

struct UseProgram : public zxd::Program {
  GLint loc_viewLightPos;
  GLint loc_worldLightPos;
  GLint loc_depthCubeMap;
  GLint loc_bias;
  GLint loc_nearPlane;
  GLint loc_farPlane;

  virtual void doUpdateFrame() {
    viewMatrix = glm::lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 0, 1));
    projMatrix = glm::perspective<GLfloat>(45.0, wndAspect, 0.1, 100);

    vec3 viewLightPos = (viewMatrix * light_position0).xyz();
    glUniform3fv(loc_viewLightPos, 1, &viewLightPos[0]);

    glUniform1f(loc_bias, bias);
    glUniform1i(loc_depthCubeMap, 0);
  }
  virtual void doUpdateModel() {
    modelViewMatrix = viewMatrix * modelMatrix;
    modelViewMatrixInverseTranspose = glm::inverse(glm::transpose(modelViewMatrix));
    modelViewProjMatrix = projMatrix * modelViewMatrix;
    //mat4 modelMatrixInverse = glm::inverse(modelMatrix);

    glUniformMatrix4fv(loc_modelMatrix, 1, 0, &modelMatrix[0][0]);
    glUniformMatrix4fv(
      loc_modelViewMatrix, 1, 0, &modelViewMatrix[0][0]);
    glUniformMatrix4fv(
      loc_modelViewProjMatrix, 1, 0, &modelViewProjMatrix[0][0]);
    glUniformMatrix4fv(
      loc_modelViewMatrixInverseTranspose, 1, 0, &modelViewMatrixInverseTranspose[0][0]);

    glUniform3f(loc_worldLightPos, light_position0[0], light_position0[1],
      light_position0[2]);
    glUniform1i(loc_depthCubeMap, 0);
    glUniform1f(loc_bias, bias);
    glUniform1f(loc_nearPlane, lightNear);
    glUniform1f(loc_farPlane, lightFar);
  }

  virtual void attachShaders() {
    // useProgram
    attachShaderFile(
      GL_VERTEX_SHADER, "data/shader/use_omni_shadowmap.vs.glsl");
    attachShaderFile(
      GL_FRAGMENT_SHADER, "data/shader/use_omni_shadowmap.fs.glsl");
  }

  virtual void bindUniformLocations() {
    setUniformLocation(&loc_modelMatrix, "modelMatrix");
    setUniformLocation(&loc_modelViewMatrixInverseTranspose, "modelViewMatrixInverseTranspose");
    setUniformLocation(&loc_modelViewMatrix, "modelViewMatrix");
    setUniformLocation(&loc_modelViewProjMatrix, "modelViewProjMatrix");
    setUniformLocation(&loc_bias, "bias");
    setUniformLocation(&loc_viewLightPos, "viewLightPos");
    setUniformLocation(&loc_worldLightPos, "worldLightPos");
    setUniformLocation(&loc_depthCubeMap, "depthCubeMap");
    setUniformLocation(&loc_bias, "bias");
    setUniformLocation(&loc_nearPlane, "nearPlane");
    setUniformLocation(&loc_farPlane, "farPlane");
  }
};
UseProgram useProgram;

void render(zxd::Program& program) {
  glm::mat4 model = glm::translate(vec3(0, 0, 2));
  program.updateModel(model);
  glutSolidSphere(1, 16, 16);

  GLuint numSphere = 8;
  GLfloat stepRotate = glm::pi<GLfloat>() * 2 / numSphere;
  for (int i = 0; i < numSphere; ++i) {
    vec4 pos = glm::rotate(stepRotate * i, vec3(0, 0, 1)) * vec4(5, 0, 2, 1);
    model = glm::translate(pos.xyz());

    program.updateModel(model);
    glutSolidSphere(1, 16, 16);
  }

  model = glm::mat4(1.0);
  program.updateModel(model);
  glNormal3f(0, 0, 1);
  glRecti(-12, -12, 12, 12);
}

void renderShadowMap() {
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glViewport(0, 0, shadowWidth, shadowHeight);

  glCullFace(GL_FRONT);
  renderProgram.updateFrame();

  // render 6 faces of cube map
  for (int i = 0; i < 6; ++i) {
    ZCGE(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
      GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, depthCubeMap, 0));

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
      printf("incomplete frame buffer\n");

    glClear(GL_DEPTH_BUFFER_BIT);

    renderProgram.index = i;
    render(renderProgram);
  }
}

void display(void) {
  renderShadowMap();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glViewport(0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glCullFace(GL_BACK);

  useProgram.updateFrame();
  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);

  render(useProgram);

  glUseProgram(0);
  glColor3f(1.0f, 0.0f, 0.0f);
  glWindowPos2i(10, 492);

  GLchar info[512];
  sprintf(info,
    "wW : camera position : %.2f %.2f %.2f\n"
    "r  : filter : %s\n"
    "fF : shadow resolution : %d\n"
    "jJ : lightNear : %.2f\n"
    "KK : lightFar : %.2f\n"
    "lL : lightPosition : %.2f %.2f %.2f\n"
    ";: : bias : %.3f\n"
    "m: GL_CULL_FACE : %d",
    cameraPos[0], cameraPos[1], cameraPos[2], filterStrings[filter],
    shadowWidth, lightNear, lightFar, light_position0[0], light_position0[1],
    light_position0[2], bias, glIsEnabled(GL_CULL_FACE));
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glFlush();
}

void init(void) {
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glShadeModel(GL_SMOOTH);

  // light
  GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_ambient0[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat light_attenuation0[] = {1, 0, 0};

  glLightfv(GL_LIGHT0, GL_POSITION, &light_position0[0]);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);
  glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, light_attenuation0[0]);
  glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, light_attenuation0[1]);
  glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, light_attenuation0[2]);

  // material
  GLfloat mat_ambient[] = {0.2, 0.2, 0.2, 1.0};
  GLfloat mat_diffuse[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat mat_specular[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat mat_emission[] = {0.0, 0.0, 0.0, 1.0};
  GLfloat mat_shininess = 50.0;

  glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, mat_ambient);
  glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, mat_diffuse);
  glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);
  glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, mat_emission);
  glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

  // create depth cube map
  glGenTextures(1, &depthCubeMap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  for (int i = 0; i < 6; ++i) {
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT16,
      shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  }

  // create frame buffer
  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  renderProgram.init();
  useProgram.init();

}

void reshape(int w, int h) {
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
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
      break;

    case 'w': {
      float d = glm::length(cameraPos);
      vec3 n = glm::normalize(cameraPos);
      cameraPos = n * (d - 0.5f);
      glutPostRedisplay();
    } break;

    case 'W': {
      float d = glm::length(cameraPos);
      vec3 n = glm::normalize(cameraPos);
      cameraPos = n * (d + 0.5f);
      glutPostRedisplay();
    } break;

    case 'e':
      break;

    case 'r': {
      filter ^= 1;
      glTexParameteri(
        GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, filters[filter]);
      glTexParameteri(
        GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, filters[filter]);
      glutPostRedisplay();
    } break;

    case 'f':
      shadowWidth *= 2;
      shadowHeight *= 2;
      // reset texture size
      glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
      for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
          GL_DEPTH_COMPONENT16, shadowWidth, shadowHeight, 0,
          GL_DEPTH_COMPONENT, GL_FLOAT, 0);
      }
      glutPostRedisplay();
      break;

    case 'F':
      shadowWidth /= 2;
      shadowHeight /= 2;
      // reset texture size
      glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubeMap);
      for (int i = 0; i < 6; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
          GL_DEPTH_COMPONENT16, shadowWidth, shadowHeight, 0,
          GL_DEPTH_COMPONENT, GL_FLOAT, 0);
      }
      glutPostRedisplay();
      break;

    case 'i':
      break;

    case 'I':
      break;
    case 'j':
      lightNear += 0.5;
      glutPostRedisplay();
      break;

    case 'J':
      lightNear -= 0.5;
      glutPostRedisplay();
      break;

    case 'k':
      lightFar += 0.5;
      glutPostRedisplay();
      break;

    case 'K':
      lightFar -= 0.5;
      glutPostRedisplay();
      break;

    case 'l':
      light_position0[2] += 0.5;
      renderProgram.resetLightPosition();
      glutPostRedisplay();
      break;

    case 'L':
      light_position0[2] -= 0.5;
      renderProgram.resetLightPosition();
      glutPostRedisplay();
      break;

    case ';':
      bias += 0.01f;
      glutPostRedisplay();
      break;

    case ':':
      bias -= 0.01f;
      bias = std::max(0.0f, bias);
      glutPostRedisplay();
      break;

    case 'm': {
      GLboolean e = glIsEnabled(GL_CULL_FACE);
      if (e)
        glDisable(GL_CULL_FACE);
      else
        glEnable(GL_CULL_FACE);
      glutPostRedisplay();
    } break;

    default:
      break;
  }
}
void idle() {}
void passiveMotion(int x, int y) {}

int main(int argc, char** argv) {
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
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
