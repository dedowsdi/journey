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
    mem_fn
    parameter_pack
    stl_sort
    adl
    copy_elision
    copy_swap
    init
    lifetime
    forward
    reference
    transparent
    variable_template
    template_type_deduction
    dependent_name
    unicode
    int_convert
    )

foreach(PROJECT ${PROJECTS})
    message("add basic : ${PROJECT}")
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/CMakeLists.txt)
        add_subdirectory(${PROJECT})
    ELSE()
        file(GLOB_RECURSE cmpUnits LIST_DIRECTORIES false  ${PROJECT}/*.cpp ${PROJECT}/*.cxx ${PROJECT}/*.c)
        add_executable(${PROJECT} ${cmpUnits})
        set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
        target_link_libraries(${PROJECT} ${COMMON_LIBS})
    ENDIF()
endforeach()
