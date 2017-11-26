set (PROJECTS
    visitor
    adapter
    factory
    abstract_factory
    )

foreach(PROJECT ${PROJECTS})
    message("add pattern : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
