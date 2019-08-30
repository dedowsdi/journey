set(SRCS
    src/glad.c
    )

add_library(gl43 ${SRCS})

target_include_directories(gl43
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include/gl43>
    )

target_link_libraries(gl43
    PUBLIC
        OpenGL::GL
    )

# export build interface
export(TARGETS gl43 NAMESPACE zxd:: FILE gl43Config.cmake)

# add to user package registry
export(PACKAGE gl43)

# install lib, export install tree target
install(TARGETS gl43
    EXPORT gl43Config
    # INCLUDES DESTINATION include/gl43b
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    )

install(DIRECTORY include/ DESTINATION include/gl43)

# install install tree target
install(EXPORT gl43Config NAMESPACE zxd:: DESTINATION lib/gl43/cmake)
