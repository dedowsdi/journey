set(SOURCES
    rgba_editor.cpp
    main.cpp
   )

set(MOC_HEADERS
    rgba_editor.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(rgba_editor ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(rgba_editor PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(rgba_editor Qt4::QtGui Qt4::QtXml)
