#ifndef GLENUMSTRING_H
#define GLENUMSTRING_H

#include <GL/gl.h>
#include "string.h"
#include <GL/glu.h>

const char* glTexEnvModeToString(GLenum e) {
  switch (e) {
    case GL_REPLACE:
      return "GL_REPLACE";
    case GL_MODULATE:
      return "GL_MODULATE";
    case GL_DECAL:
      return "GL_DECAL";
    case GL_BLEND:
      return "GL_BLEND";
    case GL_ADD:
      return "GL_ADD";
    case GL_COMBINE:
      return "GL_COMBINE";
    default:
      return "";
  }
}

GLenum glTexEnvModeFromString(const char* s) {
  if (strcmp(s, "GL_REPLACE") == 0) return GL_REPLACE;
  if (strcmp(s, "GL_MODULATE") == 0) return GL_MODULATE;
  if (strcmp(s, "GL_DECAL") == 0) return GL_DECAL;
  if (strcmp(s, "GL_BLEND") == 0) return GL_BLEND;
  if (strcmp(s, "GL_ADD") == 0) return GL_ADD;
  if (strcmp(s, "GL_COMBINE") == 0) return GL_COMBINE;
  return 0;
}

const char* glTexWrapToString(GLenum e) {
  switch (e) {
    case GL_CLAMP:
      return "GL_CLAMP";
    case GL_CLAMP_TO_BORDER:
      return "GL_CLAMP_TO_BORDER";
    case GL_CLAMP_TO_EDGE:
      return "GL_CLAMP_TO_EDGE";
    case GL_MIRRORED_REPEAT:
      return "GL_MIRRORED_REPEAT";
    case GL_REPEAT:
      return "GL_REPEAT";
    default:
      return "";
  }
}

GLenum gTexWraplFromString(const char* s) {
  if (strcmp(s, "GL_CLAMP") == 0) return GL_CLAMP;
  if (strcmp(s, "GL_CLAMP_TO_BORDER") == 0) return GL_CLAMP_TO_BORDER;
  if (strcmp(s, "GL_CLAMP_TO_EDGE") == 0) return GL_CLAMP_TO_EDGE;
  if (strcmp(s, "GL_MIRRORED_REPEAT") == 0) return GL_MIRRORED_REPEAT;
  if (strcmp(s, "GL_REPEAT") == 0) return GL_REPEAT;
  return 0;
}

const char* glTexFilterToString(GLenum e) {
  switch (e) {
    case GL_NEAREST:
      return "GL_NEAREST";
    case GL_LINEAR:
      return "GL_LINEAR";
    case GL_NEAREST_MIPMAP_NEAREST:
      return "GL_NEAREST_MIPMAP_NEAREST";
    case GL_LINEAR_MIPMAP_NEAREST:
      return "GL_LINEAR_MIPMAP_NEAREST";
    case GL_NEAREST_MIPMAP_LINEAR:
      return "GL_NEAREST_MIPMAP_LINEAR";
    case GL_LINEAR_MIPMAP_LINEAR:
      return "GL_LINEAR_MIPMAP_LINEAR";
    default:
      return "";
  }
}

GLenum glTexFilterFromString(const char* s) {
  if (strcmp(s, "GL_NEAREST") == 0) return GL_NEAREST;
  if (strcmp(s, "GL_LINEAR") == 0) return GL_LINEAR;
  if (strcmp(s, "GL_NEAREST_MIPMAP_NEAREST") == 0)
    return GL_NEAREST_MIPMAP_NEAREST;
  if (strcmp(s, "GL_LINEAR_MIPMAP_NEAREST") == 0)
    return GL_LINEAR_MIPMAP_NEAREST;
  if (strcmp(s, "GL_NEAREST_MIPMAP_LINEAR") == 0)
    return GL_NEAREST_MIPMAP_LINEAR;
  if (strcmp(s, "GL_LINEAR_MIPMAP_LINEAR") == 0) return GL_LINEAR_MIPMAP_LINEAR;
  return 0;
}

const char* gluNurbSampleMethodToString(GLenum e) {
  switch (e) {
    case GLU_OBJECT_PARAMETRIC_ERROR:
      return "GLU_OBJECT_PARAMETRIC_ERROR";
    case GLU_OBJECT_PATH_LENGTH:
      return "GLU_OBJECT_PATH_LENGTH";
    case GLU_PATH_LENGTH:
      return "GLU_PATH_LENGTH";
    case GLU_PARAMETRIC_ERROR:
      return "GLU_PARAMETRIC_ERROR";
    case GLU_DOMAIN_DISTANCE:
      return "GLU_DOMAIN_DISTANCE";
    default:
      return "";
  }
}

GLenum gluNurbSampleMethodlFromString(const char* s) {
  if (strcmp(s, "GLU_OBJECT_PARAMETRIC_ERROR") == 0)
    return GLU_OBJECT_PARAMETRIC_ERROR;
  if (strcmp(s, "GLU_OBJECT_PATH_LENGTH") == 0) return GLU_OBJECT_PATH_LENGTH;
  if (strcmp(s, "GLU_PATH_LENGTH") == 0) return GLU_PATH_LENGTH;
  if (strcmp(s, "GLU_PARAMETRIC_ERROR") == 0) return GLU_PARAMETRIC_ERROR;
  if (strcmp(s, "GLU_DOMAIN_DISTANCE") == 0) return GLU_DOMAIN_DISTANCE;
  return 0;
}

const char* glShaderTypeToString(GLenum e) {
  switch (e) {
    case GL_FRAGMENT_SHADER:
      return "GL_FRAGMENT_SHADER";
    case GL_VERTEX_SHADER:
      return "GL_VERTEX_SHADER";
    default: 
      return "";
  }
}

GLenum glShaderTypeFromString(const char* s) {
  if (strcmp(s, "GL_FRAGMENT_SHADER") == 0)
    return GL_FRAGMENT_SHADER;
  if (strcmp(s, "GL_VERTEX_SHADER") == 0)
    return GL_VERTEX_SHADER;
  return 0;
}

#endif /* GLENUMSTRING_H */
