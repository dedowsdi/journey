set(SRC
    CollisionCallback.cpp
    )

add_executable(CollisionCallback ${SRC})

target_link_libraries(CollisionCallback PRIVATE
    BulletClip
    ${OPENSCENEGRAPH_LIBRARIES}
    )

target_include_directories(CollisionCallback PRIVATE
    )
