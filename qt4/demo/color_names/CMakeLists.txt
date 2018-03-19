set(SOURCES
    color_names_dialog.cpp
    main.cpp
   )

set(MOC_HEADERS
    color_names_dialog.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(color_names ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(color_names PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(color_names Qt4::QtGui Qt4::QtXml)
