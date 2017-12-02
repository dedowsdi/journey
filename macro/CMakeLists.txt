set (PROJECTS
    stringizing
    variadic_macro
    )

foreach(PROJECT ${PROJECTS})
    message("add macro : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
