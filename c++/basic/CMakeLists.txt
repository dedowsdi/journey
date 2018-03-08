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
    template_sep_def
    function_pointer
    float
    cast
    value
    functional
    printf
    static
    array
    )

foreach(PROJECT ${PROJECTS})
    message("add basic : ${PROJECT}")
    file(GLOB_RECURSE cmpUnits LIST_DIRECTORIES false  ${PROJECT}/*.cpp ${PROJECT}/*.cxx ${PROJECT}/*.c)
    add_executable(${PROJECT} ${cmpUnits})
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS})
endforeach()
