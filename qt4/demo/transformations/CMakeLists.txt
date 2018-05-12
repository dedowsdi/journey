set(SOURCES
    renderarea.cpp
    window.cpp
    main.cpp
   )

set(MOC_HEADERS
    window.h
    renderarea.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(transformations ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(transformations PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(transformations Qt4::QtGui Qt4::QtXml)
