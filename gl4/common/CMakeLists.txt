set(COMMON_SRCS 
    src/app.cpp
    src/param.cpp
    )

add_library(common 
    ${COMMON_SRCS}
    )

target_link_libraries(common
    gl43
    glc
    ${OPENGL_gl_LIBRARY}
    ${GLFW3_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FIP_LIBS}
    )
