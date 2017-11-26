set (PROJECTS
    binary_search
    insert_sort
    merge_sort
    bubble_sort
    )

foreach(PROJECT ${PROJECTS})
    message("add algorithm : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
