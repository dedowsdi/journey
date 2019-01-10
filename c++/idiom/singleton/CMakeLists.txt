set (SRC
    main.cpp
    singleton.cpp
    derived0.cpp
    )

add_executable(singleton_base ${SRC})
set_target_properties(singleton_base PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(singleton_base ${COMMON_LIBS})
