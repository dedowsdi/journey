set (PROJECTS
    mask
    stream
    fstream
    traits
    chrono
    exception
    variadic_function
    sizeof
    data_align
    template_overload
    template_nontype
    function_pointer
    float
    cast
    value
    functional
    printf
    static
    )

foreach(PROJECT ${PROJECTS})
    message("add basic : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS})
endforeach()
