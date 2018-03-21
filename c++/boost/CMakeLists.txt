set (PROJECTS
    boost_program_options
    boost_filesystem
    )

foreach(PROJECT ${PROJECTS})
    message("add basic : ${PROJECT}")
    file(GLOB_RECURSE cmpUnits LIST_DIRECTORIES false  ${PROJECT}/*.cpp ${PROJECT}/*.cxx ${PROJECT}/*.c)
    add_executable(${PROJECT} ${cmpUnits})
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
