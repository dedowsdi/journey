# some bullet code cause this warning
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-reorder")

add_subdirectory(BulletClip)
add_subdirectory(BasicDemo)
add_subdirectory(CollisionCallback)
