set (PROJECTS
    builder
    factory
    abstract_factory
	prototype
    singleton
    visitor
    adapter
    facade
    bridge
    template_method
    chains_responsibility
    compositor
    flyweight
	decorator
    proxy
    mixin
    command
    memento
    interpreter
    mediator
    observer
    state
    strategy
    crtp
    )

foreach(PROJECT ${PROJECTS})
    message("add pattern : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
