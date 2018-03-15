set(SOURCES
    find_dialog.cpp
    main.cpp
   )

set(MOC_HEADERS
    find_dialog.h
   )

set(UIS
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_UI(UI_HEADERS ${UIS})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(find_dialog ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS})
set_target_properties(find_dialog PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(find_dialog Qt4::QtGui Qt4::QtXml)
