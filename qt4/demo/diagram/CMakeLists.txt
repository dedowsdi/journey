set(SOURCES
    diagram_window.cpp
    link.cpp
    main.cpp
    node.cpp
    properties_dialog.cpp
   )

set(MOC_HEADERS
    diagram_window.h
    properties_dialog.h
   )

set(RESOURCES
    resources.qrc
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(diagram ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(diagram PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(diagram Qt4::QtGui Qt4::QtXml)
