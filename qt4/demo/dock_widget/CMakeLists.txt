set(SOURCES
    main.cpp
   )

set(MOC_HEADERS
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(dock_widget ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(dock_widget PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(dock_widget Qt4::QtGui Qt4::QtXml)
