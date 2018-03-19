set(SOURCES
    track_editor.cpp
    track_delegate.cpp
    main.cpp
   )

set(MOC_HEADERS
    track_editor.h
    track_delegate.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(track_editor ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(track_editor PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(track_editor Qt4::QtGui Qt4::QtXml)
