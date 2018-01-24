#include "glEnumString.h"
#include <GL/glu.h>
#include <cstring>

//--------------------------------------------------------------------
const char* glTexWrapToString(GLenum e) {
  switch (e) {
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

//--------------------------------------------------------------------
GLenum gTexWraplFromString(const char* s) {
  if (strcmp(s, "GL_CLAMP_TO_BORDER") == 0) return GL_CLAMP_TO_BORDER;
  if (strcmp(s, "GL_CLAMP_TO_EDGE") == 0) return GL_CLAMP_TO_EDGE;
  if (strcmp(s, "GL_MIRRORED_REPEAT") == 0) return GL_MIRRORED_REPEAT;
  if (strcmp(s, "GL_REPEAT") == 0) return GL_REPEAT;
  return 0;
}

//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
GLenum gluNurbSampleMethodlFromString(const char* s) {
  if (strcmp(s, "GLU_OBJECT_PARAMETRIC_ERROR") == 0)
    return GLU_OBJECT_PARAMETRIC_ERROR;
  if (strcmp(s, "GLU_OBJECT_PATH_LENGTH") == 0) return GLU_OBJECT_PATH_LENGTH;
  if (strcmp(s, "GLU_PATH_LENGTH") == 0) return GLU_PATH_LENGTH;
  if (strcmp(s, "GLU_PARAMETRIC_ERROR") == 0) return GLU_PARAMETRIC_ERROR;
  if (strcmp(s, "GLU_DOMAIN_DISTANCE") == 0) return GLU_DOMAIN_DISTANCE;
  return 0;
}

//--------------------------------------------------------------------
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

//--------------------------------------------------------------------
GLenum glShaderTypeFromString(const char* s) {
  if (strcmp(s, "GL_FRAGMENT_SHADER") == 0) return GL_FRAGMENT_SHADER;
  if (strcmp(s, "GL_VERTEX_SHADER") == 0) return GL_VERTEX_SHADER;
  return 0;
}

//--------------------------------------------------------------------
const char* glDebugSourceToString(GLenum e) {
  switch (e) {
    case GL_DEBUG_SOURCE_API:
      return "GL_DEBUG_SOURCE_API";
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      return "GL_DEBUG_SOURCE_WINDOW_SYSTEM";
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      return "GL_DEBUG_SOURCE_SHADER_COMPILER";
    case GL_DEBUG_SOURCE_THIRD_PARTY:
      return "GL_DEBUG_SOURCE_THIRD_PARTY";
    case GL_DEBUG_SOURCE_APPLICATION:
      return "GL_DEBUG_SOURCE_APPLICATION";
    case GL_DEBUG_SOURCE_OTHER:
      return "GL_DEBUG_SOURCE_OTHER";
    default:
      return "";
  }
}

//--------------------------------------------------------------------
GLenum glDebugSourceFromString(const char* s) {
  if (strcmp(s, "GL_DEBUG_SOURCE_API") == 0) return GL_DEBUG_SOURCE_API;
  if (strcmp(s, "GL_DEBUG_SOURCE_WINDOW_SYSTEM") == 0)
    return GL_DEBUG_SOURCE_WINDOW_SYSTEM;
  if (strcmp(s, "GL_DEBUG_SOURCE_SHADER_COMPILER") == 0)
    return GL_DEBUG_SOURCE_SHADER_COMPILER;
  if (strcmp(s, "GL_DEBUG_SOURCE_THIRD_PARTY") == 0)
    return GL_DEBUG_SOURCE_THIRD_PARTY;
  if (strcmp(s, "GL_DEBUG_SOURCE_APPLICATION") == 0)
    return GL_DEBUG_SOURCE_APPLICATION;
  if (strcmp(s, "GL_DEBUG_SOURCE_OTHER") == 0) return GL_DEBUG_SOURCE_OTHER;
  return 0;
}

//--------------------------------------------------------------------
const char* glDebugTypeToString(GLenum e) {
  switch (e) {
    case GL_DEBUG_TYPE_ERROR:
      return "GL_DEBUG_TYPE_ERROR";
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      return "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR";
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      return "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR";
    case GL_DEBUG_TYPE_PORTABILITY:
      return "GL_DEBUG_TYPE_PORTABILITY";
    case GL_DEBUG_TYPE_PERFORMANCE:
      return "GL_DEBUG_TYPE_PERFORMANCE";
    case GL_DEBUG_TYPE_OTHER:
      return "GL_DEBUG_TYPE_OTHER";
    case GL_DEBUG_TYPE_MARKER:
      return "GL_DEBUG_TYPE_MARKER";
    case GL_DEBUG_TYPE_PUSH_GROUP:
      return "GL_DEBUG_TYPE_PUSH_GROUP";
    case GL_DEBUG_TYPE_POP_GROUP:
      return "GL_DEBUG_TYPE_POP_GROUP";
    default:
      return "";
  }
}

//--------------------------------------------------------------------
GLenum glDebugTypeFromString(const char* s) {
  if (strcmp(s, "GL_DEBUG_TYPE_ERROR") == 0) return GL_DEBUG_TYPE_ERROR;
  if (strcmp(s, "GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR") == 0)
    return GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR;
  if (strcmp(s, "GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR") == 0)
    return GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR;
  if (strcmp(s, "GL_DEBUG_TYPE_PORTABILITY") == 0)
    return GL_DEBUG_TYPE_PORTABILITY;
  if (strcmp(s, "GL_DEBUG_TYPE_PERFORMANCE") == 0)
    return GL_DEBUG_TYPE_PERFORMANCE;
  if (strcmp(s, "GL_DEBUG_TYPE_OTHER") == 0) return GL_DEBUG_TYPE_OTHER;
  if (strcmp(s, "GL_DEBUG_TYPE_MARKER") == 0) return GL_DEBUG_TYPE_MARKER;
  if (strcmp(s, "GL_DEBUG_TYPE_PUSH_GROUP") == 0)
    return GL_DEBUG_TYPE_PUSH_GROUP;
  if (strcmp(s, "GL_DEBUG_TYPE_POP_GROUP") == 0) return GL_DEBUG_TYPE_POP_GROUP;
  return 0;
}

//--------------------------------------------------------------------
const char* glDebugSeverityToString(GLenum e) {
  switch (e) {
    case GL_DEBUG_SEVERITY_HIGH:
      return "GL_DEBUG_SEVERITY_HIGH";
    case GL_DEBUG_SEVERITY_MEDIUM:
      return "GL_DEBUG_SEVERITY_MEDIUM";
    case GL_DEBUG_SEVERITY_LOW:
      return "GL_DEBUG_SEVERITY_LOW";
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      return "GL_DEBUG_SEVERITY_NOTIFICATION";
    default:
      return "";
  }
}

//--------------------------------------------------------------------
GLenum glDebugSeverityFromString(const char* s) {
  if (strcmp(s, "GL_DEBUG_SEVERITY_HIGH") == 0) return GL_DEBUG_SEVERITY_HIGH;
  if (strcmp(s, "GL_DEBUG_SEVERITY_MEDIUM") == 0)
    return GL_DEBUG_SEVERITY_MEDIUM;
  if (strcmp(s, "GL_DEBUG_SEVERITY_LOW") == 0) return GL_DEBUG_SEVERITY_LOW;
  if (strcmp(s, "GL_DEBUG_SEVERITY_NOTIFICATION") == 0)
    return GL_DEBUG_SEVERITY_NOTIFICATION;
  return 0;
}

//--------------------------------------------------------------------
const char* glPolygonModeToString(GLenum e) {
  switch (e) {
    case GL_POINT:
      return "GL_POINT";
    case GL_LINE:
      return "GL_LINE";
    case GL_FILL:
      return "GL_FILL";
    default:
      return "";
  }
}

//--------------------------------------------------------------------
GLenum glPolygonModeFromString(const char* s) {
  if (strcmp(s, "GL_POINT") == 0) return GL_POINT;
  if (strcmp(s, "GL_LINE") == 0) return GL_LINE;
  if (strcmp(s, "GL_FILL") == 0) return GL_FILL;
  return 0;
}

//--------------------------------------------------------------------
const char* glCullFaceModeToString(GLenum e) {
  switch (e) {
    case GL_FRONT:
      return "GL_FRONT";
    case GL_BACK:
      return "GL_BACK";
    case GL_FRONT_AND_BACK:
      return "GL_FRONT_AND_BACK";
    default:
      return "";
  }
}

//--------------------------------------------------------------------
GLenum glCullFaceModeFromString(const char* s) {
  if (strcmp(s, "GL_FRONT") == 0) return GL_FRONT;
  if (strcmp(s, "GL_BACK") == 0) return GL_BACK;
  if (strcmp(s, "GL_FRONT_AND_BACK") == 0) return GL_FRONT_AND_BACK;
  return 0;
}
