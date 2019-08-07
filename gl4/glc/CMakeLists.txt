set(GLC_SRCS
    src/bitmap_text.cpp
    src/common.cpp
    src/glenumstring.cpp
    src/glm.cpp
    src/glmath.cpp
    src/color.cpp
    src/light.cpp
    src/program.cpp
    src/common_program.cpp
    src/geometry.cpp
    src/string_util.cpp
    src/stream_util.cpp
    src/texutil.cpp
    src/array.cpp
    src/geometry.cpp
    src/texmap_anim.cpp
    src/dict_script.cpp
    src/mat_stack.cpp
    src/debugger.cpp
    src/primitive_set.cpp
    src/triangle_functor.cpp
    src/geometry_util.cpp
    src/spritesheet.cpp
    src/timer.cpp
    src/pingpong.cpp
    src/movable.cpp
    src/lsystem.cpp
    src/filter.cpp
    src/cycle_path.cpp
    src/fractal_spiral.cpp
    src/geometry_test.cpp
    src/quad.cpp
    src/sphere.cpp
    src/cuboid.cpp
    src/cone.cpp
    src/cylinder.cpp
    src/torus.cpp
    src/xyplane.cpp
    src/disk.cpp
    src/points.cpp
    src/bezier.cpp
    src/nurb.cpp
    src/beziersurface.cpp
    src/nurbsurface.cpp
    src/axes.cpp
    src/line.cpp
    src/super_shape_2d.cpp
    src/super_shape_3d.cpp
    src/circle.cpp
    src/rose.cpp
    src/capsule2d.cpp
    src/lissajous.cpp
    src/teardrop.cpp
    src/turtle.cpp
    src/capsule3d.cpp
    src/icosahedron.cpp
    src/freetype_text.cpp
    src/guard.cpp
    src/jsoncfg.cpp
    )

set(GLC glc${GLC_GL_VERSION})
add_library(${GLC} ${GLC_SRCS})

find_package(Boost 1.58 REQUIRED COMPONENTS filesystem)

target_link_libraries(${GLC}
    PUBLIC
        gl${GLC_GL_VERSION}
        ${FREETYPE_LIBRARIES}
        ${FIP_LIBS}
        ${Boost_LIBRARIES}
    )

target_include_directories(${GLC}
    PUBLIC
        ${Boost_INCLUDE_DIRS}
        ${FREETYPE_INCLUDE_DIR}
        ${FREETYPE_INCLUDE_DIR_ft2build}
        ${FIP_INCLUDE_DIR}
        ${RAPIDJSON_INCLUDE_DIR}
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/utfcpp/source>
        $<INSTALL_INTERFACE:include/${GLC}>
        $<INSTALL_INTERFACE:include/${GLC}/utfcpp>
    )

# create version file
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${GLC}ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

# export build interface
export(TARGETS ${GLC} NAMESPACE zxd:: FILE ${GLC}Targets.cmake)
configure_file(glcConfig.cmake.in ${GLC}Config.cmake @ONLY)

# add to user package registry
export(PACKAGE ${GLC})

# install lib, export install tree target
install(TARGETS ${GLC}
    EXPORT ${GLC}Targets
    # INCLUDES DESTINATION include/${GLC}b
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

install(DIRECTORY include/ DESTINATION include/${GLC})
install(DIRECTORY utfcpp/source/ DESTINATION include/${GLC}/utfcpp)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${GLC}Config.cmake
              ${CMAKE_CURRENT_BINARY_DIR}/${GLC}ConfigVersion.cmake
        DESTINATION lib/${GLC}/cmake)

# install install tree target
install(EXPORT ${GLC}Targets NAMESPACE zxd:: DESTINATION lib/${GLC}/cmake)
