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
    clock
    movie_recommender
    recursion
    cube_wave
    knot
    snow
    floyd_steinberg_dithering
    double_pendulum
    octree_point
    octree_region
    ripple
    fire
    sandpiles
    barnsley_fern
    recaman_sequence
    spritesheet
    hypercube
    lissajous
    seven_segment
    mandelbrot_set
    evolutionary_steer
    lightning
    chaos
    secret_garden
    eyeball
    teardrop
    matrix_rain
    blobby3d
    text_starfield
    )

foreach(PROJECT ${PROJECTS})
    message("add challenge ${PROJECT}")
    #DO
    IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/CMakeLists.txt)
        add_subdirectory(${PROJECT})
    ELSE()
        add_executable(${PROJECT} ${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT}/${PROJECT}.cpp)
        set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
        target_link_libraries(${PROJECT}
            PRIVATE
                gl43app
            )
    ENDIF()

endforeach()

file(COPY lorenz_system/lorenz_system.txt DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
file(COPY supershape3d/supershape3d.txt DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
