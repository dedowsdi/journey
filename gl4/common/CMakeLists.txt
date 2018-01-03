set(COMMON_SRCS 
    src/app.cpp
    src/glEnumString.cpp
    )

add_library(common 
    ${COMMON_SRCS}
    )

target_link_libraries(common
    gl43
    ${OPENGL_gl_LIBRARY}
    ${GLFW3_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FIP_LIBS}
    )
