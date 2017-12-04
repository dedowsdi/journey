set (PROJECTS
    circular
    )

foreach(PROJECT ${PROJECTS})
    message("add effect ${PROJECT}")
    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}.c)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
        ${GLUT_LIBRARIES}
        m
        common
        )
endforeach()

