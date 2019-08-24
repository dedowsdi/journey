set (PROJECTS
    helloworld
    geometry
    deferred
    line_stipple
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
    flat
    trackball
    triangle_random_point
    bounding_sphere
    )

foreach(PROJECT ${PROJECTS})
    message("add demo ${PROJECT}")

    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        PRIVATE
            gl43app
        )

    IF(EXISTS ${${PROJECT}_json_path})
        add_custom_target(${PROJECT}_json COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${${PROJECT}_json_path}
            ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT}.json
            )
        add_dependencies(${PROJECT} ${PROJECT}_json)
    ENDIF()

endforeach()
