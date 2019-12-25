set(SRC
    BasicDemo.cpp
    )

add_executable(BasicDemo ${SRC})

target_link_libraries(BasicDemo PRIVATE
    BulletClip
    ${OPENSCENEGRAPH_LIBRARIES}
    )

target_include_directories(BasicDemo PRIVATE
    )
