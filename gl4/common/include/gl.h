#ifndef GL_H
#define GL_H


#ifdef CLANG_COMPLETE_ONLY
    #define GL_GLEXT_PROTOTYPES
    #include <GL/gl.h>
    #include <GL/glext.h>
#else
    #include "glad/glad.h"
#endif

#endif /* GL_H */
