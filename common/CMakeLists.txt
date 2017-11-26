addCppDirect(./src COMMON_SRCS)
add_library(common ${COMMON_SRCS})
target_link_libraries(common ${EXTRA_LIBS})
