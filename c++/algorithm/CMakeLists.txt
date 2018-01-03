set (PROJECTS
    binary_search
    insert_sort
    merge_sort
    bubble_sort
    heap_sort
    max_subarray
    quick_sort
    nth_element
    count_sort
    )

foreach(PROJECT ${PROJECTS})
    message("add algorithm : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
