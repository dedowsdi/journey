set (PROJECTS
    starfield
    menger_sponge_fractal
    snake
    purple_rain
    space_invader
    mitosis
    solar
    maze_generator
    noise_terrain
    lorenz_system
    reaction_diffusion
    fractal_tree
    lsystem_tree
    space_colonization_tree
    space_colonization_tree_3d
    superellipse
    supershape
    flow_field
    supershape3d
    firework
    poisson_disk_sample
    diffusion_limited
    lexicograph
    diastic_machine
    pixel_sort
    photo_mosaic
    circle_pack
    astar
    levy_flight
    islamic_pattern
    rose
    attraction_repulsion
    butterfly
    fractal_spirograph
    forward_kinematics
    inverse_kinematics
    shakespeare_monkey
    smart_rocket
    bloop
    )

foreach(PROJECT ${PROJECTS})
    message("add challenge ${PROJECT}")

    add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        m
        common
        ${Boost_LIBRARIES}
        )
endforeach()

file(COPY lorenz_system/lorenz_system.txt DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY supershape3d/supershape3d.txt DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
