set(COMMON_SRCS 
    src/app.cpp
    )

add_library(common 
    ${COMMON_SRCS}
    )

target_link_libraries(common
    gl21
    glc
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${GLUT_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FIP_LIBS}
    X11
    )
