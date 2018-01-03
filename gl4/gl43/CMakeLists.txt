set(SRCS 
    src/glad.c
    )

add_library(gl43 
    ${SRCS}
    )

IF(NOT WIN32)
    target_link_libraries(gl43
        dl
        )
ENDIF()
