set(COMMON_SRCS 
    src/common.cpp
    src/program.cpp
    src/glm.cpp
    src/glEnumString.cpp
    src/light.cpp
    src/sphere.cpp
    src/texutil.cpp
    src/bitmap_text.cpp
    src/freetype_text.cpp
    src/quad.cpp
    src/stringutil.cpp
    )

add_library(common 
    ${COMMON_SRCS}
    )

target_link_libraries(common
    gl21
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    ${GLUT_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FIP_LIBS}
    )
