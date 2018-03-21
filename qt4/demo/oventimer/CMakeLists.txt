set(SOURCES
    oventimer.cpp
    main.cpp
   )

set(MOC_HEADERS
    oventimer.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(oventimer ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(oventimer PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(oventimer Qt4::QtGui Qt4::QtXml)
