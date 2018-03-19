set(SOURCES
    boolean_model.cpp
    boolean_parser.cpp
    boolean_window.cpp
    main.cpp
    node.cpp
   )

set(MOC_HEADERS
    boolean_window.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(boolean_parser ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(boolean_parser PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(boolean_parser Qt4::QtGui Qt4::QtXml)
