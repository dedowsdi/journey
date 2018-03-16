set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/../find_dialog/find_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../gotocell_dialog/gotocell_dialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../sort_dialog/sort_dialog.cpp
    main_window.cpp
    spreadsheet.cpp
    cell.cpp
    main.cpp
   )

set(MOC_HEADERS
    ${CMAKE_CURRENT_SOURCE_DIR}/../find_dialog/find_dialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../gotocell_dialog/gotocell_dialog.h
    ${CMAKE_CURRENT_SOURCE_DIR}/../sort_dialog/sort_dialog.h
    spreadsheet.h
    main_window.h
   )

set(RESOURCES
    ${CMAKE_SOURCE_DIR}/data/spreadsheet.qrc
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_UI(UI_HEADERS ${UIS})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

add_executable(main_window ${SOURCES} ${MOC_SRCS} ${RES_SOURCES})
set_target_properties(main_window PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(main_window Qt4::QtGui Qt4::QtXml)
