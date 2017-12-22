#/bin/bash
ctags \
    -R \
    --sort=foldcase \
    --links=yes  \
    --fields=KsSi \
    --excmd=number \
    --c++-kinds=+p \
    --c-kinds=+p \
    --exclude=build \
    --exclude=cmake \
    --exclude=CMake \
    --exclude=CMakeLists \
    --language-force=c \
    /usr/include/GL/gl.h \
    /usr/include/GL/glext.h \
    /usr/include/GL/freeglut.h \
    /usr/include/GL/glu.h \
    /usr/include/GL/freeglut_ext.h \
    /usr/include/GL/glew.h \
    /usr/local/source/glm/glm/

    
