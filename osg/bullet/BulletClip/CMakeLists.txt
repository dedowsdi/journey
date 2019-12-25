set(SRCS
    src/BulletClip.cpp
    src/DebugDrawer.cpp
)

add_library(BulletClip ${SRCS})
target_link_libraries(BulletClip
    PUBLIC
    ${BULLET_LIBRARIES}
    )

target_include_directories(BulletClip
    PUBLIC
    ${BULLET_INCLUDE_DIRS}
    include
    )

#target_link_libraries(common ${EXTRA_LIBS})
