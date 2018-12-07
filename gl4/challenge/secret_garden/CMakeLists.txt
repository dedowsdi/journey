set(SRC
    secret_garden.cpp
    bug.cpp
    )

add_executable(secret_garden ${SRC})
set_target_properties(secret_garden PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(secret_garden
    m
    common
    )
