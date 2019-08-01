set (PROJECTS
    matrix
    bitmap_font
    )

foreach(PROJECT ${PROJECTS})
    message("add util ${PROJECT}")
    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        PRIVATE
            gl21app
        )
endforeach()

file(COPY bitmap_content DESTINATION ${CMAKE_BINARY_DIR})
