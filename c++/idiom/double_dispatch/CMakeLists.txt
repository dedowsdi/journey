set (SRC
    main.cpp
    double_dispatch.cpp
    )

add_executable(double_dispatch ${SRC})
set_target_properties(double_dispatch PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(double_dispatch ${COMMON_LIBS})
