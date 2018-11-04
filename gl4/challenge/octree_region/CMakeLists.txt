set(SRC
    octree_region.cpp
    otree.cpp
    )

add_executable(octree_region ${SRC})
set_target_properties(octree_region PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(octree_region
    m
    common
    )
