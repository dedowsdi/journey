set(SOURCES
    tetrahedron.cpp
    main.cpp
   )

set(MOC_HEADERS
    tetrahedron.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(tetrahedron ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(tetrahedron PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(tetrahedron 
    Qt4::QtGui
    Qt4::QtXml
    Qt4::QtOpenGL
    ${OPENGL_gl_LIBRARY}
    ${OPENGL_glu_LIBRARY}
    )
