set(COMMON_SRCS 
    src/app.cpp
    src/kci.cpp
    )

add_library(common 
    ${COMMON_SRCS}
    )

target_link_libraries(common
    glc
    gl43
    ${GLFW3_LIBRARIES}
    ${FREETYPE_LIBRARIES}
    ${FIP_LIBS}
    ${OPENGL_gl_LIBRARY}
    X11
    Xi
    Xrandr
    Xxf86vm
    Xinerama
    Xcursor
    )
