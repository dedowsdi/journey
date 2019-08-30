set(SRC
    src/app.cpp
    src/kci.cpp
    )

add_library(gl43app ${SRC})

find_package(X11 REQUIRED)

target_link_libraries(gl43app
    PUBLIC
        glc43
        ${GLFW3_LIBRARIES}
        ${CMAKE_DL_LIBS}
    )

IF(UNIX)
    # add pkg-config --print-requires-private glfw3
    target_link_libraries(gl43app
        PUBLIC
            X11 Xrandr Xinerama Xxf86vm Xcursor
        )
ENDIF()

target_link_libraries(gl43app PUBLIC ${GLFW3_REQUIRES})

target_include_directories(gl43app
    PUBLIC
    ${GLFW3_INCLUDE_DIRS}
    include
    )

# export build interface
export(TARGETS gl43app NAMESPACE zxd:: FILE gl43appTargets.cmake)
configure_file(gl43appConfig.cmake.in gl43appConfig.cmake @ONLY)

# add to user package registry
export(PACKAGE gl43app)
