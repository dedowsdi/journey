set (PROJECTS
    manual_test_case
    test_fibonacci
    test_dataset
    test_template
    test_testsuit
    test_decorator
    test_fixture
    )

foreach(PROJECT ${PROJECTS})
    message("add boost spirit ${PROJECT}")
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

file(GLOB xmlFiles spirit_mini_xml/*.toyxml)
file(COPY ${xmlFiles} DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
