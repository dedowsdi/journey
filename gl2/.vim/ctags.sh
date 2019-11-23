#/bin/bash
ctags -R --options="${HOME}/.ctags_options/cpp.ctags" \
    /usr/include/GL/freeglut.h \
    /usr/include/GL/glu.h \
    /usr/include/GL/freeglut_ext.h \
    /usr/local/source/glm/glm/ \
    glc/ \
    ./gl21/
