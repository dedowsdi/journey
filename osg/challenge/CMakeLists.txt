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
    space_colonization_tree_3d
    )

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DSOURCE=${CMAKE_CURRENT_SOURCE_DIR}"  )

foreach(PROJECT ${PROJECTS})
    message("add challenge ${DPROJECT}")
    add_executable(${PROJECT} ${PROJECT}/${PROJECT}.cpp)
    set_target_properties(${PROJECT} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${PROJECT}
        ${OPENTHREADS_LIBRARY}
        ${OSG_LIBRARY}
        ${OSGDB_LIBRARY}
        ${OSGUTIL_LIBRARY}
        ${OSGVIEWER_LIBRARY}
        ${OSGGA_LIBRARY}
        ${OSGANIMATION_LIBRARY}
        ${OSGTEXT_LIBRARY}
        ${OSGPARTICLE_LIBRARY}
        ${OSGSHADOW_LIBRARY}
        ${OSGFX_LIBRARY}
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
        ${OSGWIDGET_LIBRARY}
        common
        )
endforeach()
