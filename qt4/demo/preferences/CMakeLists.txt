set(SOURCES
    main.cpp
    preference_dialog.cpp
   )

set(MOC_HEADERS
    preference_dialog.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(preference ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(preference PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(preference Qt4::QtGui Qt4::QtXml)
