set(SRC
    fractal_spirograph.cpp
    )

add_executable(fractal_spirograph ${SRC})
set_target_properties(fractal_spirograph PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(fractal_spirograph
    m
    common
    )
