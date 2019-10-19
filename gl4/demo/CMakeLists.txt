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
    classic_lighting
    texture
    rotation
    depthoffield
    mirror
    )

foreach(PROJECT ${PROJECTS})
    message("add demo ${PROJECT}")

    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        PRIVATE
            gl43app
        )

    # copy project_data to target file dir.
    set(${PROJECT}_data ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}_data)
    IF(EXISTS ${${PROJECT}_data})

        # *_BUILD custom_command only executed when target get build, but i
        # want data copied no matter target get build or not
        #
        # add_custom_command(TARGET ${PROJECT} POST_BUILD COMMAND
        #     ${CMAKE_COMMAND} -E copy_directory ${${PROJECT}_data}
        #     $<TARGET_FILE_DIR:${PROJECT}>/${PROJECT}_data)

        # you can not use $<TARGET_FIL_DIR:...> here, it cause cycle dependencies error
        add_custom_target(${PROJECT}_data COMMAND
              ${CMAKE_COMMAND} -E copy_directory ${${PROJECT}_data}
              ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT}_data)
        add_dependencies(${PROJECT} ${PROJECT}_data)
        message("depend ${PROJECT} on ${PROJECT}_data")
    ENDIF()
endforeach()
