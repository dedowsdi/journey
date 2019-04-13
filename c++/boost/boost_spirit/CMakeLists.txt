set (PROJECTS
    spirit_tutorial0
    spirit_tutorial1
    spirit_tutorial2
    spirit_tutorial3
    roman_numeral
    spirit_string
    spirit_employee
    spirit_mini_xml
    spirit_arithmetic
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
