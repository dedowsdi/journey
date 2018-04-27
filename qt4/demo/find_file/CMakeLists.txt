set(SOURCES
    main.cpp
    find_file_dialog.cpp
   )

set(MOC_HEADERS
    find_file_dialog.h
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(find_file ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(find_file PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(find_file Qt4::QtGui Qt4::QtXml)
