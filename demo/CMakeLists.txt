#set(RUN_DIR ${PROJECT_SOURCE_DIR}/bin)

MACRO(SUBDIRLIST result targetdir)
    FILE(GLOB children RELATIVE ${targetdir} ${targetdir}/*)
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${targetdir}/${child})
            LIST(APPEND dirlist ${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()

#don't do this, cmake can't detect change this way
#SUBDIRLIST(demos .)

set(demos
    singlePoint
    singleTriangle
    singleCube
    scissor
    multipleViewport
    plane
    )

foreach(demo ${demos})
    message(STATUS "add demo ${demo}")
    add_executable(${demo} ${demo}/${demo}.cpp)
    target_link_libraries(${demo} common)
endforeach(demo)
