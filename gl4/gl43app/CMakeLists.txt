set(SRC
    src/app.cpp
    src/kci.cpp
    ${CMAKE_SOURCE_DIR}/gl43/src/glad.c
    ${CMAKE_SOURCE_DIR}/glc/src/bitmap_text.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/common.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/glenumstring.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/glm.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/glmath.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/color.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/light.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/program.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/common_program.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/geometry.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/string_util.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/stream_util.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/texutil.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/geometry.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/texmap_anim.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/dict_script.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/mat_stack.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/debugger.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/primitive_set.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/triangle_functor.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/geometry_util.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/spritesheet.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/timer.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/pingpong.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/movable.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/lsystem.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/filter.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/cycle_path.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/fractal_spiral.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/geometry_test.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/quad.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/sphere.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/cuboid.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/cone.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/cylinder.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/torus.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/xyplane.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/disk.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/points.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/bezier.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/nurb.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/beziersurface.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/nurbsurface.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/axes.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/line.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/super_shape_2d.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/super_shape_3d.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/circle.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/rose.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/capsule2d.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/lissajous.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/teardrop.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/turtle.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/capsule3d.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/icosahedron.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/freetype_text.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/guard.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/jsoncfg.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/camman.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/common_camman.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/glfw_handler.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/objman.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/shader.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/exception.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/vao.cpp
    ${CMAKE_SOURCE_DIR}/glc/src/buffer.cpp
    ${CMAKE_SOURCE_DIR}/)

add_library(gl43app ${SRC})

find_package(X11 REQUIRED)
find_package(Boost REQUIRED COMPONENTS filesystem)

IF(UNIX)
    # add pkg-config --print-requires-private glfw3
    target_link_libraries(gl43app
        PUBLIC
            X11 Xrandr Xinerama Xxf86vm Xcursor
        )
ENDIF()

target_link_libraries(gl43app
    PUBLIC
        ${GLFW3_LIBRARIES}
        ${GLFW3_REQUIRES}
        ${GLAD_GL_LIB}
        OpenGL::GL
        ${FREETYPE_LIBRARIES}
        ${FIP_LIBS}
        ${Boost_LIBRARIES}
        ${CMAKE_DL_LIBS}
    )

target_include_directories(gl43app
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/gl43app/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/gl43/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/glc/include>
        $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/glc/utfcpp/source>
        ${GLFW3_INCLUDE_DIRS}
        ${Boost_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIR}
        ${FREETYPE_INCLUDE_DIR_ft2build}
        ${FIP_INCLUDE_DIR}
        ${RAPIDJSON_INCLUDE_DIR}
    )

# export build interface

# cmake can distinguish package by build type, you must add debug postfix to
# make it work.
IF(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(EXPORT_POSTFIX -d)
ENDIF()

export(TARGETS gl43app NAMESPACE zxd:: FILE gl43appTargets.cmake)
configure_file(gl43appConfig.cmake.in gl43app${EXPORT_POSTFIX}Config.cmake @ONLY)

# add to user package registry
export(PACKAGE gl43app${EXPORT_POSTFIX})
