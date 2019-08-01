set(SRCS
    src/glad.c
    src/glad_glx.c
    )

add_library(gl21 ${SRCS})
target_link_libraries(gl21 PRIVATE ${CMAKE_DL_LIBS})

target_include_directories(gl21
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/gl21>
    )

target_link_libraries(gl21
    PUBLIC
    OpenGL::GL
    )

# export build interface
export(TARGETS gl21 NAMESPACE zxd:: FILE gl21Config.cmake)

# add to user package registry
export(PACKAGE gl21)

# install lib, export install tree target
install(TARGETS gl21
    EXPORT gl21Config
    # INCLUDES DESTINATION include/gl21b
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

install(DIRECTORY include/ DESTINATION include/gl21)

# install install tree target
install(EXPORT gl21Config NAMESPACE zxd:: DESTINATION lib/gl21/cmake)
