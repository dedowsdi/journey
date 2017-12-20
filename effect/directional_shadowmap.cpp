/*
 * directional_shadowmap.c
 *
 * Need ARB_framebuffer_object
 *
 * Shadow acne:
 *  As depth map is discrete, different fragments with different depth might
 *  sample the same texel, the depth discrepancy could cause shadow acne, bias
 *  is used to eliminite shaodw acne. it's calculated as :
 *    float shadowBias = clamp(bias * tan(acos(ndotl)), 0, 0.01);
 *
 * Peter pan:
 *  Caused by shadow bias, can be fiexed via replacing thin geometry with
 *  thicker geometry.
 *
 * Over sampling:
 *    This only happens when the light proj doesn't include the entire visibile
 *    area of the camea.
 *
 *    If it's beyound one of left, right, bottom, top plane, the generated
 *    texcoord doesn't belong to [0,1], you can control the visibility by
 *    setting wrap mode to clamp_to_border, and set border_color to the depth
 *    you thought fitful.
 *
 *    If it's beyound one of near and far plane, the generated light space depth
 *    doesn't belong to [0,1], you have to manually handle them in the shader.
 *
 * PCF(percentage-closer filter):
 *    sample more than once from the depth map, each time with slightly
 *    different texture coordinates. example:
 *
 *    float visibility = 1.0;
 *    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
 *    for(int x = -1; x <= 1; ++x)
 *    {
 *        for(int y = -1; y <= 1; ++y)
 *        {
 *            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) *
 * texelSize).r;
 *            visibility += currentDepth - bias > pcfDepth ? 0.0 : 1.0;
 *        }
 *    }
 *    visibility /= 9.0;
 *
 *    If you use sample2DShadow, use shadow2D or shadow2DProj, pcf will be done
 *    on hardware.
 *
 *
 *
 * You can use the same depth texture for both sample2D and sample2DShadow,
 * although opengl will complain sampler type and depth compare discrapancy.
 * It's properbally safer to make a copy for shadow map.
 *
 */

#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/freeglut_ext.h>
#include <stdlib.h>
#include "common.h"
#include "glm.h"

using namespace glm;

#define WINDOWS_WIDTH 512
#define WINDOWS_HEIGHT 512
GLuint shadowWidth = WINDOWS_WIDTH;
GLuint shadowHeight = WINDOWS_HEIGHT;

GLdouble wndAspect = WINDOWS_WIDTH / WINDOWS_HEIGHT;
GLdouble lightTop = 15;
GLdouble lightNear = 0.1, lightFar = 30;
GLfloat bias = 0.005;
GLboolean useSampler2DShadow = GL_FALSE;

mat4 lightBSVP;  // bias * scale * lightProj * lightView

GLuint fbo;
GLuint depthMap, shadowMap;
GLint useShadowProgram, renderShadowProgram;

mat4 viewMatrix, projMatrix;

const char* filterStrings[] = {"NEAREST", "LINEAR"};
GLenum filters[] = {GL_NEAREST, GL_LINEAR};
GLint filter = 0;

struct {
  GLint modelViewProjMatrix;
} render_loc;

struct {
  GLint eye;       // model space
  GLint lightDir;  // to light
  GLint lightMatrix;
  GLint modelViewProjMatrix;
  GLint bias;
  GLint useSampler2DShadow;
  GLint depthMap;
  GLint shadowMap;
} use_loc;

vec4 light_position0(-8, 8, 8, 0);
vec3 cameraPos = vec3(5, -20, 20);

GLboolean cameraAtLight = GL_FALSE;

void configureUniform(const mat4& modelMatrix, const mat4& viewMatrix,
  const mat4& projMatrix, bool isRenderShadow) {
  mat4 modelViewProjMatrix = projMatrix * viewMatrix * modelMatrix;

  if (isRenderShadow) {
    glUniformMatrix4fv(
      render_loc.modelViewProjMatrix, 1, 0, &modelViewProjMatrix[0][0]);
  } else {
    mat4 modelMatrixInverse = glm::inverse(modelMatrix);
    mat4 lightMatrix = lightBSVP * modelMatrix;

    vec3 localEye;
    if (cameraAtLight) {
      localEye =
        zxd::transformPosition(modelMatrixInverse, light_position0.xyz());
    } else {
      localEye = zxd::transformPosition(modelMatrixInverse, cameraPos);
    }
    vec3 localLightDir = vec3(glm::transpose(modelMatrix) * light_position0);
    localLightDir = glm::normalize(localLightDir);

    glUniform3fv(use_loc.eye, 1, &localEye[0]);
    glUniform3fv(use_loc.lightDir, 1, &localLightDir[0]);
    glUniformMatrix4fv(use_loc.lightMatrix, 1, 0, &lightMatrix[0][0]);
    glUniformMatrix4fv(
      use_loc.modelViewProjMatrix, 1, 0, &modelViewProjMatrix[0][0]);
    glUniform1f(use_loc.bias, bias);
    glUniform1i(use_loc.useSampler2DShadow, useSampler2DShadow);
    glUniform1i(use_loc.depthMap, 0);
    glUniform1i(use_loc.shadowMap, 1);
  }
}

void render(bool isRenderShadow) {
  if (isRenderShadow) {
    glActiveTexture(GL_TEXTURE0);
    glDisable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE1);
    glDisable(GL_TEXTURE_2D);
  } else {
    glActiveTexture(GL_TEXTURE0);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, depthMap);

    glActiveTexture(GL_TEXTURE1);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
  }

  mat4 modelMatrix(1);

  modelMatrix = glm::translate(vec3(0, 0, 3));
  configureUniform(modelMatrix, viewMatrix, projMatrix, isRenderShadow);
  glutSolidSphere(2, 16, 16);

  modelMatrix = mat4(1);
  configureUniform(modelMatrix, viewMatrix, projMatrix, isRenderShadow);
  glNormal3f(0, 0, 1);
  glRecti(-10, -10, 10, 10);

  // create a wall
  modelMatrix =
    glm::translate(vec3(-10, 0, 0)) * glm::rotate(90.f, vec3(0, 1, 0));
  configureUniform(modelMatrix, viewMatrix, projMatrix, isRenderShadow);
  glNormal3f(0, 0, 1);
  glRecti(-6, -10, 0, 10);

  glFlush();
}
void renderShadowmap() {
  GLdouble right = lightTop * wndAspect;
  GLdouble left = -right;
  projMatrix =
    glm::ortho(left, right, -lightTop, lightTop, lightNear, lightFar);
  viewMatrix = glm::lookAt(light_position0.xyz(), vec3(0, 0, 0), vec3(0, 0, 1));

  lightBSVP = glm::translate(vec3(0.5, 0.5, 0.5)) *
              glm::scale(vec3(0.5, 0.5, 0.5)) * projMatrix * viewMatrix;

  glBindFramebuffer(GL_FRAMEBUFFER, fbo);

  glViewport(0, 0, shadowWidth, shadowHeight);
  glClear(GL_DEPTH_BUFFER_BIT);

  glUseProgram(renderShadowProgram);
  glCullFace(GL_FRONT);

  // copy texture
  render(true);

  // You can use the same depth texture for both sample2D and sample2DShadow,
  // although opengl will complain sampler type and depth compare discrapancy.
  // It's properbally safer to make a copy for shadow map.
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadowMap);
  glCopyTexImage2D(
    GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 0, 0, shadowWidth, shadowHeight, 0);
}

void setupCamera() {
  if (cameraAtLight)
    viewMatrix =
      glm::lookAt(light_position0.xyz(), vec3(0, 0, 0), vec3(0, 0, 1));
  else
    viewMatrix = glm::lookAt(cameraPos, vec3(0, 0, 0), vec3(0, 0, 1));
}

void display(void) {
  renderShadowmap();

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glViewport(0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_TEXTURE_2D);
  glBindTexture(GL_TEXTURE_2D, depthMap);

  glUseProgram(useShadowProgram);
  setupCamera();
  projMatrix = glm::perspective<GLfloat>(45.0, wndAspect, 0.1, 100);

  glCullFace(GL_BACK);
  render(false);

  glUseProgram(0);
  glColor3f(1.0f, 0.0f, 0.0f);
  glWindowPos2i(10, 492);
  GLchar info[256];

  sprintf(info,
    "q  : place camera at light \n"
    "wW : camera position : %.2f %.2f %.2f\n"
    "e  : use useSampler2DShadow : %d\n"
    "r  : filter : %s\n"
    "fF : shadow resolution : %d\n"
    "iI : lightTop : %.2f\n"
    "jJ : lightNear : %.2f\n"
    "KK : lightFar : %.2f\n"
    "lL : lightPosition : %.2f %.2f %.2f\n"
    ";: : bias : %.3f\n"
    "m: GL_CULL_FACE : %d",
    cameraPos[0], cameraPos[1], cameraPos[2], useSampler2DShadow,
    filterStrings[filter], shadowWidth, lightTop, lightNear, lightFar,
    light_position0[0], light_position0[1], light_position0[2], bias,
    glIsEnabled(GL_CULL_FACE));
  glutBitmapString(GLUT_BITMAP_9_BY_15, (const GLubyte*)info);

  glFlush();
}

void init(void) {
  glClearColor(0.0, 0.5, 1.0, 0.0);
  glShadeModel(GL_SMOOTH);
  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);

  // depth map
  glGenTextures(1, &depthMap);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filters[filter]);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filters[filter]);
  GLfloat borderColor[] = {1.0, 1.0, 1.0, 1.0};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowWidth,
    shadowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

  // shadow map
  glGenTextures(1, &shadowMap);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, shadowMap);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  // setup texture compare, this will be used by shadow2D
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LESS);
  glTexParameteri(
    GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
  glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_LUMINANCE);
  // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, shadowWidth, shadowHeight, 0,
  // GL_RGBA,
  // GL_UNSIGNED_BYTE, 0);

  glGenFramebuffers(1, &fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, fbo);
  glFramebufferTexture2D(
    GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    printf("incomplete frame buffer");

  // dir light
  GLfloat light_diffuse0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_specular0[] = {1.0, 1.0, 1.0, 1.0};
  GLfloat light_ambient0[] = {0.0, 0.0, 0.0, 1.0};

  glLightfv(GL_LIGHT0, GL_POSITION, &light_position0[0]);
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse0);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular0);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient0);

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

  // render shadow program
  renderShadowProgram = glCreateProgram();
  attachShaderFile(renderShadowProgram, GL_VERTEX_SHADER,
    "data/shader/render_directional_shadowmap.vs.glsl");
  attachShaderFile(renderShadowProgram, GL_FRAGMENT_SHADER,
    "data/shader/render_directional_shadowmap.fs.glsl");
  ZCGE(glLinkProgram(renderShadowProgram));

  setUniformLocation(&render_loc.modelViewProjMatrix, renderShadowProgram,
    "modelViewProjMatrix");

  // useShadowProgram
  useShadowProgram = glCreateProgram();
  attachShaderFile(
    useShadowProgram, GL_VERTEX_SHADER, "data/shader/use_directional_shadowmap.vs.glsl");
  attachShaderFile(
    useShadowProgram, GL_FRAGMENT_SHADER, "data/shader/use_directional_shadowmap.fs.glsl");
  ZCGE(glLinkProgram(useShadowProgram));

  setUniformLocation(&use_loc.eye, useShadowProgram, "eye");
  setUniformLocation(&use_loc.lightDir, useShadowProgram, "lightDir");
  setUniformLocation(&use_loc.lightMatrix, useShadowProgram, "lightMatrix");
  setUniformLocation(
    &use_loc.modelViewProjMatrix, useShadowProgram, "modelViewProjMatrix");
  setUniformLocation(&use_loc.bias, useShadowProgram, "bias");
  setUniformLocation(&use_loc.depthMap, useShadowProgram, "depthMap");
  setUniformLocation(&use_loc.shadowMap, useShadowProgram, "shadowMap");
  setUniformLocation(
    &use_loc.useSampler2DShadow, useShadowProgram, "useSampler2DShadow");
}

void reshape(int w, int h) {
  wndAspect = static_cast<double>(w) / h;
  glViewport(0, 0, (GLsizei)w, (GLsizei)h);
  setupCamera();
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
      cameraAtLight = !cameraAtLight;
      glutPostRedisplay();
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

    case 'e': {
      useSampler2DShadow = !useSampler2DShadow;
      // toggle compare mode,  you can't use regular sample2D but tunn on
      // texture compare
      // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE,
      // useSampler2DShadow ? GL_COMPARE_R_TO_TEXTURE : GL_NONE);
      glutPostRedisplay();
    } break;

    case 'r': {
      filter ^= 1;
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filters[filter]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filters[filter]);
      glutPostRedisplay();
    } break;

    case 'f':
      shadowWidth *= 2;
      shadowHeight *= 2;
      //reset texture size
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowWidth,
        shadowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
      glutPostRedisplay();
      break;

    case 'F':
      shadowWidth /= 2;
      shadowHeight /= 2;
      //reset texture size
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, depthMap);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, shadowWidth,
        shadowHeight, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
      glutPostRedisplay();
      break;

    case 'i':
      lightTop += 0.5;
      glutPostRedisplay();
      break;

    case 'I':
      lightTop -= 0.5;
      glutPostRedisplay();
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
      light_position0[0] += 0.5;
      light_position0[1] -= 0.5;
      light_position0[2] -= 0.5;
      glutPostRedisplay();
      break;

    case 'L':
      light_position0[0] -= 0.5;
      light_position0[1] += 0.5;
      light_position0[2] += 0.5;
      glutPostRedisplay();
      break;

    case ';':
      bias += 0.001f;
      glutPostRedisplay();
      break;

    case ':':
      bias -= 0.001f;
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
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH);
  glutInitWindowSize(WINDOWS_WIDTH, WINDOWS_HEIGHT);
  glutInitWindowPosition(100, 100);
  glutCreateWindow(argv[0]);
  initExtension();
  init();
  printf("%s", getVersions());
  glutDisplayFunc(display);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutPassiveMotionFunc(passiveMotion);
  glutKeyboardFunc(keyboard);
  glutIdleFunc(idle);
  glutMainLoop();

  return 0;
}
