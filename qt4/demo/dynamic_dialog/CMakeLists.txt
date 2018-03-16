set(SOURCES
    dynamic_dialog.cpp
   )

set(MOC_HEADERS
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(dynamic_dialog ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(dynamic_dialog PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(dynamic_dialog Qt4::QtGui Qt4::QtXml Qt4::QtUiTools)
