set (PROJECTS
    compile_check
    local_class
    int2type
    select
    type2type
    conversion
    typeinfo
    type_traits
    type_list
    gen_scatter_hierachy
    gen_linear_hierachy
    )

foreach(PROJECT ${PROJECTS})
    message("add trick : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()

add_subdirectory(preload)
