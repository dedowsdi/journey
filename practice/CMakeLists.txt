set (DEMOES
    plane
    universe
    createTexture
    blender
    )

foreach(DEMO ${DEMOES})
    message("add ${DEMO}")
    addCppDirect(./${DEMO} ${DEMO}_SRCS)
    add_executable(${DEMO} ${${DEMO}_SRCS})
    set_target_properties(${DEMO} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${DEMO}
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
        common
        )
endforeach()
