set(SOURCES
    settings_viewer.cpp
    main.cpp
   )

set(MOC_HEADERS
    settings_viewer.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(settings_viewer ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(settings_viewer PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(settings_viewer Qt4::QtGui Qt4::QtXml)
