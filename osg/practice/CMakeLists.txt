set (DEMOES
    plane
    universe
    createTexture
    blender
    bezier
    spline
    nurb
    )

foreach(DEMO ${DEMOES})
    message("add ${DEMO}")
    addCppDirect(./${DEMO} ${DEMO}_SRCS)
    add_executable(${DEMO} ${${DEMO}_SRCS})
    set_target_properties(${DEMO} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${DEMO}
        common
        osg3::osgViewer
        osg3::osgFX
        osg3::osgWidget
        )
endforeach()
