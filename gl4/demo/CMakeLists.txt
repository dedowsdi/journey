set (PROJECTS
    helloworld
    geometry
    deferred
    lines
    normalmap
    parallaxmap
    normalviewer
    billboard
    particle
    sinewave
    floodfill
    threshold
    plot
    fs
    texture_animation
    prefix_sum_1d
    tessellate
    teapot
    )

foreach(PROJECT ${PROJECTS})
    message("add demo ${PROJECT}")

    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        PRIVATE
            gl43app
        )
endforeach()
