set(SOURCES
    gotocell_dialog.cpp
   )

set(MOC_HEADERS
    gotocell_dialog.h
   )

set(UIS
    ${QUI}/gotocell_dialog.ui
   )

set(RESOURCES
   )

QT4_ADD_RESOURCES(RES_SOURCES ${RESOURCES})
QT4_WRAP_UI(UI_HEADERS ${UIS})
QT4_WRAP_CPP(MOC_SRCS ${MOC_HEADERS})

message("source_dir ${CMAKE_CURRENT_SOURCE_DIR}")
message("UI_HEADERS ${UI_HEADERS}")

add_executable(gotocell_dialog ${SOURCES} ${MOC_SRCS} ${RES_SOURCES} ${UI_HEADERS})
set_target_properties(gotocell_dialog PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})
target_link_libraries(gotocell_dialog Qt4::QtGui Qt4::QtXml)
