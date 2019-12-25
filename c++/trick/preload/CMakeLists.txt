add_library(preload SHARED preload.cpp)
target_link_libraries(preload ${CMAKE_DL_LIBS})
