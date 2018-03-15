set(SOURCES
    quit_button.cpp
   )

set(MOC_HEADERS
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(quit_button ${SOURCES} ${MOC_SRCS} ${RES_SOURCES})
set_target_properties(quit_button PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(quit_button Qt4::QtGui Qt4::QtXml)
