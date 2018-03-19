set(SOURCES
    coordinate_setter.cpp
    main.cpp
   )

set(MOC_HEADERS
    coordinate_setter.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(coordinate_setter ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(coordinate_setter PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(coordinate_setter Qt4::QtGui Qt4::QtXml)
