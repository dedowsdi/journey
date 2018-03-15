set(SOURCES
    age_widget.cpp
   )

set(MOC_HEADERS
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_UI(UI_HEADERS ${UIS})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(age_widget ${SOURCES} ${MOC_SRCS} ${RES_SOURCES})
set_target_properties(age_widget PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(age_widget Qt4::QtGui Qt4::QtXml)
