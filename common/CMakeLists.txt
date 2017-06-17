addCppDirect(./src COMMON_SRCS)
add_library(common SHARED ${COMMON_SRCS})
#target_link_libraries(common ${EXTRA_LIBS})
