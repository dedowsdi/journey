set (SRC
    main.cpp
    counter.cpp
    derived0.cpp
    derived1.cpp
    )

add_executable(counter ${SRC})
set_target_properties(counter PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(counter ${COMMON_LIBS})
