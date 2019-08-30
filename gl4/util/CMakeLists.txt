set (PROJECTS
    rgb
    mvp
    )

foreach(PROJECT ${PROJECTS})
    message("add util ${PROJECT}")

    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        PRIVATE
            gl43app
        )

endforeach()


IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    install(TARGETS mvp RUNTIME DESTINATION /usr/local/bin)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
