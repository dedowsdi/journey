IF(EXISTS tmp/tmp.cpp)
    set (PROJECTS
        )
ELSE()
    set(PROJECTS 
        tmp
        )
ENDIF()

foreach(PROJECT ${PROJECTS})
    message("add tmp : ${PROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT} ${COMMON_LIBS} ${Boost_LIBRARIES})
endforeach()
