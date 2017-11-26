set (PROJECTS
    number_file
    )

foreach(PROJECT ${PROJECTS})
    message("add util : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
