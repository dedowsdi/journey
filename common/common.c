#include "common.h"
#include "stdlib.h"
#include "glEnumString.h"

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
//------------------------------------------------------------------------------
void attachShaderSource(GLuint prog, GLenum type, const char *source) {
  GLuint sh;

  sh = glCreateShader(type);
  ZCGE(glShaderSource(sh, 1, &source, NULL));
  ZCGE(glCompileShader(sh));

  GLint compiled;
  // check if compile failed
  glGetShaderiv(sh, GL_COMPILE_STATUS, &compiled);
  if (!compiled) {
    GLsizei len;
    glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &len);

    GLchar *log = malloc(len + 1);
    glGetShaderInfoLog(sh, len, &len, log);
    printf("%s  compilation failed: %s", glShaderTypeToString(type), log);
    free(log);
  }
  ZCGE(glAttachShader(prog, sh));
  ZCGE(glDeleteShader(sh));
}

//------------------------------------------------------------------------------
void attachShaderFile(GLuint prog, GLenum type, const char *source) {
  FILE *fp = fopen(source, "r");
  if (!fp) {
    char s[512];
    sprintf(s, "failed to open %s\n", source);
    perror(s); 
    return;
  }
  fseek(fp, 0, SEEK_END);
  GLuint size = ftell(fp);
  rewind(fp);

  char *str = malloc(size);
  fread(str, 1, size, fp);
  attachShaderSource(prog, type, str);
  free(str);
}
