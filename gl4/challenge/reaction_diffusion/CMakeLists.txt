set(SRC
    reaction_diffusion.cpp
    pacman.cpp
    )

add_executable(reaction_diffusion ${SRC})
set_target_properties(reaction_diffusion PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(reaction_diffusion
    gl43app
    )
