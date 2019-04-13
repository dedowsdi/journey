set (PROJECTS
    boost_program_options
    boost_filesystem
    boost_iostreams
    boost_spirit
    boost_phoenix
    boost_test
    )

foreach(PROJECT ${PROJECTS})
    message("add basic : ${PROJECT}")
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/CMakeLists.txt)
        add_subdirectory(${PROJECT})
    ELSE()
        file(GLOB_RECURSE cmpUnits LIST_DIRECTORIES false  ${PROJECT}/*.cpp ${PROJECT}/*.cxx ${PROJECT}/*.c)
        add_executable(${PROJECT} ${cmpUnits})
        set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
        target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
    ENDIF()
endforeach()
