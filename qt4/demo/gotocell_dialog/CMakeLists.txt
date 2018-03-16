set(SOURCES
    gotocell_dialog.cpp
    main.cpp
   )

set(MOC_HEADERS
    gotocell_dialog.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(gotocell_dialog ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(gotocell_dialog PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(gotocell_dialog Qt4::QtGui Qt4::QtXml)
