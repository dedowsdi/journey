set (PROJECTS
    tag_dispatch
    enum_hack
    raii
    nvl
    counter
    singleton
    double_dispatch
    virtual_function_table
    )

foreach(PROJECT ${PROJECTS})
    message("add idiom : ${PROJECT}")
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/CMakeLists.txt)
        add_subdirectory(${PROJECT})
    ELSE()
        file(GLOB_RECURSE cmpUnits LIST_DIRECTORIES false  ${PROJECT}/*.cpp ${PROJECT}/*.cxx ${PROJECT}/*.c)
        add_executable(${PROJECT} ${cmpUnits})
        set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
        target_link_libraries(${PROJECT} ${COMMON_LIBS})
    ENDIF()
endforeach()
