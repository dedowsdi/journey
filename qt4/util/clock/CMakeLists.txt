set(SOURCES
    clock_dialog.cpp
    main.cpp
   )

set(MOC_HEADERS
    clock_dialog.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

file(COPY bell.wave DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

add_executable(clock ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(clock PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(clock Qt4::QtGui Qt4::QtXml Qt4::phonon)
