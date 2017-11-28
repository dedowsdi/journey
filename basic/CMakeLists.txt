set (PROJECTS
    mask
    stream
    traits
    chrono
    exception
    )

foreach(PROJECT ${PROJECTS})
    message("add basic : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS})
endforeach()
