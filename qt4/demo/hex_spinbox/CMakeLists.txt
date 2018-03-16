set(SOURCES
   hex_spinbox.cpp
   main.cpp
   )

set(MOC_HEADERS
   hex_spinbox.h
   )


set(RESOURCES
   )

QT4_ADD_RESOURCES( RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP( MOC_SRCS ${MOC_HEADERS})

add_executable(hex_spinbox ${SOURCES} ${MOC_SRCS} ${RES_SOURCES})
set_target_properties(hex_spinbox PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(hex_spinbox Qt4::QtGui Qt4::QtXml)
