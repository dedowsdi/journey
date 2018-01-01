#ifndef GLENUMSTRING_H
#define GLENUMSTRING_H

#include "glad/glad.h"

const char* glTexEnvModeToString(GLenum e);
GLenum glTexEnvModeFromString(const char* s);

const char* glTexWrapToString(GLenum e);
GLenum gTexWraplFromString(const char* s);

const char* glTexFilterToString(GLenum e);
GLenum glTexFilterFromString(const char* s);

const char* gluNurbSampleMethodToString(GLenum e);
GLenum gluNurbSampleMethodlFromString(const char* s);

const char* glShaderTypeToString(GLenum e);
GLenum glShaderTypeFromString(const char* s);

const char* glDebugSourceToString(GLenum e);
GLenum glDebugSourceFromString(const char* s);

const char* glDebugTypeToString(GLenum e);
GLenum glDebugTypeFromString(const char* s);

const char* glDebugSeverityToString(GLenum e);
GLenum glDebugSeverityFromString(const char* s);

#endif /* GLENUMSTRING_H */
