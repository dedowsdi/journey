set (DEMOES
    mask
    stream
    traits
    chrono
    exception
    )

foreach(DEMO ${DEMOES})
    message("add basic : ${DEMO}")
    add_executable(${DEMO} ${DEMO}/${DEMO}.cpp)
    target_link_libraries(${DEMO} ${COMMON_LIBS})
endforeach()
