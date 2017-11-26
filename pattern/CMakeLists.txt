set (PROJECTS
    builder
    factory
    abstract_factory
	prototype
    singleton
    visitor
    adapter
    facade
    template_method
    )

foreach(PROJECT ${PROJECTS})
    message("add pattern : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
