#include <GL/glew.h>
#include "common.h"
#include "stdlib.h"
#include "glEnumString.h"
#include <cstring>

//------------------------------------------------------------------------------
const GLchar *getVersions() {
  static GLchar version[512];
  sprintf(version,
    "GL_VERSION : %s\n"
    "GL_RENDERER : %s\n"
    "GL_VENDOR : %s\n"
    "GL_SHADING_LANGUAGE_VERSION : %s\n"
    "GLU_VERSION : %s\n",
    glGetString(GL_VERSION), glGetString(GL_RENDERER), glGetString(GL_VENDOR),
    glGetString(GL_SHADING_LANGUAGE_VERSION), gluGetString(GLU_VERSION));
  return version;
}

//------------------------------------------------------------------------------
const GLchar *getExtensions() {
  static GLchar version[10240];
  sprintf(version,
    "GL_EXTENSIONS : %s\n"
    "GLU_EXTENSIONS : %s\n",
    glGetString(GL_EXTENSIONS), glGetString(GLU_EXTENSIONS));
  return version;
}

//--------------------------------------------------------------------
GLchar *readFile(const char *file) {
  FILE *f = fopen(file, "r");
  if (f == NULL) {
    printf("faied to open file %s\n", file);
    exit(EXIT_FAILURE);
  }

  // get file character size
  fseek(f, 0, SEEK_END);
  int size = ftell(f);
  rewind(f);

  GLchar *s = (GLchar *)malloc(sizeof(GLchar *) * size + 1);
  GLuint len = fread(s, sizeof(GLchar), size, f);
  if (len == 0) {
    char str[512];
    sprintf(str, "faied to read file %s", file);
    perror(str);
    exit(EXIT_FAILURE);
  } else {
    s[len] = 0;
  }

  return s;
}

//------------------------------------------------------------------------------
GLboolean queryExtension(char *extName) {
  char *p = (char *)glGetString(GL_EXTENSIONS);
  char *end;
  if (p == NULL) return GL_FALSE;
  end = p + strlen(p);
  while (p < end) {
    int n = strcspn(p, " ");
    if ((strlen(extName) == n) && (strncmp(extName, p, n) == 0)) {
      return GL_TRUE;
    }
    p += (n + 1);
  }
  return GL_FALSE;
}

//------------------------------------------------------------------------------
GLboolean invertMatrixd(GLdouble m[16]) {
  double inv[16], det;
  int i;

  inv[0] = m[5] * m[10] * m[15] - m[5] * m[11] * m[14] - m[9] * m[6] * m[15] +
           m[9] * m[7] * m[14] + m[13] * m[6] * m[11] - m[13] * m[7] * m[10];

  inv[4] = -m[4] * m[10] * m[15] + m[4] * m[11] * m[14] + m[8] * m[6] * m[15] -
           m[8] * m[7] * m[14] - m[12] * m[6] * m[11] + m[12] * m[7] * m[10];

  inv[8] = m[4] * m[9] * m[15] - m[4] * m[11] * m[13] - m[8] * m[5] * m[15] +
           m[8] * m[7] * m[13] + m[12] * m[5] * m[11] - m[12] * m[7] * m[9];

  inv[12] = -m[4] * m[9] * m[14] + m[4] * m[10] * m[13] + m[8] * m[5] * m[14] -
            m[8] * m[6] * m[13] - m[12] * m[5] * m[10] + m[12] * m[6] * m[9];

  inv[1] = -m[1] * m[10] * m[15] + m[1] * m[11] * m[14] + m[9] * m[2] * m[15] -
           m[9] * m[3] * m[14] - m[13] * m[2] * m[11] + m[13] * m[3] * m[10];

  inv[5] = m[0] * m[10] * m[15] - m[0] * m[11] * m[14] - m[8] * m[2] * m[15] +
           m[8] * m[3] * m[14] + m[12] * m[2] * m[11] - m[12] * m[3] * m[10];

  inv[9] = -m[0] * m[9] * m[15] + m[0] * m[11] * m[13] + m[8] * m[1] * m[15] -
           m[8] * m[3] * m[13] - m[12] * m[1] * m[11] + m[12] * m[3] * m[9];

  inv[13] = m[0] * m[9] * m[14] - m[0] * m[10] * m[13] - m[8] * m[1] * m[14] +
            m[8] * m[2] * m[13] + m[12] * m[1] * m[10] - m[12] * m[2] * m[9];

  inv[2] = m[1] * m[6] * m[15] - m[1] * m[7] * m[14] - m[5] * m[2] * m[15] +
           m[5] * m[3] * m[14] + m[13] * m[2] * m[7] - m[13] * m[3] * m[6];

  inv[6] = -m[0] * m[6] * m[15] + m[0] * m[7] * m[14] + m[4] * m[2] * m[15] -
           m[4] * m[3] * m[14] - m[12] * m[2] * m[7] + m[12] * m[3] * m[6];

  inv[10] = m[0] * m[5] * m[15] - m[0] * m[7] * m[13] - m[4] * m[1] * m[15] +
            m[4] * m[3] * m[13] + m[12] * m[1] * m[7] - m[12] * m[3] * m[5];

  inv[14] = -m[0] * m[5] * m[14] + m[0] * m[6] * m[13] + m[4] * m[1] * m[14] -
            m[4] * m[2] * m[13] - m[12] * m[1] * m[6] + m[12] * m[2] * m[5];

  inv[3] = -m[1] * m[6] * m[11] + m[1] * m[7] * m[10] + m[5] * m[2] * m[11] -
           m[5] * m[3] * m[10] - m[9] * m[2] * m[7] + m[9] * m[3] * m[6];

  inv[7] = m[0] * m[6] * m[11] - m[0] * m[7] * m[10] - m[4] * m[2] * m[11] +
           m[4] * m[3] * m[10] + m[8] * m[2] * m[7] - m[8] * m[3] * m[6];

  inv[11] = -m[0] * m[5] * m[11] + m[0] * m[7] * m[9] + m[4] * m[1] * m[11] -
            m[4] * m[3] * m[9] - m[8] * m[1] * m[7] + m[8] * m[3] * m[5];

  inv[15] = m[0] * m[5] * m[10] - m[0] * m[6] * m[9] - m[4] * m[1] * m[10] +
            m[4] * m[2] * m[9] + m[8] * m[1] * m[6] - m[8] * m[2] * m[5];

  det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

  if (det == 0) return GL_FALSE;

  det = 1.0 / det;

  for (i = 0; i < 16; i++) m[i] = inv[i] * det;

  return GL_TRUE;
}

//------------------------------------------------------------------------------
GLenum rotateEnum(GLenum val, GLenum begin, GLuint size) {
  return begin + (val + 1 - begin) % size;
}

//--------------------------------------------------------------------
void attachShaderFile(GLuint prog, GLenum type, char *file) {
  char *s = readFile(file);
  if (!attachShaderSource(prog, type, 1, &s))
    printf("failed to compile file %s\n", file);
  free(s);
}

//--------------------------------------------------------------------
void attachShaderSourceAndFile(
  GLuint prog, GLenum type, GLuint count, char **source, char *file) {
  // string array to combine source and file
  char **combinedSource = (char **)malloc(count + 1);
  int i = 0;
  while (i < count) {
    combinedSource[i] = (char *)malloc(strlen(*(source + i)) + 1);
    strcpy(combinedSource[i], source[i]);
    ++i;
  }

  ++count;

  // combine file at last
  char *s = readFile(file);
  combinedSource[i] = (char *)malloc(strlen(s) + 1);
  strcpy(combinedSource[i], s);

  if (!attachShaderSource(prog, type, count, combinedSource)) {
    i = 0;
    while (i < count - 1) printf("failed to compile %s\n", source[i++]);

    printf("failed to compile file %s\n", file);
  }

  // clean up
  i = 0;
  while (i < count) {
    free(combinedSource[i++]);
  }
  free(combinedSource);
}

//------------------------------------------------------------------------------
bool attachShaderSource(GLuint prog, GLenum type, GLuint count, char **source) {
  GLuint sh;

  sh = glCreateShader(type);
  ZCGE(glShaderSource(sh, count, source, NULL));
  ZCGE(glCompileShader(sh));

  GLint compiled;
  // check if compile failed
  glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLsizei len;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);

    GLchar *log = (GLchar *)malloc(len + 1);
    glGetShaderInfoLog(sh, len, &len, log);
    printf("%s  compilation failed: %s", glShaderTypeToString(type), log);
    free(log);
    return false;
  }
  ZCGE(glAttachShader(prog, sh));
  ZCGE(glDeleteShader(sh));
  return true;
}

//--------------------------------------------------------------------
void setUniformLocation(GLint *loc, GLint program, char *name) {
  ZCGE(*loc = glGetUniformLocation(program, name));
  if (*loc == -1) {
    printf("failed to get uniform location : %s\n", name);
  }
}

//--------------------------------------------------------------------
GLfloat getTime() { return glutGet(GLUT_ELAPSED_TIME) / 1000.0; }

//--------------------------------------------------------------------
GLfloat getNormalizedTime() {
  return glutGet(GLUT_ELAPSED_TIME) % 1000 / 1000.0f;
}

//--------------------------------------------------------------------
GLfloat updateFps() {
  static GLfloat fps = 0;
  static GLuint lastTime = 0;
  static GLuint time = 0;
  static GLuint count = 0;

  GLuint curTime = glutGet(GLUT_ELAPSED_TIME);

  time += curTime - lastTime;
  ++count;

  if (time >= 1000) {
    fps = count;
    time %= 1000;
    count = 0;
  }

  lastTime = curTime;

  return fps;
}

#define checkExtension(name)                   \
  if (!GLEW_##name) {                          \
    fprintf(stdout, "##name not supported\n"); \
  }

//--------------------------------------------------------------------
void initExtension() {
  GLenum err = glewInit();
  if (GLEW_OK != err) {
    fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  }
  fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

  checkExtension(ARB_vertex_array_object);
  checkExtension(ARB_framebuffer_object);
  checkExtension(ARB_texture_stencil8);
  checkExtension(ARB_texture_float);
  checkExtension(ARB_geometry_shader4);
  checkExtension(ARB_debug_output);
  checkExtension(KHR_debug);
  checkExtension(EXT_packed_depth_stencil);
  checkExtension(EXT_gpu_shader4);

  if (!GLEW_VERSION_2_1) {
    fprintf(stdout, "OpenGL2.1 not supported");
  }

  // combined check
  // if (glewIsSupported("GL_VERSION_1_4  GL_ARB_point_sprite")) {
  /* Great, we have OpenGL 1.4 + point sprites. */
  //}

  // if (WGLEW_extension)  //check WGL extension
  // if (GLXEW_extension)  //check GLX extension

  initDebugOutput();
}

//--------------------------------------------------------------------
void initDebugOutput() {
   glEnable(GL_DEBUG_OUTPUT);
   glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

  // 4.3+
  // GLint flag;
  // glGetIntegerv(GL_CONTEXT_FLAGS, &flag);
  // if (!(flag & GL_CONTEXT_FLAG_DEBUG_BIT)) {
  // printf("faled to create debug context\n");
  // return;
  //}
  // glEnable(GL_DEBUG_OUTPUT);

  glDebugMessageCallback(glDebugOutput, 0);
}

//--------------------------------------------------------------------
void glDebugOutput(GLenum source, GLenum type, GLuint id, GLenum severity,
  GLsizei length, const GLchar *message, const void *userParam) {
  const char *debugSource = glDebugSourceToString(source);
  const char *debugType = glDebugTypeToString(type);
  const char *debugSeverity = glDebugSeverityToString(severity);

  printf("%s : %s : %s : %d : %.*s\n", debugSeverity, debugSource, debugType,
    id, length, message);
}

//--------------------------------------------------------------------
void drawTexRect(GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1) {
  glBegin(GL_QUADS);

  glTexCoord2f(0, 0);
  glVertex2f(x0, y0);

  glTexCoord2f(1, 0);
  glVertex2f(x1, y0);

  glTexCoord2f(1, 1);
  glVertex2f(x1, y1);

  glTexCoord2f(0, 1);
  glVertex2f(x0, y1);

  glEnd();
}

//--------------------------------------------------------------------
void drawXZRect(GLfloat x0, GLfloat z0, GLfloat x1, GLfloat z1) {
  glBegin(GL_QUADS);
  glVertex3f(x0, 0, z0);
  glVertex3f(x1, 0, z0);
  glVertex3f(x1, 0, z1);
  glVertex3f(x0, 0, z1);
  glEnd();
}

//--------------------------------------------------------------------
void drawXYPlane(
  GLfloat x0, GLfloat y0, GLfloat x1, GLfloat y1, GLuint slices /* = 1
*/) {
  GLfloat dx = (x1 - x0) / slices;
  GLfloat dy = (y1 - y0) / slices;
  for (int i = 0; i < slices; ++i) {
    glBegin(GL_QUAD_STRIP);
    GLfloat _y0 = y0 + dy * i;
    GLfloat _y1 = _y0 + dy;
    // loop from end to start to keep ccw order
    for (int j = slices; j >= 0; ++j) {
      GLfloat x = x0 + dx * j;
      glVertex2f(x, _y0);
      glVertex2f(x, _y1);
    }
    glEnd();
  }
}

//--------------------------------------------------------------------
void detachAllShaders(GLuint program) {
  GLuint shaders[256];
  GLsizei count;
  glGetAttachedShaders(program, 256, &count, shaders);
  while (count) {
    glDetachShader(program, shaders[--count]);
  }
}

//--------------------------------------------------------------------
void getModelViewProj(GLfloat *p) {
  GLint mode;
  glGetIntegerv(GL_MATRIX_MODE, &mode);

  glGetFloatv(GL_MODELVIEW_MATRIX, p);

  // place resultin projection matrix, then get it.
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glMultMatrixf(p);
  glGetFloatv(GL_PROJECTION_MATRIX, p);
  glPopMatrix();

  glMatrixMode(mode);
}
