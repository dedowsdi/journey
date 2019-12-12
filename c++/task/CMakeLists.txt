set (PROJECTS
    longestcommonsubstring
    )

foreach(PROJECT ${PROJECTS})
    message("add task : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
endforeach()
