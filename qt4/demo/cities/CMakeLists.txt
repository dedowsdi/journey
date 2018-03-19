set(SOURCES
    city_model.cpp
    main.cpp
   )

set(MOC_HEADERS
    city_model.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(cities ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(cities PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(cities Qt4::QtGui Qt4::QtXml)
