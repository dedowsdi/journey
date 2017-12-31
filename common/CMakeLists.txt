set(COMMON_SRCS 
    common.cpp
    program.cpp
    glm.cpp
    glEnumString.cpp
    light.cpp
    sphere.cpp
    texutil.cpp
    bitmap_text.cpp
    freetype_text.cpp
    quad.cpp
    stringutil.cpp
    )

add_library(common 
    ${COMMON_SRCS}
    )

target_link_libraries(common
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${GLUT_LIBRARIES}
    ${GLEW_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FIP_LIBS}
    )
