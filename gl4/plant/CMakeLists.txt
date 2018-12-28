set (PROJECTS
    turtle
    plant1_5
    )

set(CMAKE_INCLUDE_CURRENT_DIR ON) # include qt generated files
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

#set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} /opt/Qt5.7.0/5.7/gcc_64)

find_package(Qt5Widgets CONFIG REQUIRED)

foreach(PROJECT ${PROJECTS})
    message("add challenge ${PROJECT}")
    #DO
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/CMakeLists.txt)
        add_subdirectory(${PROJECT})
    ELSE()
        add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
        set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
        target_link_libraries(${PROJECT}
            common
            m
            ${Boost_LIBRARIES}
            )
    ENDIF()

endforeach()
