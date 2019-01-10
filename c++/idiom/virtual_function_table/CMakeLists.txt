set (SRC
    main.cpp
    vft.cpp
    collision_manager.cpp
    )

add_executable(virtual_function_table ${SRC})
set_target_properties(virtual_function_table PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(virtual_function_table ${COMMON_LIBS})
