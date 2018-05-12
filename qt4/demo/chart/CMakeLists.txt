set(SOURCES
    pieview.cpp
    mainwindow.cpp
    main.cpp
   )

set(MOC_HEADERS
    pieview.h
    mainwindow.h
   )

set(RESOURCES
    chart.qrc
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(chart ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} )
set_target_properties(chart PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(chart Qt4::QtGui Qt4::QtXml)
