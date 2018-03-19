set(SOURCES
    startup_dialog.cpp
    main.cpp
   )

set(MOC_HEADERS
    startup_dialog.h
   )

set(RESOURCES
    openmw_startup.qrc
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(openmw_startup ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(openmw_startup PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(openmw_startup Qt4::QtGui Qt4::QtXml)
