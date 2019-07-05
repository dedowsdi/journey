set(SRC
    main.cpp
    lightning.cpp
    )

add_executable(lightning ${SRC})
set_target_properties(lightning PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(lightning
    gl43app
    )
