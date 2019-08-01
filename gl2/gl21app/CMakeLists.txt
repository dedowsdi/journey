set(SRC
    src/app.cpp
    )

add_library(gl21app ${SRC})

find_package(X11 REQUIRED)

target_link_libraries(gl21app
    PUBLIC
        glc21
        ${OPENGL_glu_LIBRARY}
    PRIVATE
        ${GLFW3_LIBRARIES}
    )

IF(UNIX)
    # add pkg-config --print-requires-private glfw3
    target_link_libraries(gl21app
        PRIVATE
            X11 Xrandr Xinerama Xxf86vm Xcursor
        )
ENDIF()

target_link_libraries(gl21app PUBLIC ${GLUT_LIBRARIES})

target_include_directories(gl21app
    PUBLIC
    ${GLUT_INCLUDE_DIRS}
    include
    )

# export build interface
export(TARGETS gl21app NAMESPACE zxd:: FILE gl21appTargets.cmake)
configure_file(gl21appConfig.cmake.in gl21appConfig.cmake @ONLY)

# add to user package registry
export(PACKAGE gl21app)
