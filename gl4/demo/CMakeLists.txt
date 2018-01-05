set (PROJECTS
    helloworld
    geometry
    deferred
    )

foreach(PROJECT ${PROJECTS})
    message("add demo ${PROJECT}")

    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        m
        common
        )
endforeach()