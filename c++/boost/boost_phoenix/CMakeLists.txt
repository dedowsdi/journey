set (PROJECTS
    phoenix_tutorial0
    phoenix_tutorial1
    )

foreach(PROJECT ${PROJECTS})
    message("add boost phoenix ${PROJECT}")
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/CMakeLists.txt)
        add_subdirectory(${PROJECT})
    ELSE()
        add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
        set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
        target_link_libraries(${PROJECT}
            ${Boost_LIBRARIES}
            )
    ENDIF()

endforeach()

