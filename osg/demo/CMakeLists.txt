set (DEMOES
    borderline
    helloworld
    log
    shape
    quad
    octahedron
    tessellate
    faceCollector
    transform
    switch
    lod
    proxy
    pagedlod
    animSwitch
    infoVisitor
    polygonMode
    stateAttrib
    fog
    light
    texture
    translucent
    cartoonCow
    cubicBezier
    hudCamera
    loop
    compositeViewer
    multiSample
    rtt
    updateCallback
    dynamicGeometry
    animPath
    fadein
    imageSequence
    driveCessna
    timerEvent
    pick
    cube
    positionAttitudeTransform
    lightAttribute
    upsidedown
    windowMode
    extrude
    revolution
    highlight
    animChannel
    serializer
    billboard
    billboardTree
    text
    text3d
    particleFountain
    particleRotarySprinkler
    shadow
    outline
    threadHelloWorld
    threadSwitch
    occludeCull
    shareTexture
    terrain
    clone
    boundingbox
    car
    mirrorScene
    bfsVisitor
    backgroundImage
    billboardNode
    compass
    nurbSurface
    clock
    ribbon
    selectFace
    selectPoint
    drawInstanced
    multiScreen
    fourView
    followModel
    trackerManipulator
    door
    movie
    scrollingText
    gimbal
    euler
    gridfloor
    axis
    cursor
    cameraManipulator2D
    fixVerticalUp
    test
    animPivot
    fx_transparency
    )

foreach(DEMO ${DEMOES})
    message("add ${DEMO}")
    add_executable(${DEMO} ${DEMO}/${DEMO}.cpp)
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
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
        ${OSGWIDGET_LIBRARY}
        common
        )
endforeach()

set(QT_DEMOES
   qt4_single
   qt4_qthread
   qt4_widgets
   )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
foreach(DEMO ${QT_DEMOES})
    message("add ${DEMO}")
    add_executable(${DEMO} ${DEMO}/${DEMO}.cpp)
    set_target_properties(${DEMO} PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
    target_link_libraries(${DEMO}
        Qt4::QtGui Qt4::QtXml
        ${OPENGL_gl_LIBRARY}
        ${OPENGL_glu_LIBRARY}
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
        ${OSGWIDGET_LIBRARY}
        ${OSGQT_LIBRARY}
        common
        X11
        )
endforeach()

add_library(osgdb_tri SHARED osgdb_tri/osgdb_tri.cpp)
