set (PROJS
    basic_sphere
    )

foreach(PROJECT ${PROJS})
    message("add tracing ${PROJECT}")

    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        PRIVATE
            common
            ${OPENSCENEGRAPH_LIBRARIES}
        )
endforeach()
