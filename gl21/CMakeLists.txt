set(SRCS 
    src/glad.c
    )

add_library(gl21 
    ${SRCS}
    )

IF(NOT WIN32)
    target_link_libraries(gl21
        dl
        )
ENDIF()
