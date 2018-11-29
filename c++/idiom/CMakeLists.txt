set (PROJECTS
    tag_dispatch
    enum_hack
    )

foreach(PROJECT ${PROJECTS})
    message("add quiz : ${PROJECT}")
    file(GLOB_RECURSE cmpUnits LIST_DIRECTORIES false  ${PROJECT}/*.cpp ${PROJECT}/*.cxx ${PROJECT}/*.c)
    add_executable(${PROJECT} ${cmpUnits})
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS})
endforeach()
