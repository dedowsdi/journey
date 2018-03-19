set(SOURCES
   icon_editor.cpp
   main.cpp
   )

set(MOC_HEADERS
   icon_editor.h
   )


set(RESOURCES
    icon_editor.qrc
   )

QT4_ADD_RESOURCES( RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS})

add_executable(icon_editor ${SOURCES} ${MOC_SRCS} ${RES_SOURCES})
set_target_properties(icon_editor PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(icon_editor Qt4::QtGui Qt4::QtXml)
