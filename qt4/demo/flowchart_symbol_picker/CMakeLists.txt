set(SOURCES
    flowchart_symbol_picker.cpp
    main.cpp
   )

set(MOC_HEADERS
    flowchart_symbol_picker.h
   )

set(RESOURCES
    flowchart_symbol_picker.qrc
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(flowchart_symbol_picker ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(flowchart_symbol_picker PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(flowchart_symbol_picker Qt4::QtGui Qt4::QtXml)
