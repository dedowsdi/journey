set(SRC
    matrix_rain.cpp
    rain.cpp)

add_executable(matrix_rain ${SRC})
set_target_properties(matrix_rain PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(matrix_rain m common)
